#include "catch.hpp"
#include "lazy.h"

#include <chrono>
#include <string>
#include <vector>

namespace tests
{
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
    std::vector<typename std::iterator_traits<I>::value_type> resVct(first, last);
    bool res = std::equal(first, last, expected.begin(), expected.end());

    INFO("type    : " << typeid(typename std::iterator_traits<I>::value_type).name());
    INFO("given   : " << convertListToString(resVct));
    INFO("expected: " << convertListToString(expected));
    REQUIRE(res);
}
}
namespace
{
TEST_CASE( "complex" ) {
    std::vector< std::string > words{
        "hi",
        "hello",
        "world",
        "terrible",
        "a",
        "I",
        "piggy",
        "car",
        "left",
        "a"
    };

    SECTION( "chain1" ) {
        auto f = lazy::filter( words.begin(), words.end(), []( const std::string &w ) {
            return w.size() == 5;
        } );
        auto m = lazy::map( f.begin(), f.end(), []( const std::string &w ) {
            return w.size();
        } );
        auto u = lazy::unique( m.begin(), m.end() );

        ::tests::check( u.begin(), u.end(), { 5 } );
    }


    SECTION( "chain2" ) {
        using namespace std::literals;
        auto m = lazy::map( words.begin(), words.end(), []( const std::string &w ) {
            return w.size();
        } );
        auto u = lazy::unique( m.begin(), m.end() );

        auto z = lazy::zip( u.begin(), u.end(),
                            words.begin(), words.end(),
                            []( size_t s, const std::string &w ) {
                if ( s != w.size() )
                return "--"s;
                return w;
    } );
        ::tests::check( z.begin(), z.end(), { "hi", "hello", "--", "--", "--", "--" } );
    }
    SECTION( "chain3" ) {
        auto m = lazy::map( words.begin(), words.end(), []( const std::string &w ) {
            return w.size();
        } );
        auto z = lazy::zip( m.begin(), m.end(),
                            words.begin(), words.end(),
                            []( size_t s, const std::string &w ) {
                return w + " " + std::to_string( s );
    } );
        SECTION( "3" ) {
            auto f = lazy::filter( z.begin(), z.end(), []( const std::string &s ) {
                return s.size() == 3;
            } );
            ::tests::check( f.begin(), f.end(), { "a 1", "I 1", "a 1" } );
        }
        SECTION( "5" ) {
            auto f = lazy::filter( z.begin(), z.end(), []( const std::string &s ) {
                return s.size() == 5;
            } );
            ::tests::check( f.begin(), f.end(), { "car 3" } );
        }
        SECTION( "10" ) {
            auto f = lazy::filter( z.begin(), z.end(), []( const std::string &s ) {
                return s.size() == 10;
            } );
            ::tests::check( f.begin(), f.end(), { "terrible 8" } );
        }
    }
}
}

