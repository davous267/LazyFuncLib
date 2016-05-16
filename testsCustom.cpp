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

#define COMB_TEST
//#undef COMB_TEST

template<typename Container>
std::string convertListToString(Container list)
{
    std::stringstream str;
    for(auto a : list)
        str << a << " | ";
    return str.str();
}

template<typename I, typename T>
void c_check(I first, I last, std::initializer_list<T> expected)
{
    std::vector<typename std::iterator_traits<I>::value_type> resVct(first, last);
    bool res = std::equal(first, last, expected.begin(), expected.end());

    INFO("type    : " << typeid(typename std::iterator_traits<I>::value_type).name());
    INFO("given   : " << convertListToString(resVct));
    INFO("expected: " << convertListToString(expected));
    REQUIRE(res);
}

TEST_CASE("custom tests", "[custom]")
{
    std::vector<int> dataInt {6,4,1,2,3,145,-535,4};
    std::vector<float> dataFloat {2.5,3.0,3.2,6.3,-5};
    std::vector<double> dataDouble {10.5,10.5,10.5,10.5};
    std::vector<char> dataEmpty {};
    std::vector<std::string> dataString {"prvni", "druhe", "treti", "ctvrte"};
    std::vector<int> dataIntOne {1};
    std::vector<int> dataSame(200,2);

    const std::vector<char> dataChar {'a', 'b', 'c', 'd', 'e'};

    std::array<const int, 3> arrayInt {{4,5,6}}; // Double braces because of clang complaining
    std::array<const char*, 2> arrayChar {{"prvniConst", "druhyConst"}};

    std::set<int> setInt {7,5,6,1};

#ifdef MAP_TST
    SECTION("map")
    {
        // Test function
        auto c_m1 = lazy::map(dataInt.begin(), dataInt.end(), [](int x) {return x-1;});
        c_check(c_m1.begin(), c_m1.end(), {5,3,0,1,2,144,-536,3});

        auto c_m2 = lazy::map(dataFloat.begin(), dataFloat.end(), [&](auto x) {return 0*x;});
        c_check(c_m2.begin(), c_m2.end(), {0,0,0,0,0});

        auto c_m3 = lazy::map(dataDouble.begin(), dataDouble.end(), [&](auto x) {return x-0.5;});
        c_check(c_m3.begin(), c_m3.end(), {10,10,10,10});
        c_check(c_m3.begin(), c_m3.end(), {10,10,10,10});

        auto c_m4 = lazy::map(dataEmpty.begin(), dataEmpty.end(), [&](auto x) {return 2*x;});
        c_check(c_m4.begin(), c_m4.end(), std::initializer_list<char>());

        auto c_m5 = lazy::map(dataString.begin(), dataString.end(), [](auto x) {return x+x;});
        c_check(c_m5.begin(), c_m5.end(), {"prvniprvni","druhedruhe","tretitreti","ctvrtectvrte"});

        auto c_m6 = lazy::map(dataChar.begin(), dataChar.end(), [](auto x) {return std::string(2,x);});
        c_check(c_m6.begin(), c_m6.end(), {"aa","bb","cc","dd","ee"});

        // NOTE const problem - should be solved? - probably not, storing consts seems a bit weird
        /*auto c_m7 = lazy::map(arrayInt.begin(), arrayInt.end(), [](auto x) -> int {return x-1;});
        c_check(c_m7.begin(), c_m7.end(), {3,4,5});

        auto c_m8 = lazy::map(arrayChar.begin(), arrayChar.end(), [](auto x) {return x[0];});
        c_check(c_m8.begin(),c_m8.end(),{'p','d'});*/

        auto c_m9 = lazy::map(setInt.begin(), setInt.end(), [](auto x){return x-1;});
        c_check(c_m9.begin(),c_m9.end(),{0,4,5,6});

        auto c_m10 = lazy::map(setInt.cbegin(), setInt.cend(), [](auto x){return x-1;});
        c_check(c_m10.begin(),c_m10.end(),{0,4,5,6});

        // Test MapIt
        lazy::MapIt<std::vector<int>::iterator, int> c_mit1;
        lazy::MapIt<std::vector<int>::iterator, int> c_mit2(c_mit1);
        lazy::MapIt<std::vector<int>::iterator, int> c_mit1s;

        REQUIRE(c_mit1 == c_mit2);
        REQUIRE(c_mit1 == c_mit1s);

        auto c_mit3 = c_m1.begin();
        c_mit2 = c_mit3;

        c_check(c_mit2, c_m1.end(), {5,3,0,1,2,144,-536,3});
        REQUIRE(c_mit2 == c_m1.begin());
        REQUIRE_FALSE(c_mit2 == c_m1.end());

        REQUIRE(*c_mit2 == 5);

        c_mit1 = ++c_mit2;
        REQUIRE(c_mit1 == c_mit2);
        c_mit3 = c_mit1++;
        REQUIRE_FALSE(c_mit3 == c_mit1);
        REQUIRE(*c_mit1 == 0);
        REQUIRE(++c_mit3 == c_mit1);
    }

#endif
#ifdef ZIP_TST
    SECTION("zip")
    {
        // Test function
        auto c_z1 = lazy::zip(dataInt.begin(), dataInt.end(), dataFloat.begin(), dataFloat.end(),
                              [](int x, float y) -> int {return x+y;});
        c_check(c_z1.begin(),c_z1.end(),{8,7,4,8,-2});

        auto c_z2 = lazy::zip(dataFloat.begin(), dataFloat.end(), dataInt.begin(), dataInt.end(),
                              [](float x, int y) -> int {return x+y;});
        c_check(c_z2.begin(),c_z2.end(),{8,7,4,8,-2});
        c_check(c_z2.begin(),c_z2.end(),{8,7,4,8,-2});

        auto c_z3 = lazy::zip(dataDouble.begin(), dataDouble.end(), dataEmpty.begin(), dataEmpty.end(),
                              [](auto x, auto y) {return x+y;});
        c_check(c_z3.begin(),c_z3.end(),std::initializer_list<char>());

        auto c_z4 = lazy::zip(dataString.begin(), dataString.end(), dataString.begin(), dataString.end(),
                              [](auto x, auto y) {return x+y;});
        c_check(c_z4.begin(), c_z4.end(), {"prvniprvni","druhedruhe","tretitreti","ctvrtectvrte"});

        auto c_z5 = lazy::zip(setInt.begin(), setInt.end(), setInt.begin(), setInt.end(),
                              [](int x, int y) {return x==y;});
        c_check(c_z5.begin(),c_z5.end(),{true,true,true,true});


        auto c_z6 = lazy::zip(dataEmpty.begin(), dataEmpty.end(), dataFloat.begin(), dataFloat.end(),
                              [](auto x, auto y) {return x>y;});
        c_check(c_z6.begin(),c_z6.end(),std::initializer_list<bool>());

        auto c_z7 = lazy::zip(dataInt.cbegin(), dataInt.cend(), dataFloat.cbegin(), dataFloat.cend(),
                              [](int x, float y) -> int {return x+y;});
        c_check(c_z7.begin(),c_z7.end(),{8,7,4,8,-2});

        // Test ZipIt
        lazy::ZipIt<std::vector<int>::iterator, std::vector<float>::iterator, int> c_zit1;
        lazy::ZipIt<std::vector<int>::iterator, std::vector<float>::iterator, int> c_zit2(c_zit1);
         lazy::ZipIt<std::vector<int>::iterator, std::vector<float>::iterator, int> c_zit1s;

        REQUIRE(c_zit1 == c_zit2);
        REQUIRE(c_zit1 == c_zit1s);

        auto c_zit3 = c_z1.begin();
        c_zit2 = c_zit3;

        c_check(c_zit2, c_z1.end(), {8,7,4,8,-2});
        REQUIRE(c_zit2 == c_z1.begin());
        REQUIRE_FALSE(c_zit2 == c_z1.end());

        REQUIRE(*c_zit2 == 8);

        c_zit1 = ++c_zit2;
        REQUIRE(c_zit1 == c_zit2);
        c_zit3 = c_zit1++;
        REQUIRE_FALSE(c_zit3 == c_zit1);
        REQUIRE(*c_zit1 == 4);
        REQUIRE(++c_zit3 == c_zit1);
    }

#endif
#ifdef FILTER_TST
    SECTION("filter")
    {
        // Test function
        auto c_f1 = lazy::filter(dataInt.begin(), dataInt.end(), [](int x) {return x%2 == 0;});
        c_check(c_f1.begin(),c_f1.end(), {6,4,2,4});

        auto c_f2 = lazy::filter(dataEmpty.begin(), dataEmpty.end(), [](char x) {return x=='a';});
        c_check(c_f2.begin(),c_f2.end(), std::initializer_list<char>());

        auto c_f3 = lazy::filter(dataString.begin(), dataString.end(), [](std::string x) {return x=="prvni";});
        c_check(c_f3.begin(),c_f3.end(),{"prvni"});
        c_check(c_f3.begin(),c_f3.end(),{"prvni"});

        auto c_f5 = lazy::filter(dataString.begin(), dataString.end(), [](std::string x) {return x=="ctvrte";});
        c_check(c_f5.begin(),c_f5.end(),{"ctvrte"});

        auto c_f6 = lazy::filter(dataIntOne.begin(), dataIntOne.end(), [](int x){return x==1;});
        c_check(c_f6.begin(),c_f6.end(),{1});

        auto c_f7 = lazy::filter(dataIntOne.begin(), dataIntOne.end(), [](int x){return x!=1;});
        c_check(c_f7.begin(),c_f7.end(),std::initializer_list<int>());

        auto c_f4 = lazy::filter(dataChar.begin(), dataChar.end(), [](char x) {return x=='x';});
        c_check(c_f4.begin(),c_f4.end(),std::initializer_list<char>());

        auto c_f8 = lazy::filter(setInt.begin(),setInt.end(),[](int){return true;});
        c_check(c_f8.begin(),c_f8.end(),{1,5,6,7});

        auto c_f9 = lazy::filter(setInt.cbegin(),setInt.cend(),[](int){return true;});
        c_check(c_f9.begin(),c_f9.end(),{1,5,6,7});

        // Test FilterIt
        lazy::FilterIt<std::vector<int>::iterator> c_fit1;
        lazy::FilterIt<std::vector<int>::iterator> c_fit2(c_fit1);
        lazy::FilterIt<std::vector<int>::iterator> c_fit1s;

        REQUIRE(c_fit1 == c_fit2);
        REQUIRE(c_fit1 == c_fit1s);

        auto c_fit3 = c_f1.begin();
        c_fit2 = c_fit3;

        c_check(c_fit2, c_f1.end(), {6,4,2,4});
        REQUIRE(c_fit2 == c_f1.begin());
        REQUIRE_FALSE(c_fit2 == c_f1.end());

        REQUIRE(*c_fit2 == 6);

        c_fit1 = ++c_fit2;
        REQUIRE(c_fit1 == c_fit2);
        c_fit3 = c_fit1++;
        REQUIRE_FALSE(c_fit3 == c_fit1);
        REQUIRE(*c_fit1 == 2);
        REQUIRE(++c_fit3 == c_fit1);
    }

#endif
#ifdef UNIQUE_TST
    SECTION("unique")
    {
        // Test function
        auto c_u1 = lazy::unique(dataInt.begin(), dataInt.end());
        c_check(c_u1.begin(), c_u1.end(), {6,4,1,2,3,145,-535});

        auto c_u2 = lazy::unique(dataEmpty.begin(), dataEmpty.end());
        c_check(c_u2.begin(), c_u2.end(), std::initializer_list<char>());

        auto c_u3 = lazy::unique(dataString.begin(), dataString.end());
        c_check(c_u3.begin(), c_u3.end(), {"prvni","druhe","treti","ctvrte"});

        auto c_u4 = lazy::unique(dataIntOne.begin(), dataIntOne.end());
        c_check(c_u4.begin(), c_u4.end(), {1});
        c_check(c_u4.begin(), c_u4.end(), {1});

        auto c_u5 = lazy::unique(dataChar.begin(), dataChar.end());
        c_check(c_u5.begin(), c_u5.end(), {'a','b','c','d','e'});

        auto c_u6 = lazy::unique(setInt.begin(), setInt.end());
        c_check(c_u6.begin(), c_u6.end(), {1,5,6,7});

        auto c_u7 = lazy::unique(dataSame.begin(), dataSame.end());
        c_check(c_u7.begin(), c_u7.end(), {2});

        auto c_u8 = lazy::unique(dataSame.cbegin(), dataSame.cend());
        c_check(c_u8.begin(), c_u8.end(), {2});

        // Test UniqueIt - iterator Removed
        /*lazy::UniqueIt<std::vector<int>::iterator> c_uit1;
        lazy::UniqueIt<std::vector<int>::iterator> c_uit2(c_uit1);

        REQUIRE(c_uit1 == c_uit2);

        auto c_uit3 = c_u1.begin();
        c_uit2 = c_uit3;

        c_check(c_uit2, c_u1.end(), {6,4,1,2,3,145,-535});
        REQUIRE(c_uit2 == c_u1.begin());
        REQUIRE_FALSE(c_uit2 == c_u1.end());

        REQUIRE(*c_uit2 == 6);

        c_uit1 = ++c_uit2;
        REQUIRE(c_uit1 == c_uit2);
        c_uit3 = c_uit1++;
        REQUIRE_FALSE(c_uit3 == c_uit1);
        REQUIRE(*c_uit1 == 1);
        REQUIRE(++c_uit3 == c_uit1);*/
    }
#endif
#ifdef COMB_TEST
    SECTION("combined tests")
    {
        // map . zip
        auto c_c1 = lazy::zip(dataInt.begin(), dataInt.end(), dataFloat.begin(), dataFloat.end(),
                              [](int x, float y) -> int {return x+y;});
        auto c_c2 = lazy::map(c_c1.begin(),c_c1.end(), [](auto x) {return x+2;});
        c_check(c_c2.begin(), c_c2.end(),{10,9,6,10,0});

        // filter . unique
        auto c_c3 = lazy::unique(setInt.begin(), setInt.end());
        auto c_c4 = lazy::filter(c_c3.begin(), c_c3.end(), [](auto x){return x%2 == 0;});
        c_check(c_c4.begin(), c_c4.end(), {6});

        // filter . map
        auto c_c5 = lazy::filter(c_c2.begin(), c_c2.end(), [](auto x){return x%2 == 0;});
        c_check(c_c5.begin(), c_c5.end(), {10,6,10,0});

        // unique . filter
        auto c_c6 = lazy::unique(c_c5.begin(), c_c5.end());
        c_check(c_c6.begin(), c_c6.end(), {10,6,0});

        // unique . map
        auto c_c7 = lazy::unique(c_c2.begin(), c_c2.end());
        c_check(c_c7.begin(), c_c7.end(), {10,9,6,0});

        // map . unique
        auto c_c8 = lazy::map(c_c3.begin(), c_c3.end(), [](auto x){return x-1;});
        c_check(c_c8.begin(), c_c8.end(), {0,4,5,6});

        // zip . filter
        auto c_c9 = lazy::zip(c_c5.begin(), c_c5.end(), c_c4.begin(), c_c4.end(), [](auto x, auto y){return x+y;});
        c_check(c_c9.begin(), c_c9.end(), {16});

        // zip. map
        auto c_c10 = lazy::zip(c_c2.begin(), c_c2.end(), c_c8.begin(), c_c8.end(), [](auto x, auto y){return x-y;});
        c_check(c_c10.begin(), c_c10.end(), {10, 5, 1, 4});

        // zip. unique
        auto c_c11 = lazy::zip(c_c6.begin(), c_c6.end(), c_c7.begin(), c_c7.end(), [](auto x, auto y){return x+y;});
        c_check(c_c11.begin(), c_c11.end(), {20, 15, 6});

        // map . filter
        auto c_c12 = lazy::map(c_c5.begin(), c_c5.end(), [](int x){return x/2;});
        c_check(c_c12.begin(), c_c12.end(), {5,3,5,0});

        // map . map
        auto c_c13 = lazy::map(c_c12.begin(), c_c12.end(), [](int x){return x-1;});
        c_check(c_c13.begin(), c_c13.end(), {4,2,4,-1});

        // filter . filter
        auto c_c14 = lazy::filter(c_c5.begin(), c_c5.end(), [](int x){return x>0;});
        c_check(c_c14.begin(), c_c14.end(), {10,6,10});

        // unique . filter
        auto c_c15 = lazy::unique(c_c14.begin(), c_c14.end());
        c_check(c_c15.begin(), c_c15.end(), {10,6});

        // unique . zip
        auto c_c16 = lazy::unique(c_c9.begin(), c_c9.end());
        c_check(c_c16.begin(), c_c16.end(), {16});
    }

    SECTION("combined tests v2")
    {
        int testCArray[4] = { 1, 2, 3, 4 };
        bool testCArrayBool[3] = {true, false, true};

        lazy::MapIt<int *, int> c_mapIt(testCArray, [](int x){return x+1;});
        lazy::MapIt<int *, int> c_mapIt2(testCArray+4, [](int x){return x+1;});
        c_check(c_mapIt, c_mapIt2, {2,3,4,5});

        REQUIRE_FALSE(c_mapIt == c_mapIt2);

        auto tmp = c_mapIt;
        REQUIRE(++c_mapIt == ++tmp);
        REQUIRE(*c_mapIt == 3);

        lazy::ZipIt<int *, bool *, int> c_zipIt(testCArray, testCArrayBool, [](int x, bool y){if(y) return x; return -1;} );
        lazy::ZipIt<int *, bool *, int> c_zipIt2(testCArray+4, testCArrayBool+3, [](int x, bool y){if(y) return x; return -1;} );
        c_check(c_zipIt, c_zipIt2, {1, -1, 3});

        REQUIRE_FALSE(c_zipIt == c_zipIt2);

        auto tmp2 = c_zipIt;
        REQUIRE(++c_zipIt == ++tmp2);
        REQUIRE(*c_zipIt == -1);

        lazy::FilterIt<bool*> c_filterIt(testCArrayBool, testCArrayBool + 3, [](bool x){return x;});
        lazy::FilterIt<bool*> c_filterIt2(testCArrayBool + 3, testCArrayBool + 3, [](bool x){return x;});
        c_check(c_filterIt, c_filterIt2, {true, true});

        REQUIRE_FALSE(c_filterIt == c_filterIt2);

        auto tmp3 = c_filterIt;
        REQUIRE(++c_filterIt == ++tmp3);
        REQUIRE(*c_filterIt == true);

        auto tmp4 = lazy::unique(testCArrayBool, testCArrayBool+3);
        c_check(tmp4.begin(), tmp4.end(), {true, false});

        REQUIRE_FALSE(tmp4.begin() == tmp4.end());
        auto tmp5 = tmp4.begin();
        REQUIRE(++tmp4.begin() == ++tmp5);
        REQUIRE(*tmp5 == false);
        REQUIRE(++tmp5 == tmp4.end());

        // Test nullptr passed to constructor

        lazy::MapIt<int *, int> cn_mapIt(nullptr, [](int x){return x+1;});
        lazy::MapIt<int *, int> cn_mapIt2(nullptr, [](int x){return x+1;});
        REQUIRE(cn_mapIt == cn_mapIt2);

        lazy::FilterIt<bool*> cn_filterIt(nullptr, nullptr, [](bool x){return x;});
        lazy::FilterIt<bool*> cn_filterIt2(nullptr, nullptr, [](bool x){return x;});
        REQUIRE(cn_filterIt == cn_filterIt2);

        lazy::ZipIt<int *, bool *, int> cn_zipIt(nullptr, nullptr, [](int x, bool y){if(y) return x; return -1;} );
        lazy::ZipIt<int *, bool *, int> cn_zipIt2(nullptr, nullptr, [](int x, bool y){if(y) return x; return -1;} );
        REQUIRE(cn_zipIt == cn_zipIt2);

        lazy::MapIt<int*, int> defMapIt;
        lazy::ZipIt<int*, bool*, int> defZipIt;
        lazy::FilterIt<bool*> defFiltIt;

        REQUIRE_FALSE(c_mapIt == defMapIt);
        REQUIRE_FALSE(defMapIt == c_mapIt);
        REQUIRE_FALSE(c_zipIt == defZipIt);
        REQUIRE_FALSE(defZipIt == c_zipIt);
        REQUIRE_FALSE(c_filterIt == defFiltIt);
        REQUIRE_FALSE(defFiltIt == cn_filterIt);
    }

#endif
}
