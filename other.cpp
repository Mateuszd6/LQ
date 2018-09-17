#include <iostream>
#include <functional>

#include <vector>
#include <list>

struct foobar
{
    void debug_log() { printf("Hello wolrd"); }
};

enum struct return_state : unsigned char
{
    NONE = 0,
    VALID = 1,
    END = 2,
};

template<typename T>
struct return_element
{
    return_state state;
    T value;

    return_element()
    {
    }

    return_element(return_state c_state)
    {
        this->state = c_state;
    }

    return_element(return_state c_state, T c_value)
    {
        this->state = c_state;
        this->value = c_value;
    }
};

#define CONVERSION_FUNCIONS(RET_TYPE)                   \
    std::vector<RET_TYPE> to_vector()                   \
    {                                                   \
        auto result = std::vector<RET_TYPE>();          \
        while(1)                                        \
        {                                               \
            auto ret = give_next();                     \
                                                        \
            if(ret.state == return_state::END)          \
                break;                                  \
            else if(ret.state == return_state::VALID)   \
                result.emplace_back(ret.value);         \
        }                                               \
                                                        \
        return result;                                  \
    }                                                   \
                                                        \
    std::list<RET_TYPE> to_list()                       \
    {                                                   \
        auto result = std::list<RET_TYPE>();            \
        while(1)                                        \
        {                                               \
            auto ret = give_next();                     \
                                                        \
            if(ret.state == return_state::END)          \
                break;                                  \
            else if(ret.state == return_state::VALID)   \
                result.emplace_back(ret.value);         \
        }                                               \
                                                        \
        return result;                                  \
    }

template<typename T> struct from_t;
template<typename T, typename PREV> struct where_t;
template<typename T, typename U, typename PREV> struct select_t;
template<typename T, typename PREV> struct take_t;
template<typename T, typename PREV> struct skip_t;

#define LINQ_FUCTIONS(CURR_TYPE, TO_TYPE, PREVIOUS_TYPE)                \
    template<typename _U>                                               \
    select_t<TO_TYPE, _U, PREVIOUS_TYPE>                                \
    select(std::function<_U(CURR_TYPE)> c_select_function)              \
    {                                                                   \
        return select_t<TO_TYPE, _U, PREVIOUS_TYPE>(c_select_function,  \
                                                    *this);             \
    }                                                                   \
                                                                        \
    where_t<CURR_TYPE, PREVIOUS_TYPE>                                   \
    where(std::function<bool(CURR_TYPE)> c_where_function)              \
    {                                                                   \
        return where_t<CURR_TYPE, PREVIOUS_TYPE>(c_where_function,      \
                                                 *this);                \
    }                                                                   \
                                                                        \
    take_t<CURR_TYPE, PREVIOUS_TYPE>                                    \
    take(size_t amount)                                                 \
    {                                                                   \
        return take_t<CURR_TYPE, PREVIOUS_TYPE>(amount, *this);         \
    }                                                                   \
                                                                        \
    skip_t<CURR_TYPE, PREVIOUS_TYPE>                                    \
    skip(size_t amount)                                                 \
    {                                                                   \
        return skip_t<CURR_TYPE, PREVIOUS_TYPE>(amount, *this);         \
    }

template<typename T>
struct from_raw_array_t
{
    // TODO: Remove!
    from_raw_array_t() {}
    from_raw_array_t(T const* c_arr, std::size_t c_len)
    {
        this->arr = c_arr;
        this->len = c_len;
        this->curr = 0;
    }

    T const* arr;
    std::size_t len;
    std::size_t curr;

    return_element<T> give_next()
    {
        return (curr == len
                ? return_element<T>(return_state::END)
                : return_element<T>(return_state::VALID, *(arr + curr++)));
    }

    ///
    template<typename U>
    select_t<T, U, from_raw_array_t<T>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, from_raw_array_t<T>>(c_select_function, *this);
    }

    where_t<T, from_raw_array_t<T>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, from_raw_array_t<T>>(c_where_function, *this);
    }

    take_t<T, from_raw_array_t<T>> take(size_t c_take_amount)
    {
        return take_t<T, from_raw_array_t<T>>(c_take_amount, *this);
    }

    skip_t<T, from_raw_array_t<T>> skip(size_t c_take_amount)
    {
        return skip_t<T, from_raw_array_t<T>>(c_take_amount, *this);
    }

    CONVERSION_FUNCIONS(T);
};

template<typename T, typename ITER>
struct from_iterator_t
{
    ITER begin;
    ITER end;
    ITER curr;

    from_iterator_t() {}
    from_iterator_t(ITER begin_iterator, ITER end_iterator)
    {
        begin = begin_iterator;
        curr = begin;
        end = end_iterator;
    }

    return_element<T> give_next()
    {
        if(curr == end)
            return return_element<T>(return_state::END);
        else
            return return_element<T>(return_state::VALID, *curr++);
    }

    ///
    template<typename U>
    select_t<T, U, from_iterator_t<T, ITER>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, from_iterator_t<T, ITER>>(c_select_function, *this);
    }

    where_t<T, from_iterator_t<T, ITER>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, from_iterator_t<T, ITER>>(c_where_function, *this);
    }

    take_t<T, from_iterator_t<T, ITER>> take(size_t c_take_amount)
    {
        return take_t<T, from_iterator_t<T, ITER>>(c_take_amount, *this);
    }

    skip_t<T, from_iterator_t<T, ITER>> skip(size_t c_take_amount)
    {
        return skip_t<T, from_iterator_t<T, ITER>>(c_take_amount, *this);
    }

    CONVERSION_FUNCIONS(T);
};

template<typename T, typename PREV>
struct where_t
{
    PREV prev;
    std::function<bool(T)> where_function;

    where_t () {}
    where_t(std::function<bool(T)> c_where_function, PREV c_prev)
    {
        prev = c_prev;
        where_function = c_where_function;
    }

    return_element<T> give_next()
    {
        return_element<T> ret;

        while(1)
        {
            ret = prev.give_next();
            if(ret.state == return_state::END)
                return ret;
            else if(ret.state == return_state::VALID && where_function(ret.value))
                return ret;
        }
    }

    template<typename U>
    select_t<T, U, where_t<T, PREV>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, where_t<T, PREV>>(c_select_function, *this);
    }

    where_t<T, where_t<T, PREV>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, where_t<T, PREV>>(c_where_function, *this);
    }

    take_t<T, where_t<T, PREV>> take(size_t c_take_amount)
    {
        return take_t<T, where_t<T, PREV>>(c_take_amount, *this);
    }

    skip_t<T, where_t<T, PREV>> skip(size_t c_take_amount)
    {
        return skip_t<T, where_t<T, PREV>>(c_take_amount, *this);
    }

    CONVERSION_FUNCIONS(T);
};

template<typename T, typename PREV>
struct take_t
{
    PREV prev;
    size_t take_amount;
    size_t took;

    take_t() {}
    take_t(size_t c_take_amount, PREV c_prev)
    {
        prev = c_prev;
        take_amount = c_take_amount;
        took = 0;
    }

    return_element<T> give_next()
    {
        return_element<T> ret;
        took++;
        ret = prev.give_next();

        if(ret.state == return_state::END || took > take_amount)
            return return_element<T>(return_state::END);
        else if(ret.state == return_state::VALID)
            return ret;

        else
            return ret;
    }

    template<typename U>
    select_t<T, U, take_t<T, PREV>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, take_t<T, PREV>>(c_select_function, *this);
    }

    where_t<T, take_t<T, PREV>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, take_t<T, PREV>>(c_where_function, *this);
    }

    take_t<T, take_t<T, PREV>> take(size_t c_take_amount)
    {
        return take_t<T, take_t<T, PREV>>(c_take_amount, *this);
    }

    skip_t<T, take_t<T, PREV>> skip(size_t c_take_amount)
    {
        return skip_t<T, take_t<T, PREV>>(c_take_amount, *this);
    }

    CONVERSION_FUNCIONS(T);
};

template<typename T, typename PREV>
struct skip_t
{
    PREV prev;
    size_t skip_amount;
    size_t skipped_already;

    skip_t() {}
    skip_t(size_t c_skip_amount, PREV c_prev)
    {
        prev = c_prev;
        skip_amount = c_skip_amount;
        skipped_already = 0;
    }

    return_element<T> give_next()
    {
        return_element<T> ret;
        while(skipped_already < skip_amount)
        {
            ret = prev.give_next();
            skipped_already++;

            if(ret.state == return_state::END)
                return ret;
        }

        ret = prev.give_next();
        if(ret.state == return_state::END)
            return ret;
        else if(ret.state == return_state::VALID)
            return ret;

        else
            return ret;
    }

    template<typename U>
    select_t<T, U, skip_t<T, PREV>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, skip_t<T, PREV>>(c_select_function, *this);
    }

    where_t<T, skip_t<T, PREV>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, skip_t<T, PREV>>(c_where_function, *this);
    }

    take_t<T, skip_t<T, PREV>> take(size_t c_take_amount)
    {
        return take_t<T, skip_t<T, PREV>>(c_take_amount, *this);
    }

    skip_t<T, skip_t<T, PREV>> skip(size_t c_take_amount)
    {
        return skip_t<T, skip_t<T, PREV>>(c_take_amount, *this);
    }

    CONVERSION_FUNCIONS(T);
};

template<typename T, typename U, typename PREV>
struct select_t
{
    PREV prev;
    std::function<U(T)> select_function;

    // TODO: Remove!
    select_t() {}

    select_t(std::function<U(T)> c_select_function, PREV c_prev)
    {
        prev = c_prev;
        select_function = c_select_function;
    }

    return_element<U> give_next()
    {
        return_element<T> ret;
        while(1)
        {
            ret = prev.give_next();

            if(ret.state == return_state::END)
                return return_element<U>(return_state::END);
            else if(ret.state == return_state::VALID)
                return return_element<U>(return_state::VALID,
                                         select_function(ret.value));
        }
    }

    ///
    template<typename H>
    select_t<U, H, select_t<T, U, PREV>>
    select(std::function<H(T)> c_select_function)
    {
        return select_t<U, H, select_t<T, U, PREV>>(c_select_function, *this);
    }

    where_t<U, select_t<T, U, PREV>>
    where(std::function<bool(T)> c_where_function)
    {
        return where_t<U, select_t<T, U, PREV>>(c_where_function, *this);
    }

    take_t<U, select_t<T, U, PREV>>
    take(size_t c_take_amount)
    {
        return take_t<U, select_t<T, U, PREV>>(c_take_amount, *this);
    }

    skip_t<U, select_t<T, U, PREV>>
    skip(size_t c_skip_amount)
    {
        return skip_t<U, select_t<T, U, PREV>>(c_skip_amount, *this);
    }

    CONVERSION_FUNCIONS(T);
};

template<typename T>
from_raw_array_t<T>
from(T const* arr, std::size_t len)
{
    return from_raw_array_t(arr, len);
}

#if 0
template<typename T, std::size_t N>
from_iterator_t<T, typename std::array<T, N>::iterator>
from(std::array<T, N> arr)
{
    return from_iterator_t<T, decltype(arr.begin())>(arr.begin(), arr.end());
}
#endif

#if 0
template<typename T>
from_iterator_t<T, typename std::vector<T>::iterator>
from(std::vector<T> vec)
{
    return from_iterator_t<T, decltype(vec.begin())>(vec.begin(), vec.end());
}
#endif

template<typename ITERABLE>
from_iterator_t<typename ITERABLE::value_type, typename ITERABLE::iterator>
from(ITERABLE iterable)
{
    return from_iterator_t<typename ITERABLE::value_type, typename ITERABLE::iterator>(iterable.begin(), iterable.end());
}

int main()
{
    {
        auto text = "MaTeUSz";
        auto test = from(text, 7)
            .select<int>([](auto x) { return static_cast<int>(x % 5); })
            .where([](auto x) { return x % 2 == 0; })
            .skip(2)
            .take(3)
            .to_vector();

        for(auto v : test)
            printf("%d ", v);
        printf("\n");
    }

    {
        auto numbers = std::array { 5, 4, 1, 3, 9, 8, 6, 7, 2, 0 };
        auto result = from(numbers)
            .where([](auto x) { return x < 5; })
            .to_vector()
            ;

        std::cout << "Numbers < 5: ";
        std::for_each(result.begin(), result.end(),
                      [](auto x) { std::cout << x << ' '; });
        std::cout << '\n';
    }

    return 0;
}
