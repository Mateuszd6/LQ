#include <functional>
#include <iostream>

#include <list>
#include <vector>

#define CONVERSION_FUNCIONS(RET_TYPE)                                                              \
    std::vector<RET_TYPE> to_vector()                                                              \
    {                                                                                              \
        auto result = std::vector<RET_TYPE>();                                                     \
        while (1)                                                                                  \
            {                                                                                      \
                auto ret = give_next();                                                            \
                                                                                                   \
                if (ret.state == return_state::END)                                                \
                    break;                                                                         \
                else if (ret.state == return_state::VALID)                                         \
                    result.push_back(ret.value);                                                   \
            }                                                                                      \
                                                                                                   \
        return result;                                                                             \
    }                                                                                              \
                                                                                                   \
    std::list<RET_TYPE> to_list()                                                                  \
    {                                                                                              \
        auto result = std::list<RET_TYPE>();                                                       \
        while (1)                                                                                  \
            {                                                                                      \
                auto ret = give_next();                                                            \
                                                                                                   \
                if (ret.state == return_state::END)                                                \
                    break;                                                                         \
                else if (ret.state == return_state::VALID)                                         \
                    result.push_back(ret.value);                                                   \
            }                                                                                      \
                                                                                                   \
        return result;                                                                             \
    }                                                                                              \
                                                                                                   \
    bool any()                                                                                     \
    {                                                                                              \
        auto ret = give_next();                                                                    \
        if (ret.state == return_state::END)                                                        \
            return false;                                                                          \
        else                                                                                       \
            return true;                                                                           \
    }                                                                                              \
                                                                                                   \
    template <typename FOLD_LEFT_RESULT__T>                                                        \
    FOLD_LEFT_RESULT__T fold_left(std::function<FOLD_LEFT_RESULT__T(FOLD_LEFT_RESULT__T, T)> func, \
                                  FOLD_LEFT_RESULT__T start_value)                                 \
    {                                                                                              \
        auto result = start_value;                                                                 \
        while (1)                                                                                  \
            {                                                                                      \
                auto ret = give_next();                                                            \
                                                                                                   \
                if (ret.state == return_state::END)                                                \
                    return result;                                                                 \
                else if (ret.state == return_state::VALID)                                         \
                    result = func(result, ret.value);                                              \
            }                                                                                      \
    }                                                                                              \
                                                                                                   \
    T sum()                                                                                        \
    {                                                                                              \
        return fold_left<T>([](auto x, auto y) { return x + y; }, 0);                              \
    }                                                                                              \
                                                                                                   \
    T multiply()                                                                                   \
    {                                                                                              \
        return fold_left<T>([](auto x, auto y) { return x * y; }, 1);                              \
    }                                                                                              \
                                                                                                   \
    template <typename FUNC>                                                                       \
    void for_each(FUNC func)                                                                       \
    {                                                                                              \
        while (1)                                                                                  \
            {                                                                                      \
                auto ret = give_next();                                                            \
                                                                                                   \
                if (ret.state == return_state::END)                                                \
                    return;                                                                        \
                else if (ret.state == return_state::VALID)                                         \
                    func(ret.value);                                                               \
            }                                                                                      \
    }

#define LINQ_FUCTIONS(CURR_TYPE, TO_TYPE, PREVIOUS_TYPE)                                        \
    template <typename _U>                                                                      \
    select_t<TO_TYPE, _U, PREVIOUS_TYPE> select(std::function<_U(CURR_TYPE)> c_select_function) \
    {                                                                                           \
        return select_t<TO_TYPE, _U, PREVIOUS_TYPE>(c_select_function, *this);                  \
    }                                                                                           \
                                                                                                \
    where_t<CURR_TYPE, PREVIOUS_TYPE> where(std::function<bool(CURR_TYPE)> c_where_function)    \
    {                                                                                           \
        return where_t<CURR_TYPE, PREVIOUS_TYPE>(c_where_function, *this);                      \
    }                                                                                           \
                                                                                                \
    take_t<CURR_TYPE, PREVIOUS_TYPE> take(size_t amount)                                        \
    {                                                                                           \
        return take_t<CURR_TYPE, PREVIOUS_TYPE>(amount, *this);                                 \
    }                                                                                           \
                                                                                                \
    skip_t<CURR_TYPE, PREVIOUS_TYPE> skip(size_t amount)                                        \
    {                                                                                           \
        return skip_t<CURR_TYPE, PREVIOUS_TYPE>(amount, *this);                                 \
    }

namespace lq
{
    namespace detail
    {
        enum struct return_state : unsigned char
        {
            VALID,
            END,
        };

        enum sort_method : unsigned char
        {
            DEFAULT,
            STABLE,
            UNIQUE,
        };

        template <typename T>
        struct return_element
        {
            return_state state;
            T value;

            return_element() {}

            return_element(return_state c_state) { this->state = c_state; }

            return_element(return_state c_state, T c_value)
            {
                this->state = c_state;
                this->value = c_value;
            }
        };


        template <typename T>
        struct from_t;
        template <typename T, typename PREV>
        struct where_t;
        template <typename T, typename PREV>
        struct sort_t;
        template <typename T, typename PREV>
        struct take_t;
        template <typename T, typename PREV>
        struct skip_t;
        template <typename T, typename U, typename PREV>
        struct select_t;

        template <typename ITER>
        struct from_t
        {
            using T = typename std::iterator_traits<ITER>::value_type;

            ITER begin;
            ITER end;
            ITER curr;

            from_t() {}
            from_t(ITER begin_iterator, ITER end_iterator)
            {
                begin = begin_iterator;
                curr = begin_iterator;
                end = end_iterator;
            }

            return_element<T> give_next()
            {
                if (curr == end)
                    return return_element<T>(return_state::END);
                else
                    {
                        auto res = *curr;
                        curr++;
                        return return_element<T>(return_state::VALID, res);
                    }
            }

            template <typename SELECT_TYPE>
            select_t<T, SELECT_TYPE, from_t<ITER>> select(
                std::function<SELECT_TYPE(T)> c_select_function)
            {
                return select_t<T, SELECT_TYPE, from_t<ITER>>(c_select_function, *this);
            }
            where_t<T, from_t<ITER>> where(std::function<bool(T)> c_where_function)
            {
                return where_t<T, from_t<ITER>>(c_where_function, *this);
            }
            take_t<T, from_t<ITER>> take(size_t c_take_amount)
            {
                return take_t<T, from_t<ITER>>(c_take_amount, *this);
            }
            skip_t<T, from_t<ITER>> skip(size_t c_take_amount)
            {
                return skip_t<T, from_t<ITER>>(c_take_amount, *this);
            }
            sort_t<T, from_t<ITER>> sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, from_t<ITER>>(c_sort_function, sort_method::DEFAULT, *this);
            }
            sort_t<T, from_t<ITER>> stable_sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, from_t<ITER>>(c_sort_function, sort_method::STABLE, *this);
            }
            sort_t<T, from_t<ITER>> unique() { return sort_t<T, from_t<ITER>>(*this); }

            CONVERSION_FUNCIONS(T);
        };

        template <typename T, typename PREV>
        struct sort_t
        {
            PREV prev;

            sort_method sort_style;
            bool dirty;
            std::function<int(T, T)> compare_function;
            std::vector<T> cache_vector;
            typename std::vector<T>::iterator curr;

            sort_t() {}
            sort_t(PREV c_prev)
            {
                prev = c_prev;
                dirty = false;
                sort_style = sort_method::UNIQUE;
            }
            sort_t(std::function<int(T, T)> c_compare_function, sort_method c_sort_method,
                   PREV c_prev)
            {
                prev = c_prev;
                dirty = false;
                sort_style = c_sort_method;
                compare_function = c_compare_function;
            }

            return_element<T> give_next()
            {
                // Initialize.
                if (!dirty)
                    {
                        cache_vector.clear();
                        while (1)
                            {
                                auto ret = prev.give_next();
                                if (ret.state == return_state::END)
                                    break;
                                else
                                    cache_vector.emplace_back(ret.value);
                            }

                        switch (sort_style)
                            {
                                case sort_method::DEFAULT:
                                    std::sort(cache_vector.begin(), cache_vector.end(),
                                              compare_function);
                                    break;

                                case sort_method::STABLE:
                                    std::stable_sort(cache_vector.begin(), cache_vector.end(),
                                                     compare_function);
                                    break;

                                case sort_method::UNIQUE:
                                    auto last =
                                        std::unique(cache_vector.begin(), cache_vector.end());
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

            template <typename SELECT_TYPE>
            select_t<T, SELECT_TYPE, sort_t<T, PREV>> select(
                std::function<SELECT_TYPE(T)> c_select_function)
            {
                return select_t<T, SELECT_TYPE, sort_t<T, PREV>>(c_select_function, *this);
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
            sort_t<T, sort_t<T, PREV>> sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, sort_t<T, PREV>>(c_sort_function, sort_method::DEFAULT, *this);
            }
            sort_t<T, sort_t<T, PREV>> stable_sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, sort_t<T, PREV>>(c_sort_function, sort_method::STABLE, *this);
            }
            sort_t<T, sort_t<T, PREV>> unique() { return sort_t<T, sort_t<T, PREV>>(*this); }

            CONVERSION_FUNCIONS(T);
        };

        template <typename T, typename PREV>
        struct where_t
        {
            PREV prev;
            std::function<bool(T)> where_function;

            where_t() {}
            where_t(std::function<bool(T)> c_where_function, PREV c_prev)
            {
                prev = c_prev;
                where_function = c_where_function;
            }

            return_element<T> give_next()
            {
                return_element<T> ret;

                while (1)
                    {
                        ret = prev.give_next();
                        if (ret.state == return_state::END)
                            return ret;
                        else if (ret.state == return_state::VALID && where_function(ret.value))
                            return ret;
                    }
            }

            template <typename SELECT_TYPE>
            select_t<T, SELECT_TYPE, where_t<T, PREV>> select(
                std::function<SELECT_TYPE(T)> c_select_function)
            {
                return select_t<T, SELECT_TYPE, where_t<T, PREV>>(c_select_function, *this);
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
            sort_t<T, where_t<T, PREV>> unique() { return sort_t<T, where_t<T, PREV>>(*this); }

            CONVERSION_FUNCIONS(T);
        };

        template <typename T, typename PREV>
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

                if (ret.state == return_state::END || took > take_amount)
                    return return_element<T>(return_state::END);
                else if (ret.state == return_state::VALID)
                    return ret;

                else
                    return ret;
            }

            template <typename SELECT_TYPE>
            select_t<T, SELECT_TYPE, take_t<T, PREV>> select(
                std::function<SELECT_TYPE(T)> c_select_function)
            {
                return select_t<T, SELECT_TYPE, take_t<T, PREV>>(c_select_function, *this);
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
            sort_t<T, take_t<T, PREV>> sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, take_t<T, PREV>>(c_sort_function, sort_method::DEFAULT, *this);
            }
            sort_t<T, take_t<T, PREV>> stable_sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, take_t<T, PREV>>(c_sort_function, sort_method::STABLE, *this);
            }
            sort_t<T, take_t<T, PREV>> unique() { return sort_t<T, take_t<T, PREV>>(*this); }

            CONVERSION_FUNCIONS(T);
        };

        template <typename T, typename PREV>
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
                while (skipped_already < skip_amount)
                    {
                        ret = prev.give_next();
                        skipped_already++;

                        if (ret.state == return_state::END)
                            return ret;
                    }

                ret = prev.give_next();
                if (ret.state == return_state::END)
                    return ret;
                else if (ret.state == return_state::VALID)
                    return ret;

                else
                    return ret;
            }

            template <typename SELECT_TYPE>
            select_t<T, SELECT_TYPE, skip_t<T, PREV>> select(
                std::function<SELECT_TYPE(T)> c_select_function)
            {
                return select_t<T, SELECT_TYPE, skip_t<T, PREV>>(c_select_function, *this);
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
            sort_t<T, skip_t<T, PREV>> sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, skip_t<T, PREV>>(c_sort_function, sort_method::DEFAULT, *this);
            }
            sort_t<T, skip_t<T, PREV>> stable_sort(std::function<int(T, T)> c_sort_function)
            {
                return sort_t<T, skip_t<T, PREV>>(c_sort_function, sort_method::STABLE, *this);
            }
            sort_t<T, skip_t<T, PREV>> unique() { return sort_t<T, skip_t<T, PREV>>(*this); }

            CONVERSION_FUNCIONS(T);
        };

        template <typename T, typename U, typename PREV>
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
                while (1)
                    {
                        ret = prev.give_next();

                        if (ret.state == return_state::END)
                            return return_element<U>(return_state::END);
                        else if (ret.state == return_state::VALID)
                            return return_element<U>(return_state::VALID,
                                                     select_function(ret.value));
                    }
            }

            template <typename SELECT_TYPE>
            select_t<U, SELECT_TYPE, select_t<T, U, PREV>> select(
                std::function<SELECT_TYPE(T)> c_select_function)
            {
                return select_t<U, SELECT_TYPE, select_t<T, U, PREV>>(c_select_function, *this);
            }
            where_t<U, select_t<T, U, PREV>> where(std::function<bool(U)> c_where_function)
            {
                return where_t<U, select_t<T, U, PREV>>(c_where_function, *this);
            }
            take_t<U, select_t<T, U, PREV>> take(size_t c_take_amount)
            {
                return take_t<U, select_t<T, U, PREV>>(c_take_amount, *this);
            }
            skip_t<U, select_t<T, U, PREV>> skip(size_t c_take_amount)
            {
                return skip_t<U, select_t<T, U, PREV>>(c_take_amount, *this);
            }
            sort_t<U, select_t<T, U, PREV>> sort(std::function<int(U, U)> c_sort_function)
            {
                return sort_t<U, select_t<T, U, PREV>>(c_sort_function, sort_method::DEFAULT,
                                                       *this);
            }
            sort_t<U, select_t<T, U, PREV>> stable_sort(std::function<int(U, U)> c_sort_function)
            {
                return sort_t<U, select_t<T, U, PREV>>(c_sort_function, sort_method::STABLE, *this);
            }
            sort_t<U, select_t<T, U, PREV>> unique()
            {
                return sort_t<U, select_t<T, U, PREV>>(*this);
            }

            CONVERSION_FUNCIONS(U);
        };
    }  // namespace detail
}  // namespace lq

namespace lq
{
    template <typename ITERATOR>
    detail::from_t<ITERATOR> from(ITERATOR begin, ITERATOR end)
    {
        return detail::from_t<ITERATOR>(begin, end);
    }
}  // namespace lq
