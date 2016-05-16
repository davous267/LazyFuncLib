/*
 * HW01, lazy functional library
 * Author: David Kuťák, 433409
 */
#pragma once

#include <type_traits>
#include <iterator>
#include <functional>
#include <utility>
#include <memory>
#include <unordered_set>

namespace lazy
{

/**
 * Nested namespace containing additional helper classes/functions
 */
namespace helper
{

/**
 * UniqueFunc is functor passed to FilterIt to achieve unique functionality
 */
template<typename T>
class UniqueFunc
{
private:
    std::unordered_set<T> mFoundValues;

    bool isUnique(T res)
    {
        auto tmp = mFoundValues.insert(res);
        return tmp.second;
    }
public:
    UniqueFunc():mFoundValues(std::unordered_set<T>()) {}

    bool operator()(T val)
    {
        return isUnique(val);
    }
};

/**
 * Functions used to deduce tag of custom iterator
 */
constexpr bool isInputIterator(std::input_iterator_tag)
{
    return true;
}

template<typename T>
constexpr bool isInputIterator(T)
{
    return false;
}

/**
 * If input iterator is passed, resulting type is std::input_iterator_tag, otherwise std::forward_iterator_tag
 */
template<typename Iterator,
         typename Result = typename std::conditional<isInputIterator(typename std::iterator_traits<Iterator>::iterator_category()),
                                                     std::input_iterator_tag,
                                                     std::forward_iterator_tag>::type
         >
constexpr Result getIteratorType()
{
    return Result();
}

}

/**
 * ITERATORS
 */

/**
 * Range struct contains and provides begin & end iterators of given object
 */
template<typename Iter>
struct Range
{
private:
    Iter mBeg;
    Iter mEnd;
public:
    typedef typename std::iterator_traits<Iter>::value_type value_type;
    typedef typename std::iterator_traits<Iter>::reference reference;
    typedef typename std::iterator_traits<Iter>::pointer pointer;

    Range(Iter beg, Iter end)
        :mBeg(beg), mEnd(end) {}

    Range(const Range<Iter>& other) = default;

    Iter begin() const
    {
        return mBeg;
    }

    Iter end() const
    {
        return mEnd;
    }
};

/**
 * MapIt is iterator designed for (lazy) map function
 * Operates with underlying operator, applies (on demand) unary function to default values and returns new result
 * Iterator tag is equal to type returned by helper::getIteratorType function
 */
template<typename Iter, typename Result>
class MapIt : public std::iterator<decltype(helper::getIteratorType<Iter>()), Result>
{
    private:
    using valType = typename std::iterator_traits<Iter>::value_type;
    using tUnFunc = std::function<Result(const valType&)>;

    std::unique_ptr<Iter> mDataIterator;
    std::unique_ptr<Result> mLastResult;
    bool mIsResultActual;
    tUnFunc mUnaryFunction;

    void makeStep()
    {
        ++*mDataIterator;
        mIsResultActual = false;
    }

    void swap(MapIt& other)
    {
        using std::swap;
        swap(mDataIterator, other.mDataIterator);
        swap(mLastResult, other.mLastResult);
        swap(mIsResultActual, other.mIsResultActual);
        swap(mUnaryFunction, other.mUnaryFunction);
    }

    public:
    MapIt()
        : mDataIterator(nullptr), mLastResult(nullptr), mIsResultActual(false), mUnaryFunction(tUnFunc())
    { }

    MapIt(Iter dataIterator, tUnFunc unaryFunction)
        : mDataIterator(std::make_unique<Iter>(dataIterator)), mLastResult(nullptr), mIsResultActual(false), mUnaryFunction(unaryFunction)
    { }

    MapIt(const MapIt& other)
        :mDataIterator(other.mDataIterator ? std::make_unique<Iter>(*other.mDataIterator) : nullptr),
          mLastResult(other.mLastResult ? std::make_unique<Result>(*other.mLastResult) : nullptr),
          mIsResultActual(other.mIsResultActual), mUnaryFunction(other.mUnaryFunction)
    { }

    MapIt& operator=(MapIt other)
    {
        swap(other);
        return *this;
    }

    MapIt& operator++()
    {
        makeStep();
        return *this;
    }

    MapIt operator++(int)
    {
        auto tmp = *this;
        makeStep();
        return tmp;
    }

    const Result& operator*()
    {
        if(mIsResultActual)
            return *mLastResult;

        auto origData = *(*mDataIterator);
        if(!mLastResult)
            mLastResult = std::make_unique<Result>(mUnaryFunction(origData));
        else
            *mLastResult = mUnaryFunction(origData);
        mIsResultActual = true;
        return *mLastResult;
    }

    const Result* operator->()
    {
        return &(operator*());
    }

    ~MapIt() = default;

    template<typename I, typename R>
    friend bool operator==(const MapIt<I, R>&, const MapIt<I, R>&);

    template<typename I, typename R>
    friend bool operator!=(const MapIt<I, R>&, const MapIt<I, R>&);
};

template<typename I, typename R>
bool operator==(const MapIt<I, R>& lhs, const MapIt<I, R>& rhs)
{
    return lhs.mDataIterator && rhs.mDataIterator ? *lhs.mDataIterator == *rhs.mDataIterator : lhs.mDataIterator == rhs.mDataIterator;
}

template<typename I, typename R>
bool operator!=(const MapIt<I, R>& lhs,const MapIt<I, R>& rhs)
{
    return !(lhs == rhs);
}

/**
 * FilterIt is iterator for filter function
 * Contains two underlying operators which determine the range of container => new "container" contains only values which are evaluated by predicate as true
 * Iterator tag is equal to type returned by helper::getIteratorType function
 */
template<typename Iter>
class FilterIt : public std::iterator<decltype(helper::getIteratorType<Iter>()), typename std::iterator_traits<Iter>::value_type>
{
    private:
    using Result = typename std::iterator_traits<Iter>::value_type;
    using tUnFunc = std::function<bool(const Result&)>;

    std::unique_ptr<Iter> mDataIterator_beg;
    std::unique_ptr<Iter> mDataIterator_end;
    tUnFunc mUnaryPredicate;

    void moveToFirst()
    {
        if(*mDataIterator_beg == *mDataIterator_end) return;

        if(!mUnaryPredicate(*(*mDataIterator_beg)))
            makeStep();
    }

    void makeStep()
    {
        if(*mDataIterator_beg == *mDataIterator_end) return;

        while(++*mDataIterator_beg != *mDataIterator_end &&
              !mUnaryPredicate(*(*mDataIterator_beg)));
    }

    void swap(FilterIt& other)
    {
        using std::swap;
        swap(mDataIterator_beg, other.mDataIterator_beg);
        swap(mDataIterator_end, other.mDataIterator_end);
        swap(mUnaryPredicate, other.mUnaryPredicate);
    }

    public:
    FilterIt()
        :mDataIterator_beg(nullptr), mDataIterator_end(nullptr), mUnaryPredicate(tUnFunc())
    { }

    FilterIt(Iter dataIterator_beg, Iter dataIterator_end, tUnFunc unaryPredicate)
        :mDataIterator_beg(std::make_unique<Iter>(dataIterator_beg)), mDataIterator_end(std::make_unique<Iter>(dataIterator_end)), mUnaryPredicate(unaryPredicate)
    {
        // This stuff here is actually not "lazy", but it seems necessary to have it here
        // to ensure the case when no element of given range will be present in resulting one
        // so the resulting will be empty and without this begin & end iterators would not be equal although they should
        moveToFirst();
    }

    FilterIt(const FilterIt& other)
        :mDataIterator_beg(other.mDataIterator_beg ? std::make_unique<Iter>(*other.mDataIterator_beg) : nullptr),
          mDataIterator_end(other.mDataIterator_end ? std::make_unique<Iter>(*other.mDataIterator_end) : nullptr),
          mUnaryPredicate(other.mUnaryPredicate)
    { }

    FilterIt& operator=(FilterIt other)
    {
        swap(other);
        return *this;
    }

    FilterIt& operator++()
    {
        makeStep();
        return *this;
    }

    FilterIt operator++(int)
    {
        auto tmp = *this;
        makeStep();
        return tmp;
    }

    const Result& operator*()
    {
        return *(*mDataIterator_beg);
    }

    const Result* operator->()
    {
        return &(operator*());
    }

    ~FilterIt() = default;

    template<typename I>
    friend bool operator==(const FilterIt<I>&, const FilterIt<I>&);

    template<typename I>
    friend bool operator!=(const FilterIt<I>&, const FilterIt<I>&);
};

template<typename I>
bool operator==(const FilterIt<I>& lhs, const FilterIt<I>& rhs)
{
    return lhs.mDataIterator_beg && rhs.mDataIterator_beg ? *lhs.mDataIterator_beg == *rhs.mDataIterator_beg : lhs.mDataIterator_beg == rhs.mDataIterator_beg;
}

template<typename I>
bool operator!=(const FilterIt<I>& lhs, const FilterIt<I>& rhs)
{
    return !(lhs == rhs);
}

/**
 * ZipIt is iterator for zip functions
 * Contains two iterators, each one from (not necessarily) different container and applies binary function to each pair from those containers (on demand)
 * Iterator tag is equal to "weaker" type of two iterators:
 * if helper::getIteratorType function returns std::input_iterator_tag for one of iterators, resulting tag is also std::input_iterator_tag, otherwise std::forward_iterator_tag
 */
template<typename Iter1, typename Iter2, typename Result>
class ZipIt : public std::iterator<typename std::conditional<helper::isInputIterator(helper::getIteratorType<Iter1>()),
                                                             decltype(helper::getIteratorType<Iter1>()),
                                                             decltype(helper::getIteratorType<Iter2>())>::type,
                                   Result>
{
    private:
    template<typename I>
    using valType = typename std::iterator_traits<I>::value_type;
    using tBinFunc = std::function<Result(const valType<Iter1>&,
    const valType<Iter2>&)>;

    std::unique_ptr<Iter1> mDataIterator1;
    std::unique_ptr<Iter2> mDataIterator2;
    tBinFunc mBinaryFunction;
    std::unique_ptr<Result> mLastResult;
    bool mIsResultActual;

    void makeStep()
    {
        ++*mDataIterator1;
        ++*mDataIterator2;
        mIsResultActual = false;
    }

    void swap(ZipIt& other)
    {
        using std::swap;
        swap(mDataIterator1, other.mDataIterator1);
        swap(mDataIterator2, other.mDataIterator2);
        swap(mBinaryFunction, other.mBinaryFunction);
        swap(mLastResult, other.mLastResult);
        swap(mIsResultActual, other.mIsResultActual);
    }

    public:
    ZipIt()
        :mDataIterator1(nullptr), mDataIterator2(nullptr), mBinaryFunction(tBinFunc()), mLastResult(nullptr), mIsResultActual(false)
    {}

    ZipIt(Iter1 dataIterator1, Iter2 dataIterator2, tBinFunc binaryFunction)
        :mDataIterator1(std::make_unique<Iter1>(dataIterator1)), mDataIterator2(std::make_unique<Iter2>(dataIterator2)),
          mBinaryFunction(binaryFunction), mLastResult(nullptr), mIsResultActual(false)
    { }

    ZipIt(const ZipIt& other)
        :mDataIterator1(other.mDataIterator1 ? std::make_unique<Iter1>(*other.mDataIterator1) : nullptr),
          mDataIterator2(other.mDataIterator2 ? std::make_unique<Iter2>(*other.mDataIterator2) : nullptr),
          mBinaryFunction(other.mBinaryFunction),
          mLastResult(other.mLastResult ? std::make_unique<Result>(*other.mLastResult) : nullptr),
          mIsResultActual(other.mIsResultActual)
    { }

    ZipIt& operator=(ZipIt other)
    {
        swap(other);
        return *this;
    }

    ZipIt& operator++()
    {
        makeStep();
        return *this;
    }

    ZipIt operator++(int)
    {
        auto tmp = *this;
        makeStep();
        return tmp;
    }

    const Result& operator*()
    {
        if(mIsResultActual)
            return *mLastResult;

        auto origData1 = *(*mDataIterator1);
        auto origData2 = *(*mDataIterator2);
        if(!mLastResult)
            mLastResult = std::make_unique<Result>(mBinaryFunction(origData1, origData2));
        else
            *mLastResult = mBinaryFunction(origData1, origData2);
        mIsResultActual = true;
        return *mLastResult;
    }

    const Result* operator->()
    {
        return &(operator*());
    }

    ~ZipIt() = default;

    template<typename I1, typename I2, typename R>
    friend bool operator==(const ZipIt<I1, I2, R>&, const ZipIt<I1, I2, R>&);

    template<typename I1, typename I2, typename R>
    friend bool operator!=(const ZipIt<I1, I2, R>&, const ZipIt<I1, I2, R>&);
};

template<typename I1, typename I2, typename R>
bool operator==(const ZipIt<I1, I2, R>& lhs, const ZipIt<I1, I2, R>& rhs)
{
    return (lhs.mDataIterator1 && rhs.mDataIterator1 ? *lhs.mDataIterator1 == *rhs.mDataIterator1 : lhs.mDataIterator1 == rhs.mDataIterator1) ||
            (lhs.mDataIterator2 && rhs.mDataIterator2 ? *lhs.mDataIterator2 == *rhs.mDataIterator2 : lhs.mDataIterator2 == rhs.mDataIterator2);
}

template<typename I1, typename I2, typename R>
bool operator!=(const ZipIt<I1, I2, R>& lhs, const ZipIt<I1, I2, R>& rhs)
{
    return !(lhs == rhs);
}


/**
 * FUNCTIONS
 * map, filter, zip, unique
 */

template<typename Iterator, typename UnaryFunction>
auto map(Iterator first, Iterator last, UnaryFunction f)
{
    MapIt<Iterator, typename std::result_of<UnaryFunction(typename std::iterator_traits<Iterator>::value_type)>::type> beginIt(first, f);
    MapIt<Iterator, typename std::result_of<UnaryFunction(typename std::iterator_traits<Iterator>::value_type)>::type> endIt(last, f);

    return Range< MapIt<Iterator, typename std::result_of<UnaryFunction(typename std::iterator_traits<Iterator>::value_type)>::type> >(beginIt, endIt);
}


template<typename Iterator, typename UnaryPredicate>
auto filter(Iterator first, Iterator last, UnaryPredicate p)
{
    FilterIt<Iterator> beginIt(first, last, p);
    FilterIt<Iterator> endIt(last, last, p);

    return Range< FilterIt<Iterator> >(beginIt, endIt);
}


template< typename Iterator1, typename Iterator2, typename BinaryFunction >
auto zip(Iterator1 first1, Iterator1 last1,
         Iterator2 first2, Iterator2 last2,
         BinaryFunction f)
{
    ZipIt<Iterator1, Iterator2, typename std::result_of<BinaryFunction(typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type)>::type> beginIt(first1, first2, f);
    ZipIt<Iterator1, Iterator2, typename std::result_of<BinaryFunction(typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type)>::type> endIt(last1, last2, f);

    return Range< ZipIt<Iterator1, Iterator2, typename std::result_of<BinaryFunction(typename std::iterator_traits<Iterator1>::value_type, typename std::iterator_traits<Iterator2>::value_type)>::type> >(beginIt, endIt);
}


template< typename Iterator >
auto unique( Iterator first, Iterator last )
{
    FilterIt<Iterator> beginIt(first, last, helper::UniqueFunc<typename std::iterator_traits<Iterator>::value_type>());
    FilterIt<Iterator> endIt(last, last, helper::UniqueFunc<typename std::iterator_traits<Iterator>::value_type>());

    return Range< FilterIt<Iterator> >(beginIt, endIt);
}


} // namespace lazy
