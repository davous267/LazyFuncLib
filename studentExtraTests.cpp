#include <vector>
#include <initializer_list>
#include <algorithm>
#include <iostream>

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

template< typename T, typename I >
void check( I first, I last, std::initializer_list< T > expected ) {
    bool result = std::equal( first, last, expected.begin(), expected.end() );
    REQUIRE( result );
}

struct Item {
    Item():_value(-1) {
        FAIL( "nonparametric constructor must not be called" );
    }
    Item( int value ) :
        _value( value )
    {}

    int get() const {
        return _value;
    }

private:
    int _value;
};

#if defined(MAP_TST) || defined(ZIP_TST)
bool operator==( const Item &lhs, const Item &rhs ) {
    return lhs.get() == rhs.get();
}
#endif
/*bool operator!=( const Item &lhs, const Item &rhs ) {
    return !( lhs == rhs );
}
*/

struct SequenceGenerator {

    struct iterator : std::iterator< std::forward_iterator_tag, int, std::ptrdiff_t, const int *, const int & > {
        iterator() :
            _value( 1 ),
            _sequence( 1 ),
            _limit( 0 )
        {}
        iterator( int limit ) :
            _value( 0 ),
            _sequence( 0 ),
            _limit( limit )
        {}

        const int &operator*() const {
            return _value;
        }
        iterator &operator++() {
            ++_limit;
            ++_value;
            if ( _value > _sequence ) {
                ++_sequence;
                _value = 1;
            }
            return *this;
        }
        iterator operator++( int ) {
            iterator self( *this );
            ++( *this );
            return self;
        }
        bool operator==( const iterator &other ) const {
            return _limit == other._limit;
        }
        bool operator!=( const iterator &other ) const {
            return _limit != other._limit;
        }

    private:
        int _value;
        int _sequence;
        int _limit;
    };

    SequenceGenerator( int stop ) :
        _stop( stop )
    {}

    iterator begin() const {
        return iterator();
    }
    iterator end() const {
        return iterator( _stop );
    }

private:
    int _stop;
};

}

TEST_CASE( "students tests of extra required behaviour", "[student extra]" ) {

#ifdef MAP_TST
    SECTION( "nonparametric map" ) {
        std::vector< Item > data{ 1,2,3,4 };
        auto m = lazy::map( data.begin(), data.end(), []( const Item &i ) {
            return Item( i.get() * 2 );
        } );
        check< Item >( m.begin(), m.end(), { 2,4,6,8 } );

        auto m_tmp = m.begin();
        for(int i=2; i<=8; i+=2)
            REQUIRE(i == (m_tmp++)->get());
    }
#endif
#ifdef ZIP_TST
    SECTION( "nonparametric zip" ) {
        std::vector< Item > data{ 1,2,3,4 };
        auto z = lazy::zip( data.begin(), data.end(),
                            data.begin(), data.end(),
                            []( const Item &a, const Item &b ) {
            return Item( a.get() * b.get() );
        } );
        check< Item >( z.begin(), z.end(), { 1,4,9,16 } );

        auto z_tmp = z.begin();
        REQUIRE(1 == (z_tmp++)->get());
        REQUIRE(4 == (z_tmp++)->get());
        REQUIRE(9 == (z_tmp++)->get());
        REQUIRE(16 == (z_tmp++)->get());
    }
#endif
#ifdef FILTER_TST
    SECTION("nonparametric filter")
    {
        std::vector< Item > data{ 1,2,3,4 };
        auto f = lazy::filter( data.begin(), data.end(), []( const Item &i ) {
            return i.get() % 2 == 0;
        } );
        check< Item >( f.begin(), f.end(), { 2,4 } );

        auto f_tmp = f.begin();
        REQUIRE(2 == f_tmp->get());
        REQUIRE(2 == (*f_tmp).get());
        ++f_tmp;
        REQUIRE(4 == f_tmp->get());
    }
#endif
#ifdef UNIQUE_TST
    SECTION("nonparametric unique")
    {
        // NOTE - not working, hash template spec. needed for std::unordered_set
        /*std::vector< Item > data{ 1,2,3,4,4,2,3,1,5 };
        auto u = lazy::unique( data.begin(), data.end() );
        check< Item >( u.begin(), u.end(), { 1,2,3,4,5 } );*/

        std::vector< int > data{ 1,2,3,4,4,2,3,1,5 };
        auto u = lazy::unique( data.begin(), data.end() );
        check< int >( u.begin(), u.end(), { 1,2,3,4,5 } );

        auto u_tmp = u.begin();
        REQUIRE(*u_tmp == 1);
        REQUIRE(u_tmp == u.begin());
        REQUIRE(++u_tmp == ++u.begin());
        REQUIRE_FALSE(u_tmp == u.begin());
        REQUIRE(*u_tmp == 2);
    }

    SECTION( "this test has to pass in reasonable amout of time (15 seconds at maximum)" ) {
        SequenceGenerator sg( 1000*1000 ); // default for test is 1000*1000
        auto u = lazy::unique( sg.begin(), sg.end() );
        REQUIRE( 1413 == *std::max_element( u.begin(), u.end() ) );
    }
#endif
}
