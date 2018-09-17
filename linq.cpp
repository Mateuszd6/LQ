#include <functional>

#include <vector>

namespace detail
{
    enum struct query_node_type : unsigned short
    {
        NONE = 0,

        SELECT = 1,
        WHERE = 2,
        SORT = 3,
    };

    template<typename T, typename U>
    struct query_node
    {
        query_node_type node_type;
        std::function<U(T)> select_function;
        std::function<bool(T)> where_function;
        std::function<int(T, T)> sort_function;

        size_t get_return_type_size()
        {
            return sizeof(U);
        }

#if 0
        query_node()
        {
            this->node_type = query_node_type::NONE;
        }

        query_node(query_node const& other)
        {
            this->node_type = other.node_type;
            switch(other.node_type)
            {
                case query_node_type::SELECT:
                    this->select_function = other.select_function;
                    break;

                case query_node_type::WHERE:
                    this->where_function = other.where_function;
                    break;

                case query_node_type::SORT:
                    this->sort_function = other.sort_function;
                    break;

                default:
                    break;
            }
        }

        ~query_node() { }
#endif
    };

    template<typename T, typename U>
    struct query
    {
        T const* start;
        size_t len;

        std::vector<query_node<T, U>> _;

#if 1
        std::function<bool(T)> where_function;
        std::function<int(T, T)> sort_function;
#endif

        query(T const* start, size_t len);

        query(const query&) = delete;
        query& operator=(const query&) & = delete;

        query(query&&) = default;
        query& operator=(query&&) & = default;

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
        this->_.reserve(16);
    }

    template<typename T, typename U>
    query<T, U> query<T, U>::where(std::function<bool(T)> predicate)
    {
        auto result = std::move(*this);
        result.where_function = predicate;

        auto node = query_node<T, U> {};
        node.node_type = query_node_type::WHERE;
        node.where_function = predicate;

        result._.push_back(node);
        return result;
    }

    template<typename T, typename U>
    query<T, U> query<T, U>::sort(std::function<int(T, T)> compare)
    {
        auto result = std::move(*this);
        result.sort_function = compare;

        auto node = query_node<T, U> {};
        node.node_type = query_node_type::SORT;
        node.sort_function = compare;

        result._.push_back(node);
        return result;
    }

    template<typename T, typename U>
    ::std::vector<T> query<T, U>::to_vector()
    {
        auto begin = start;
        auto end = start + len;
        ::std::vector<T> result;
        result.reserve(end - begin);

#if 0
        for(auto i = begin; i != end; ++i)
        {
            if(where_function(*i))
                result.push_back(*i);
        }
#else
        size_t max_size = size_t { 0 };
        for(auto j : _)
        {
            max_size = std::max(max_size, j.get_return_type_size());
        }
        max_size = std::max(max_size, sizeof(T));
        printf("max size is %lu\n", max_size);

        unsigned char result_bytes[max_size];
        for(auto i = begin; i != end; ++i)
        {
            if(where_function(*i))
                result.push_back(*i);
        }
#endif

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

    auto test2 = from(int_ptr, 7)
        .where([](int x) { return x % 2 == 0 && x > 0; })
        .sort([](int x, int y) { return x >= y; });

    return 0;
}
