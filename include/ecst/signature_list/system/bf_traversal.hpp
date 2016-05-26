// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#pragma once

#include <ecst/config.hpp>
#include <ecst/aliases.hpp>
#include <ecst/mp/list.hpp>

ECST_SIGNATURE_LIST_SYSTEM_NAMESPACE
{
    namespace bf_traversal
    {
        using namespace mp;

        template <typename TStartNode>
        constexpr auto make(TStartNode&& sn) noexcept
        {
            return bh::make_pair(list::make(FWD(sn)), list::empty_v);
        }

        template <typename TBFTContext>
        constexpr auto queue(TBFTContext&& c) noexcept
        {
            return bh::first(FWD(c));
        }

        template <typename TBFTContext>
        constexpr auto visited(TBFTContext&& c) noexcept
        {
            return bh::second(FWD(c));
        }

        template <typename TBFTContext, typename TNode>
        constexpr auto is_visited(TBFTContext&& c, TNode&& n) noexcept
        {
            return bh::contains(visited(FWD(c)), FWD(n));
        }

        template <typename TBFTContext, typename TNode>
        constexpr auto is_in_queue(TBFTContext&& c, TNode&& n) noexcept
        {
            return bh::contains(queue(FWD(c)), FWD(n));
        }

        template <typename TBFTContext>
        constexpr auto is_queue_empty(TBFTContext&& c)
        {
            return bh::is_empty(queue(FWD(c)));
        }

        template <typename TBFTContext>
        constexpr auto top_node(TBFTContext&& c)
        {
            return bh::front(queue(FWD(c)));
        }

        template <typename TBFTContext, typename TSSL>
        auto step_forward(TBFTContext&& c, TSSL ssl)
        {
            auto node = top_node(c);
            auto popped_queue = bh::remove_at(queue(c), mp::sz_v<0>);
            auto neighbors =
                dependent_ids_list(ssl, signature_by_id(ssl, node));

            auto unvisited_neighbors = bh::remove_if(neighbors, [=](auto x_nbr)
                {
                    return is_visited(c, x_nbr);
                });

            auto new_visited = bh::concat(visited(c), unvisited_neighbors);
            auto new_queue = bh::concat(popped_queue, unvisited_neighbors);

            return bh::make_pair(new_queue, new_visited);
        }

        template <typename TStartNode, typename TSSL>
        auto bf_traversal_impl(TStartNode&& sn, TSSL ssl)
        {
            using namespace mp;
            namespace btfc = bf_traversal;

            // TODO: is this a fold?
            auto step = [=](auto self, auto ctx)
            {
                return static_if(btfc::is_queue_empty(ctx))
                    .then([=](auto)
                        {
                            return list::empty_v;
                        })
                    .else_([=](auto x_ctx)
                        {
                            return bh::append(
                                self(btfc::step_forward(x_ctx, ssl)),
                                btfc::top_node(x_ctx));
                        })(ctx);
            };

            return vrmc::y_combinator(step)(btfc::make(FWD(sn)));
        }
    }
}
ECST_SIGNATURE_LIST_SYSTEM_NAMESPACE_END
