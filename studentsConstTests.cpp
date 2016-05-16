#include <vector>
#include <initializer_list>

#include "catch.hpp"
#include "lazy.h"

#define MAP_TST
//#undef MAP_TST

#define ZIP_TST
//#undef ZIP_TST

#define FILTER_TST
//#undef FILTER_TST

#define UNIQUE_TST
//#undef UNIQUE_TST

namespace {

template< typename I, typename T >
void check( I first, I last, std::initializer_list< T > expected ) {
    bool result = std::equal( first, last, expected.begin(), expected.end() );
    REQUIRE( result );
}

}

TEST_CASE( "students tests of const iterators", "[student const iterator]" ) {

    const std::vector< int > data{ 1,2,3,4,5,6,7,8,9,1 };

#ifdef MAP_TST
    SECTION( "map" ) {
        auto m = lazy::map( data.begin(), data.end(), []( int i ) {
            return i + 1;
        } );
        check( m.begin(), m.end(), { 2,3,4,5,6,7,8,9,10,2 } );
    }
#endif
#ifdef FILTER_TST
    SECTION( "filter" ) {
        auto f = lazy::filter( data.begin(), data.end(), []( int i ) {
            return i % 2 == 0;
        } );
        check( f.begin(), f.end(), { 2,4,6,8 } );
    }
#endif
#ifdef ZIP_TST
    SECTION( "zip" ) {
        auto z = lazy::zip( data.begin(), data.end(), data.begin(), data.end(), []( int a, int b ) {
            return a + b;
        } );
        check( z.begin(), z.end(), { 2,4,6,8,10,12,14,16,18,2 } );
    }
#endif
#ifdef UNIQUE_TST
    SECTION( "unique" ) {
        auto u = lazy::unique( data.begin(), data.end() );
        check( u.begin(), u.end(), { 1,2,3,4,5,6,7,8,9 } );
    }
#endif
}

