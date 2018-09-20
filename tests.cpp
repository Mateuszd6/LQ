#include "lq.hpp"

namespace streaming {

    template <typename T>
    struct tag : std::reference_wrapper<T> {
        using std::reference_wrapper<T>::reference_wrapper;
    };

    using tag_ostream = tag<std::ostream>;

    template <typename T1, typename T2>
    static inline tag_ostream operator<<(tag_ostream os, std::pair<T1, T2> const& p) {
        os.get() << "{" << p.first << ", " << p.second  << "}";
        return os;
    }

    template <typename Other>
    static inline tag_ostream operator<<(tag_ostream os, Other const& o) {
        os.get() << o;
        return os;
    }
}
using namespace streaming;

using namespace lq;

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
    std::cout << "\nTest " << TEST_ID++ << ": ";
    print_with_message(msg, begin, end);
}

#include <string_view>

int main()
{
    {
        auto text = "MaTeUSz";
        print_test_msg("", text, text + 7);

        auto result = from(text, text + 7)
            .where([](auto x) { return isupper(x); })
            .sort([](auto x, auto y) { return x < y; })
            .to_vector();

        print_with_message("Upcase characters (sorted):",
                           result.begin(), result.end());
    }

    {
        auto numbers = std::array { 5, 4, 1, 3, 9, 8, 6, 7, 2, 0 };
        print_test_msg("", numbers.begin(), numbers.end());

        auto result = from(numbers.begin(), numbers.end())
            .where([](auto x) { return x < 5; })
            .skip(1)
            .to_vector();

        print_with_message("Numbers < 5 (without the first one): ",
                           result.begin(), result.end());
    }

    {
        auto numbers = std::array { 5, 4, 1, 3, 9, 8, 6, 7, 2, 0 };
        print_test_msg("", numbers.begin(), numbers.end());

        auto result = from(numbers.begin(), numbers.end())
            .where([](auto x) { return x < 5; })
            .skip(1)
            .sum();

        std::cout << "Sum of numbers < 5 (without the first one): " << result << "\n";
    }

    {
        auto numbers = std::vector { 5, 4, 1, 3, 9, 8, 6, 7, 2, 1, 2, 4, 0 };
        auto strings = std::vector<std::string_view> { "zero", "one", "two", "three",
                                                       "four", "five", "six", "seven",
                                                       "eight", "nine" };

        print_test_msg("", numbers.begin(), numbers.end());

        auto result = from(numbers.begin(), numbers.end())
            .select<std::string_view>([&strings](auto x) { return strings[x]; })
            .where([](auto) { return true; })
            .sort([](auto x, auto y) { return std::lexicographical_compare(x.begin(), x.end(),
                                                                           y.begin(), y.end()); })
            .where([](auto) { return true; })
            .unique()
            .to_vector();

        print_with_message("Numbers mapped to strings (sorted alphabetically and unique'd): ",
                           result.begin(), result.end());
    }

    {
        auto pairs = std::vector<std::pair<int, int>> { {1, 2}, {2, 0}, {1, 3}, {1, 4}, {1, -2} };
        print_test_msg("", pairs.begin(), pairs.end());

        auto result = from(pairs.begin(), pairs.end())
            .where([](auto x) { return x.first > 0; })
            .stable_sort([](auto x, auto y) { return x.first < y.first; })
            .to_vector();

        print_with_message("Stable sorted pairs with 1 as first: ",
                           result.begin(), result.end());
    }

    {
        auto words = std::vector<std::string> { "believe", "relief", "receipt", "field" };
        print_test_msg("", words.begin(), words.end());

        auto result = from(words.begin(), words.end())
            .where([](auto x) { return x.find("ei") != std::string::npos; })
            .any();

        std::cout << "There is a word that contains in the list that contains 'ei': "
                  << (result ? "true " : "false")
                  << "\n";
    }

    {
        auto words = std::vector<std::string> { "believe", "relief", "field" };
        print_test_msg("", words.begin(), words.end());

        auto result = from(words.begin(), words.end())
            .where([](auto x) { return x.find("ei") != std::string::npos; })
            .any();

        std::cout << "There is a word that contains in the list that contains 'ei': "
                  << (result ? "true " : "false") << "\n";
    }

    {
        auto numbers = std::array { 5, 4, 1, 3, 9, 8, 6, 7, 2, 0 };
        print_test_msg("", numbers.begin(), numbers.end());

        std::cout << "All numbers x4, hex formatted: ";
        from(numbers.begin(), numbers.end())
            .select<int>([](auto x) { return 4 * x; })
            .for_each([](auto x) { std::cout << std::hex << x << std::dec << ' '; });
        std::cout << '\n';
    }

    return 0;
}
