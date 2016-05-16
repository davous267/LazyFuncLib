#include "catch.hpp"
#include "lazy.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <climits>

#define MAP_TST
//#undef MAP_TST

#define ZIP_TST
//#undef ZIP_TST

#define FILTER_TST
//#undef FILTER_TST

#define UNIQUE_TST
//#undef UNIQUE_TST

#define COMB_TST
//#undef COMB_TST

class IteratorTypeChecker
{
public:
    template<typename Iter>
    static void getIteratorType(Iter)
    {
        printType(typename std::iterator_traits<Iter>::iterator_category());
    }
private:
    static void printType(std::forward_iterator_tag)
    {
        std::cout << "Forward iterator" << std::endl;
    }

    static void printType(std::input_iterator_tag)
    {
        std::cout << "Input iterator" << std::endl;
    }

    template<typename T>
    static void printType(T)
    {
        std::cout << "OTHER TYPE!" << std::endl;
    }
};

/**
 * @brief The BadassIterator class does not own data, it just encapsulates pointer to somewhere else
 */
class BadassIterator : public std::iterator<std::input_iterator_tag, int>
{
private:
    int* mDataIter;
public:
    BadassIterator(int* dataIter) : mDataIter(dataIter) {}

    BadassIterator& operator++()
    {
        ++mDataIter;
        return *this;
    }

    BadassIterator operator++(int)
    {
        auto tmp = *this;
        ++mDataIter;
        return tmp;
    }

    const int& operator*()
    {
        return *mDataIter;
    }

    friend bool operator==(const BadassIterator&, const BadassIterator&);
    friend bool operator!=(const BadassIterator&, const BadassIterator&);
};

bool operator==(const BadassIterator& lhs, const BadassIterator& rhs)
{
    return lhs.mDataIter == rhs.mDataIter;
}

bool operator!=(const BadassIterator& lhs, const BadassIterator& rhs)
{
    return !(lhs==rhs);
}

template<typename Container>
std::string convertListToString(Container list)
{
    std::stringstream str;
    for(auto a : list)
        str << a << " | ";
    return str.str();
}

template<typename I, typename T>
void check(I first, I last, std::initializer_list<T> expected)
{
    //std::vector<typename std::iterator_traits<I>::value_type> resVct(first, last);
    bool res = std::equal(first, last, expected.begin(), expected.end());

    /*INFO("type    : " << typeid(typename std::iterator_traits<I>::value_type).name());
    INFO("given   : " << convertListToString(resVct));
    INFO("expected: " << convertListToString(expected));*/
    REQUIRE(res);
}

TEST_CASE("Input iterators passing", "[input_iter]")
{
std::istringstream str1("1 2 3 4");
std::istringstream str2("2 3 4 5");
std::istringstream str3("2 2 2 2");

#ifdef MAP_TST
    SECTION("Map")
    {
        auto m1 = lazy::map(std::istream_iterator<int>(str1), std::istream_iterator<int>(), [](int x){return x-1;});
        check(m1.begin(), m1.end(), {0,1,2,3});
        // NOTE Now str1 is empty - input iterator = no multipass - how to deal with that?
    }
#endif

#ifdef ZIP_TST
    SECTION("Zip")
    {
        auto z1 = lazy::zip(std::istream_iterator<int>(str1),std::istream_iterator<int>(),
                            std::istream_iterator<int>(str2), std::istream_iterator<int>(), [](int x, int y){return x+y;});
        check(z1.begin(), z1.end(), {3,5,7,9});
    }
#endif

#ifdef FILTER_TST
    SECTION("Filter")
    {
        auto f1 = lazy::filter(std::istream_iterator<int>(str1), std::istream_iterator<int>(), [](int x){return x%2==0;});
        check(f1.begin(), f1.end(), {2,4});

        auto f2 = lazy::filter(std::istream_iterator<int>(str2), std::istream_iterator<int>(), [](int){return false;});
        check(f2.begin(), f2.end(), std::initializer_list<int>());
    }
#endif

#ifdef UNIQUE_TST
#endif
    SECTION("Unique")
    {
        auto u1 = lazy::unique(std::istream_iterator<int>(str3), std::istream_iterator<int>());
        check(u1.begin(), u1.end(), {2});
    }

#ifdef COMB_TST
#endif
}

TEST_CASE("Baddass & pointer passing", "[badass]")
{
    int array1[4] = {1, 2, 3, 4};
    int* p_array1 = array1;

    int array2[8] = {1,1,2,2,3,3,4,4};
    int* p_array2 = array2;
#ifdef MAP_TST
    SECTION("Map")
    {
        auto m1 = lazy::map(BadassIterator(p_array1), BadassIterator(p_array1+4), [](int x){return x-1;});
        check(m1.begin(), m1.end(), {0,1,2,3});

        auto m2 = lazy::map(p_array1, p_array1+4, [](int x){return x-1;});
        check(m2.begin(), m2.end(), {0,1,2,3});

        auto m3 = lazy::map(m1.begin(), m1.end(), [](int x){return x%2;});
        check(m3.begin(), m3.end(), {0,1,0,1});

        auto m4 = lazy::map(m2.begin(), m2.end(), [](int x){return x%2;});
        check(m4.begin(), m4.end(), {0,1,0,1});
    }

#endif

#ifdef ZIP_TST
    SECTION("Zip")
    {
        auto z1 = lazy::zip(BadassIterator(p_array1), BadassIterator(p_array1+4),
                            BadassIterator(p_array1), BadassIterator(p_array1+4), [](int x, int y){return x+y;});
        check(z1.begin(), z1.end(), {2,4,6,8});

        auto z2 = lazy::zip(p_array1, p_array1+4,
                            p_array2, p_array2+8, [](int x, int y){return x+y;});
        check(z2.begin(), z2.end(), {2,3,5,6});

        auto z3 = lazy::zip(z1.begin(), z1.end(),
                            z1.begin(), z1.end(), [](int x, int y){return x-y;});
        check(z3.begin(), z3.end(), {0,0,0,0});

        auto z4 = lazy::zip(z2.begin(), z2.end(),
                            z2.begin(), z2.end(), [](int x, int y){return x-y;});
        check(z4.begin(), z4.end(), {0,0,0,0});

        auto z5 = lazy::zip(z1.begin(), z1.end(),
                            z2.begin(), z2.end(), [](int x, int y){return x+y;});
        check(z5.begin(), z5.end(), {4,7,11,14});

        auto z6 = lazy::zip(z2.begin(), z2.end(),
                            z1.begin(), z1.end(), [](int x, int y){return x+y;});
        check(z6.begin(), z6.end(), {4,7,11,14});
    }

#endif

#ifdef FILTER_TST
    SECTION("Filter")
    {
        auto f1 = lazy::filter(BadassIterator(p_array1), BadassIterator(p_array1+4), [](int x){return x>2;});
        check(f1.begin(), f1.end(), {3, 4});

        auto f2 = lazy::filter(p_array1, p_array1+4, [](int x){return x>2;});
        check(f2.begin(), f2.end(), {3, 4});

        auto f3 = lazy::filter(f1.begin(), f1.end(), [](int x){return x==4;});
        check(f3.begin(), f3.end(), {4});

        auto f4 = lazy::filter(f2.begin(), f2.end(), [](int x){return x==3;});
        check(f4.begin(), f4.end(), {3});
    }
#endif

#ifdef UNIQUE_TST
    SECTION("Unique")
    {
        auto u1 = lazy::unique(BadassIterator(p_array2), BadassIterator(p_array2+8));
        check(u1.begin(), u1.end(), {1, 2, 3, 4});

        auto u2 = lazy::unique(p_array2, p_array2+8);
        check(u2.begin(), u2.end(), {1, 2, 3, 4});

        auto u3 = lazy::unique(u1.begin(), u1.end());
        check(u3.begin(), u3.end(), {1, 2, 3, 4});

        auto u4 = lazy::unique(u2.begin(), u2.end());
        check(u4.begin(), u4.end(), {1, 2, 3, 4});

        // NOTE - what about static variable in template to "disable" iterator if it's input iter. and increased
    }
#endif

#ifdef COMB_TST
#endif
}
