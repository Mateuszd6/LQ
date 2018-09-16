#include <cstdio>
#include <functional>

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

template<typename T> struct from_t;
template<typename T, typename PREV> struct where_t;
template<typename T, typename U, typename PREV> struct select_t;

#define LINQ_FUNCTIONS(TYPENAME, CLASS_TYPE)                                          \
template<typename U>                                                                  \
select_t<TYPENAME, U, CLASS_TYPE> select(std::function<U(TYPENAME)> select_function)  \
{                                                                                     \
    return select_t<TYPENAME, U, CLASS_TYPE>(select_function, *this);                 \
}                                                                                     \
                                                                                      \
where_t<TYPENAME, CLASS_TYPE> where(std::function<bool(TYPENAME)> c_where_function)   \
{                                                                                     \
    return where_t<TYPENAME, CLASS_TYPE>(c_where_function);                           \
}

template<typename T>
struct from_t
{
    // TODO: Remove!
    from_t() {}

    from_t(T const* c_arr, std::size_t c_len)
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
    select_t<T, U, from_t<T>>
    select(std::function<U(T)> c_select_function)
    {
        return select_t<T, U, from_t<T>>(c_select_function, *this);
    }

    where_t<T, from_t<T>> where(std::function<bool(T)> c_where_function)
    {
        return where_t<T, from_t<T>>(c_where_function, *this);
    }
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

    ///
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
};

namespace mquery
{
    template<typename T>
    from_t<T> from(T const* arr, std::size_t len)
    {
        return from_t(arr, len);
    }
}

using namespace mquery;

int main()
{
    auto text = "MaTeUSz";

#if 0
    auto from = from_t<char>(text, 7);
    auto where = where_t<char, decltype(from)>();
    where.prev = from;
    where.where_function = [](char x) { return x >= 'A' && x <= 'Z'; };
    auto select = select_t<char, int, decltype(where)>();
    select.prev = where;
    select.select_function = [](char x) { return static_cast<int>(x % 2); };
#endif

    // auto test = from.select<int>();
    auto test =
        from(text, std::size_t { 7 })
         .where ([](char x) { return x >= 'A' && x <= 'Z'; })
         .select<int>([](char x) { return static_cast<int>(x % 2); })
        ;


    while(1)
    {
        auto ret = test.give_next();

        if(ret.state == return_state::END)
            break;
        else if(ret.state == return_state::VALID)
            printf("%d ", ret.value);
    }
    printf("\n");

    return 0;
}
