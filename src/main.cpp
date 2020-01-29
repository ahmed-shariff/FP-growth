#include <cassert>
#include <cstdlib>
#include <iostream>
#include <set>
#include <vector>

#include "fptree.hpp"


void test_1()
{
    const std::string a{ "A" };
    const std::string b{ "B" };
    const std::string c{ "C" };
    const std::string d{ "D" };
    const std::string e{ "E" };

    const std::vector<Transaction<std::string>> transactions{
        { a, b },
        { b, c, d },
        { a, c, d, e },
        { a, d, e },
        { a, b, c },
        { a, b, c, d },
        { a },
        { a, b, c },
        { a, b, d },
        { b, c, e }
    };

    const uint64_t minimum_support_threshold = 2;

    const FPTree<std::string> fptree{ transactions, minimum_support_threshold };

    const std::set<Pattern<std::string>> patterns = fptree_growth( fptree );

    assert( patterns.size() == 19 );
    assert( patterns.count( { { a }, 8 } ) );
    assert( patterns.count( { { b, a }, 5 } ) );
    assert( patterns.count( { { b }, 7 } ) );
    assert( patterns.count( { { c, b }, 5 } ) );
    assert( patterns.count( { { c, a, b }, 3 } ) );
    assert( patterns.count( { { c, a }, 4 } ) );
    assert( patterns.count( { { c }, 6 } ) );
    assert( patterns.count( { { d, a }, 4 } ) );
    assert( patterns.count( { { d, c, a }, 2 } ) );
    assert( patterns.count( { { d, c }, 3 } ) );
    assert( patterns.count( { { d, b, a }, 2 } ) );
    assert( patterns.count( { { d, b, c }, 2 } ) );
    assert( patterns.count( { { d, b }, 3 } ) );
    assert( patterns.count( { { d }, 5 } ) );
    assert( patterns.count( { { e, d }, 2 } ) );
    assert( patterns.count( { { e, c }, 2 } ) );
    assert( patterns.count( { { e, a, d }, 2 } ) );
    assert( patterns.count( { { e, a }, 2 } ) );
    assert( patterns.count( { { e }, 3 } ) );
}

void test_2()
{
    const std::string a{ "A" };
    const std::string b{ "B" };
    const std::string c{ "C" };
    const std::string d{ "D" };
    const std::string e{ "E" };

    const std::vector<Transaction<std::string>> transactions{
        { a, b, d, e },
        { b, c, e },
        { a, b, d, e },
        { a, b, c, e },
        { a, b, c, d, e },
        { b, c, d },
    };

    const uint64_t minimum_support_threshold = 3;

    const FPTree<std::string> fptree{ transactions, minimum_support_threshold };

    const std::set<Pattern<std::string>> patterns = fptree_growth( fptree );

    assert( patterns.size() == 19 );
    assert( patterns.count( { { e, b }, 5 } ) );
    assert( patterns.count( { { e }, 5 } ) );
    assert( patterns.count( { { a, b, e }, 4 } ) );
    assert( patterns.count( { { a, b }, 4 } ) );
    assert( patterns.count( { { a, e }, 4 } ) );
    assert( patterns.count( { { a }, 4 } ) );
    assert( patterns.count( { { d, a, b }, 3 } ) );
    assert( patterns.count( { { d, a }, 3 } ) );
    assert( patterns.count( { { d, e, b, a }, 3 } ) );
    assert( patterns.count( { { d, e, b }, 3 } ) );
    assert( patterns.count( { { d, e, a }, 3 } ) );
    assert( patterns.count( { { d, e }, 3 } ) );
    assert( patterns.count( { { d, b }, 4 } ) );
    assert( patterns.count( { { d }, 4 } ) );
    assert( patterns.count( { { c, e, b }, 3 } ) );
    assert( patterns.count( { { c, e }, 3 } ) );
    assert( patterns.count( { { c, b }, 4 } ) );
    assert( patterns.count( { { c }, 4 } ) );
    assert( patterns.count( { { b }, 6 } ) );
}

void test_3()
{
    const std::string a{ "A" };
    const std::string b{ "B" };
    const std::string c{ "C" };
    const std::string d{ "D" };
    const std::string e{ "E" };
    const std::string f{ "F" };
    const std::string g{ "G" };
    const std::string h{ "H" };
    const std::string i{ "I" };
    const std::string j{ "J" };
    const std::string k{ "K" };
    const std::string l{ "L" };
    const std::string m{ "M" };
    const std::string n{ "N" };
    const std::string o{ "O" };
    const std::string p{ "P" };
    const std::string s{ "S" };

    const std::vector<Transaction<std::string>> transactions{
        { f, a, c, d, g, i, m, p },
        { a, b, c, f, l, m, o },
        { b, f, h, j, o },
        { b, c, k, s, p },
        { a, f, c, e, l, p, m, n }
    };

    const uint64_t minimum_support_threshold = 3;

    const FPTree<std::string> fptree{ transactions, minimum_support_threshold };

    const std::set<Pattern<std::string>> patterns = fptree_growth( fptree );

    assert( patterns.size() == 18 );
    assert( patterns.count( { { f }, 4 } ) );
    assert( patterns.count( { { c, f }, 3 } ) );
    assert( patterns.count( { { c }, 4 } ) );
    assert( patterns.count( { { b }, 3 } ) );
    assert( patterns.count( { { p, c }, 3 } ) );
    assert( patterns.count( { { p }, 3 } ) );
    assert( patterns.count( { { m, f, c }, 3 } ) );
    assert( patterns.count( { { m, f }, 3 } ) );
    assert( patterns.count( { { m, c }, 3 } ) );
    assert( patterns.count( { { m }, 3 } ) );
    assert( patterns.count( { { a, f, c, m }, 3 } ) );
    assert( patterns.count( { { a, f, c }, 3 } ) );
    assert( patterns.count( { { a, f, m }, 3 } ) );
    assert( patterns.count( { { a, f }, 3 } ) );
    assert( patterns.count( { { a, c, m }, 3 } ) );
    assert( patterns.count( { { a, c }, 3 } ) );
    assert( patterns.count( { { a, m }, 3 } ) );
    assert( patterns.count( { { a }, 3 } ) );
}

int main(int argc, const char *argv[])
{
    test_1();
    test_2();
    test_3();
    std::cout << "All tests passed!" << std::endl;

    return EXIT_SUCCESS;
}
