## LQ

# Some linq-like query library for c++, for practice. Work in progres.

```
auto pairs = std::vector<std::pair<int, int>>
{
    {1, 2},
    {2, 0},
    {1, 3},
    {1, 4},
    {1, -2}
};

auto result = from(pairs)
    .where([](auto x) { return x.first > 0; })
    .stable_sort([](auto x, auto y) { return x.first < y.first; })
    .to_vector();
```