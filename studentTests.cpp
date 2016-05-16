#include <vector>
#include <string>
#include <iostream>
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

template< typename I, typename T >
void check( I first, I last, std::initializer_list< T > expected ) {
    bool result = std::equal( first, last, expected.begin(), expected.end() );
    REQUIRE( result );
}

TEST_CASE( "students tests of iterators", "[student]" ) {

    std::vector< int > data{ 1,2,3,4,5,6,7,8,9,1 };

#ifdef MAP_TST
    SECTION( "map" ) {
        auto m = lazy::map( data.begin(), data.end(), []( int i ) {
            return i + 1;
        } );
        check( m.begin(), m.end(), { 2,3,4,5,6,7,8,9,10, 2 } );

        auto m1 = lazy::map(data.begin(), data.end(), [&]( int i ) -> std::string {
            return "cislo" + std::to_string(i);
        });
        check(m1.begin(), m1.end(), {"cislo1","cislo2","cislo3","cislo4","cislo5","cislo6","cislo7","cislo8","cislo9","cislo1"});

        auto m1bg = ++m1.begin();
        std::vector<std::string> m2v = {m1.begin(), m1bg};
        std::vector<std::string> m2va = {"cislo1"};
        REQUIRE(m2v == m2va);

        REQUIRE(*m1.begin() == "cislo1");
        REQUIRE(*++m1.begin() == "cislo2");
        REQUIRE(++m1.begin() != m1.begin());
        REQUIRE(++m1.begin() == ++m1.begin());

        int i=0;
        for(auto it = m1.begin();it!=m1.end();++it)
            ++i;
        REQUIRE(i==10);

        lazy::MapIt<std::vector<int>::iterator, int> testMapIter1;
        lazy::MapIt<std::vector<int>::iterator, int> testMapIter2;
        REQUIRE(testMapIter1 == testMapIter2);
    }
#endif
#ifdef FILTER_TST
    SECTION( "filter" ) {
        auto f = lazy::filter( data.begin(), data.end(), []( int i ) {
            return i % 2 == 0;
        } );
        check( f.begin(), f.end(), { 2,4,6,8 } );

        std::vector<int> f1 = {f.begin(), f.end()};
        REQUIRE(f1.size() == 4);

        auto g = lazy::filter( data.begin(), data.end(), [&]( int i ) {
            return i % 2 == 1;
        } );

        std::vector<int> g1 = {g.begin(), g.end()};
        std::vector<int> g2 = {1,3,5,7,9,1};
        REQUIRE(g1 == g2);

        REQUIRE(g.begin() == g.begin());
        REQUIRE(++g.begin() == ++g.begin());

        lazy::FilterIt<std::vector<int>::iterator> testFilterIter1;
        lazy::FilterIt<std::vector<int>::iterator> testFilterIter2;
        REQUIRE(testFilterIter1 == testFilterIter2);
    }
#endif
#ifdef ZIP_TST
    SECTION( "zip" ) {
        auto z = lazy::zip( data.begin(), data.end(), data.begin(), data.end(), []( int a, int b ) {
            return a + b;
        } );
        check( z.begin(), z.end(), { 2,4,6,8,10,12,14,16,18,2 } );

        std::vector<std::string> strVct = {"someNum", "anotherNum"};
        auto z1 = lazy::zip( data.begin(), data.end(), strVct.begin(), strVct.end(), [&]( int a, std::string b ) {
            return std::to_string(a)+b;
        } );
        check( z1.begin(), z1.end(), { "1someNum", "2anotherNum" } );

        auto z2 = lazy::zip( strVct.begin(), strVct.end(), data.begin(), data.end(), [&]( std::string a, int b ) {
            return a+std::to_string(b);
        } );
        check( z2.begin(), z2.end(), { "someNum1", "anotherNum2" } );

        REQUIRE(z.begin() == z.begin());
        REQUIRE(++z.begin() == ++z.begin());

        lazy::ZipIt<std::vector<int>::iterator, std::vector<bool>::iterator, int> testZipIter1;
        lazy::ZipIt<std::vector<int>::iterator, std::vector<bool>::iterator, int> testZipIter2;
        REQUIRE(testZipIter1 == testZipIter2);
    }
#endif
#ifdef UNIQUE_TST
    SECTION( "unique" ) {
        auto u = lazy::unique( data.begin(), data.end() );
        check( u.begin(), u.end(), { 1,2,3,4,5,6,7,8,9 } );
    }
#endif
}


