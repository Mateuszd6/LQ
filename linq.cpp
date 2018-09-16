#include <functional>

#include <vector>

namespace detail
{
    template<typename T, typename U>
    struct query_node
    {
        union impl
        {
            struct where
            {
                std::function<bool(T)> where_function;
            };

            struct sort
            {
                std::function<int(T, T)> sort_function;
            };

            struct select
            {
                std::function<U(T)> select_function;
            };
        };
    };

    template<typename T, typename U>
    struct query
    {
        T const* start;
        size_t len;

        query_node<T, U> _[16];

#if 1
        std::function<bool(T)> where_function;
        std::function<int(T, T)> sort_function;
#endif

        query(T const* start, size_t len);

        query<T, U> where(std::function<bool(T)> predicate);
        query<T, U> sort(std::function<int(T, T)> compare);

        void select() { }
        void stable_sort() { }
        void partition() { }

        ::std::vector<T> to_vector();
    };

    template<typename T>
    query<T, T> from(T const* start, size_t len)
    {
        return query<T, T>(start, len);
    }

    ///////////////////////////////////////////

    template<typename T, typename U>
    query<T, U>::query(T const* start, size_t len)
    {
        this->start = start;
        this->len = len;
    }

    template<typename T, typename U>
    query<T, U> query<T, U>::where(std::function<bool(T)> predicate)
    {        auto result = *this;
        result.where_function = predicate;
        return result;
    }

    template<typename T, typename U>
    query<T, U> query<T, U>::sort(std::function<int(T, T)> compare)
    {
        auto result = *this;
        result.sort_function = compare;
        return result;
    }

    template<typename T, typename U>
    ::std::vector<T> query<T, U>::to_vector()
    {
        auto begin = start;
        auto end = start + len;
        ::std::vector<T> result;
        result.reserve(end - begin);

        for(auto i = begin; i != end; ++i)
        {
            if(where_function(*i))
                result.push_back(*i);
        }

        std::sort(result.begin(), result.end(), sort_function);
        return result;
    }
}

using namespace detail;

#include <cstdio>
#include <string>
#include <cctype>

int main()
{
    auto int_ptr = static_cast<int*>(std::malloc(sizeof(int) * 7));
    int_ptr[0] = 1;
    int_ptr[1] = 15;
    int_ptr[2] = 16;
    int_ptr[3] = 8;
    int_ptr[4] = -2;
    int_ptr[5] = 4;
    int_ptr[6] = 3;

    auto test = from(int_ptr, 7)
        .where([](int x) { return x % 2 == 0 && x > 0; })
        .sort([](int x, int y) { return x >= y; })
        .to_vector();

    for(auto v : test)
        printf("%d ", v);
    printf("\n");

    return 0;
}
