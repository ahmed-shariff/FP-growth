#ifndef TUTORIALS_FPTREE_H
#define TUTORIALS_FPTREE_H

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cassert>

#define ForAll(T)       template <typename T>

ForAll(Item) using Transaction           = std::vector<Item>;
ForAll(Item) using TransformedPrefixPath = std::pair<std::vector<Item>, uint64_t>;
ForAll(Item) using Pattern               = std::pair<std::set<Item>, uint64_t>;

ForAll(Item) struct FPNode {
    const Item                           item;
    uint64_t                             frequency;
    std::shared_ptr<FPNode<Item>>        node_link;
    std::weak_ptr<FPNode<Item>>          parent;
    std::vector<std::shared_ptr<FPNode>> children;

    FPNode(const Item& item, const std::shared_ptr<FPNode<Item>>& parent) :
            item( item ), frequency( 1 ), node_link( nullptr ), parent( parent ), children() {
    }
};

ForAll(Item) struct FPTree {
    std::shared_ptr<FPNode<Item>>                 root;
    std::map<Item, std::shared_ptr<FPNode<Item>>> header_table;
    uint64_t                                      minimum_support_threshold;

    // order items by decreasing frequency
    struct frequency_comparator
    {
        bool operator()(const std::pair<Item, uint64_t> &lhs, const std::pair<Item, uint64_t> &rhs) const
        {
            return std::tie(lhs.second, lhs.first) > std::tie(rhs.second, rhs.first);
        }
    };

    FPTree(const std::vector<Transaction<Item>>& transactions, uint64_t minimum_support_threshold) :
            root(std::make_shared<FPNode<Item>>( Item{}, nullptr )), header_table(),
            minimum_support_threshold( minimum_support_threshold ) {
        // scan the transactions counting the frequency of each item
        std::map<Item, uint64_t> frequency_by_item;
        for ( const Transaction<Item>& transaction : transactions ) {
            for ( const Item& item : transaction ) {
                ++frequency_by_item[item];
            }
        }

        // keep only items which have a frequency greater or equal than the minimum support threshold
        for ( auto it = frequency_by_item.cbegin(); it != frequency_by_item.cend(); ) {
            const uint64_t item_frequency = (*it).second;
            if ( item_frequency < minimum_support_threshold ) { frequency_by_item.erase( it++ ); }
            else { ++it; }
        }


        std::set<std::pair<Item, uint64_t>, frequency_comparator> items_ordered_by_frequency(frequency_by_item.cbegin(), frequency_by_item.cend());

        // start tree construction

        // scan the transactions again
        for ( const Transaction<Item>& transaction : transactions ) {
            auto curr_fpnode = root;

            // select and sort the frequent items in transaction according to the order of items_ordered_by_frequency
            for ( const auto& pair : items_ordered_by_frequency ) {
                const Item& item = pair.first;

                // check if item is contained in the current transaction
                if ( std::find( transaction.cbegin(), transaction.cend(), item ) != transaction.cend() ) {
                    // insert item in the tree

                    // check if curr_fpnode has a child curr_fpnode_child such that curr_fpnode_child.item = item
                    const auto it = std::find_if(
                            curr_fpnode->children.cbegin(), curr_fpnode->children.cend(),  [item](const std::shared_ptr<FPNode<Item>>& fpnode) {
                                return fpnode->item == item;
                            } );
                    if ( it == curr_fpnode->children.cend() ) {
                        // the child doesn't exist, create a new node
                        const auto curr_fpnode_new_child = std::make_shared<FPNode<Item>>( item, curr_fpnode );

                        // add the new node to the tree
                        curr_fpnode->children.push_back( curr_fpnode_new_child );

                        // update the node-link structure
                        if ( header_table.count( curr_fpnode_new_child->item ) ) {
                            auto prev_fpnode = header_table[curr_fpnode_new_child->item];
                            while ( prev_fpnode->node_link ) { prev_fpnode = prev_fpnode->node_link; }
                            prev_fpnode->node_link = curr_fpnode_new_child;
                        }
                        else {
                            header_table[curr_fpnode_new_child->item] = curr_fpnode_new_child;
                        }

                        // advance to the next node of the current transaction
                        curr_fpnode = curr_fpnode_new_child;
                    }
                    else {
                        // the child exist, increment its frequency
                        auto curr_fpnode_child = *it;
                        ++curr_fpnode_child->frequency;

                        // advance to the next node of the current transaction
                        curr_fpnode = curr_fpnode_child;
                    }
                }
            }
        }
    }

    bool empty() const {
        assert( root );
        return root->children.size() == 0;
    }
};

ForAll(Item) bool contains_single_path(const std::shared_ptr<FPNode<Item>>& fpnode) {
    assert( fpnode );
    if ( fpnode->children.size() == 0 ) { return true; }
    if ( fpnode->children.size() > 1 ) { return false; }
    return contains_single_path( fpnode->children.front() );
}

ForAll(Item) bool contains_single_path(const FPTree<Item>& fptree) {
    return fptree.empty() || contains_single_path( fptree.root );
}

ForAll(Item) std::set<Pattern<Item>> fptree_growth(const FPTree<Item>& fptree) {
    if ( fptree.empty() ) { return {}; }

    if ( contains_single_path( fptree ) ) {
        // generate all possible combinations of the items in the tree

        std::set<Pattern<Item>> single_path_patterns;

        // for each node in the tree
        assert( fptree.root->children.size() == 1 );
        auto curr_fpnode = fptree.root->children.front();
        while ( curr_fpnode ) {
            const Item& curr_fpnode_item = curr_fpnode->item;
            const uint64_t curr_fpnode_frequency = curr_fpnode->frequency;

            // add a pattern formed only by the item of the current node
            Pattern<Item> new_pattern{ { curr_fpnode_item }, curr_fpnode_frequency };
            single_path_patterns.insert( new_pattern );

            // create a new pattern by adding the item of the current node to each pattern generated until now
            for ( const Pattern<Item>& pattern : single_path_patterns ) {
                Pattern<Item> new_pattern{ pattern };
                new_pattern.first.insert( curr_fpnode_item );
                assert( curr_fpnode_frequency <= pattern.second );
                new_pattern.second = curr_fpnode_frequency;

                single_path_patterns.insert( new_pattern );
            }

            // advance to the next node until the end of the tree
            assert( curr_fpnode->children.size() <= 1 );
            if ( curr_fpnode->children.size() == 1 ) { curr_fpnode = curr_fpnode->children.front(); }
            else { curr_fpnode = nullptr; }
        }

        return single_path_patterns;
    }
    else {
        // generate conditional fptrees for each different item in the fptree, then join the results

        std::set<Pattern<Item>> multi_path_patterns;

        // for each item in the fptree
        for ( const auto& pair : fptree.header_table ) {
            const Item& curr_item = pair.first;

            // build the conditional fptree relative to the current item

            // start by generating the conditional pattern base
            std::vector<TransformedPrefixPath<Item>> conditional_pattern_base;

            // for each path in the header_table (relative to the current item)
            auto path_starting_fpnode = pair.second;
            while ( path_starting_fpnode ) {
                // construct the transformed prefix path

                // each item in th transformed prefix path has the same frequency (the frequency of path_starting_fpnode)
                const uint64_t path_starting_fpnode_frequency = path_starting_fpnode->frequency;

                auto curr_path_fpnode = path_starting_fpnode->parent.lock();
                // check if curr_path_fpnode is already the root of the fptree
                if ( curr_path_fpnode->parent.lock() ) {
                    // the path has at least one node (excluding the starting node and the root)
                    TransformedPrefixPath<Item> transformed_prefix_path{ {}, path_starting_fpnode_frequency };

                    while ( curr_path_fpnode->parent.lock() ) {
                        assert( curr_path_fpnode->frequency >= path_starting_fpnode_frequency );
                        transformed_prefix_path.first.push_back( curr_path_fpnode->item );

                        // advance to the next node in the path
                        curr_path_fpnode = curr_path_fpnode->parent.lock();
                    }

                    conditional_pattern_base.push_back( transformed_prefix_path );
                }

                // advance to the next path
                path_starting_fpnode = path_starting_fpnode->node_link;
            }

            // generate the transactions that represent the conditional pattern base
            std::vector<Transaction<Item>> conditional_fptree_transactions;
            for ( const TransformedPrefixPath<Item>& transformed_prefix_path : conditional_pattern_base ) {
                const std::vector<Item>& transformed_prefix_path_items = transformed_prefix_path.first;
                const uint64_t transformed_prefix_path_items_frequency = transformed_prefix_path.second;

                Transaction<Item> transaction = transformed_prefix_path_items;

                // add the same transaction transformed_prefix_path_items_frequency times
                for ( auto i = 0; i < transformed_prefix_path_items_frequency; ++i ) {
                    conditional_fptree_transactions.push_back( transaction );
                }
            }

            // build the conditional fptree relative to the current item with the transactions just generated
            const FPTree<Item> conditional_fptree( conditional_fptree_transactions, fptree.minimum_support_threshold );
            // call recursively fptree_growth on the conditional fptree (empty fptree: no patterns)
            std::set<Pattern<Item>> conditional_patterns = fptree_growth( conditional_fptree );

            // construct patterns relative to the current item using both the current item and the conditional patterns
            std::set<Pattern<Item>> curr_item_patterns;

            // the first pattern is made only by the current item
            // compute the frequency of this pattern by summing the frequency of the nodes which have the same item (follow the node links)
            uint64_t curr_item_frequency = 0;
            auto fpnode = pair.second;
            while ( fpnode ) {
                curr_item_frequency += fpnode->frequency;
                fpnode = fpnode->node_link;
            }
            // add the pattern as a result
            Pattern<Item> pattern{ { curr_item }, curr_item_frequency };
            curr_item_patterns.insert( pattern );

            // the next patterns are generated by adding the current item to each conditional pattern
            for ( const Pattern<Item>& pattern : conditional_patterns ) {
                Pattern<Item> new_pattern{ pattern };
                new_pattern.first.insert( curr_item );
                assert( curr_item_frequency >= pattern.second );
                new_pattern.second = pattern.second;

                curr_item_patterns.insert( { new_pattern } );
            }

            // join the patterns generated by the current item with all the other items of the fptree
            multi_path_patterns.insert( curr_item_patterns.cbegin(), curr_item_patterns.cend() );
        }

        return multi_path_patterns;
    }
}

#endif  // FPTREE_HPP
