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

enum sort_method : unsigned char
{
    DEFAULT,
    STABLE,
    UNIQUE,
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
                result.push_back(ret.value);            \
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
                result.push_back(ret.value);            \
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
        curr = begin_iterator;
        end = end_iterator;

#if 0
        std::cout << "{ Elements ";
        for(curr = begin; curr != end; )
            std::cout << *curr++ << ' ';
        std::cout << " }\n";
        curr = begin_iterator;
#endif
    }

    return_element<T> give_next()
    {
        if(curr == end)
            return return_element<T>(return_state::END);
        else
        {
            auto res = *curr;
            curr++;
            return return_element<T>(return_state::VALID, res);
        }
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
struct sort_t
{
    PREV prev;

    sort_method sort_style;
    bool dirty;
    std::function<int(T, T)> compare_function;
    std::vector<T> cache_vector;
    typename std::vector<T>::iterator curr;

    sort_t () {}
    sort_t(PREV c_prev)
    {
        prev = c_prev;
        dirty = false;
        sort_style = sort_method::UNIQUE;
    }
    sort_t(std::function<int(T, T)> c_compare_function, sort_method c_sort_method, PREV c_prev)
    {
        prev = c_prev;
        dirty = false;
        sort_style = c_sort_method;
        compare_function = c_compare_function;
    }

    return_element<T> give_next()
    {
        // Initialize.
        if(!dirty)
        {
            cache_vector.clear();
            while(1)
            {
                auto ret = prev.give_next();
                if(ret.state == return_state::END)
                    break;
                else
                    cache_vector.emplace_back(ret.value);
            }

            switch(sort_style)
            {
                case sort_method::DEFAULT:
                    std::sort(cache_vector.begin(), cache_vector.end(), compare_function);
                    break;

                case sort_method::STABLE:
                    std::stable_sort(cache_vector.begin(), cache_vector.end(), compare_function);
                    break;

                case sort_method::UNIQUE:
                    auto last = std::unique(cache_vector.begin(), cache_vector.end());
                    cache_vector.erase(last, cache_vector.end());
                    break;
            }
            curr = cache_vector.begin();
            dirty = true;
        }

        return (curr == cache_vector.end()
                ? return_element<T>(return_state::END)
                : return_element<T>(return_state::VALID, *curr++));
    }

    template<typename U>
    select_t<T, U, sort_t<T, PREV>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, sort_t<T, PREV>>(c_select_function, *this);
    }

    where_t<T, sort_t<T, PREV>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, sort_t<T, PREV>>(c_where_function, *this);
    }

    take_t<T, sort_t<T, PREV>> take(size_t c_take_amount)
    {
        return take_t<T, sort_t<T, PREV>>(c_take_amount, *this);
    }

    skip_t<T, sort_t<T, PREV>> skip(size_t c_take_amount)
    {
        return skip_t<T, sort_t<T, PREV>>(c_take_amount, *this);
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

    sort_t<T, where_t<T, PREV>> sort(std::function<int(T, T)> c_sort_function)
    {
        return sort_t<T, where_t<T, PREV>>(c_sort_function, sort_method::DEFAULT, *this);
    }

    sort_t<T, where_t<T, PREV>> stable_sort(std::function<int(T, T)> c_sort_function)
    {
        return sort_t<T, where_t<T, PREV>>(c_sort_function, sort_method::STABLE, *this);
    }

    sort_t<T, where_t<T, PREV>> unique()
    {
        return sort_t<T, where_t<T, PREV>>(*this);
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
    where(std::function<bool(U)> c_where_function)
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

    CONVERSION_FUNCIONS(U);
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
    return from_iterator_t<typename ITERABLE::value_type,
                           typename ITERABLE::iterator>(
                               iterable.begin(), iterable.end());
}



///////////////////////////////////////////////////////////////////



static auto TEST_ID = 1;

template<typename ITER>
inline static void print_with_message(char const* msg, ITER begin, ITER end)
{
    std::cout << msg;
    std::for_each(begin, end, [](auto x) { std::cout << x << ' '; });
    std::cout << '\n';
}

template<typename ITER>
inline static void print_test_msg(char const* msg, ITER begin, ITER end)
{
    std::cout << "\nTest" << TEST_ID++ << ": ";
    print_with_message(msg, begin, end);
}

#include <string_view>

int main()
{
    {
        auto text = "MaTeUSz";
        print_test_msg("", text, text + 7);

        auto result = from(text, 7)
            .where([](auto x) { return isupper(x); })
            .to_vector();

        print_with_message("Upcase characters: ",
                           result.begin(), result.end());
    }

    {
        auto numbers = std::array { 5, 4, 1, 3, 9, 8, 6, 7, 2, 0 };
        print_test_msg("", numbers.begin(), numbers.end());

        auto result = from(numbers)
            .where([](auto x) { return x < 5; })
            .skip(1)
            .to_vector();

        print_with_message("Numbers < 5 (without the first one): ",
                           result.begin(), result.end());
    }

    {
        auto numbers = std::vector { 5, 4, 1, 3, 9, 8, 6, 7, 2, 1, 2, 4, 0 };
        auto strings = std::vector<std::string_view> { "zero", "one", "two", "three",
                                                       "four", "five", "six", "seven",
                                                       "eight", "nine" };

        print_test_msg("", numbers.begin(), numbers.end());

        auto result = from(numbers)
            .select<std::string_view>([&strings](auto x) { return strings[x]; })
            .where([](auto x) { return true; })
            .sort([](auto x, auto y) { return std::lexicographical_compare(x.begin(), x.end(),
                                                                           y.begin(), y.end()); })
            .where([](auto x) { return true; })
            .unique()
            .where([](auto x) { return true; })
            .to_vector();

        print_with_message("Numbers mapped to strings (unique'd): ",
                           result.begin(), result.end());
    }

#if 1
    {
        auto pairs = std::vector<std::pair<int, int>> { {1, 2}, {2, 0}, {1, 3}, {1, 4}, {1, -2} };
        std::cout << "Stable sorted pairs: ";
        for(auto v : pairs)
            std::cout << "{ " << v.first << ", " << v.second << " } ";
        std::cout << "\n";

        auto result = from(pairs)
            .where([](auto x) { return x.first > 0; })
            .stable_sort([](auto x, auto y) { return x.first < y.first; })
            .to_vector();

        std::cout << "Stable sorted pairs: ";
        for(auto v : result)
            std::cout << "{ " << v.first << ", " << v.second << " } ";
        std::cout << "\n";
    }
#endif

    return 0;
}
