# sort2
Sort two ranges at same time, according the order of the first range.

# For example
```
vector a = { 3, 2, 1};
vector b = { x, y, z};
vector c = { 3, 2, 1};
sort::sort(c.begin(), c.end()); // c -> { 1, 2, 3, }
sort::sort2(a.begin(), a.end(), b.begin()); // a -> { 1, 2, 3}    b-> { z, y, x}
```

# Implementation note
this implementation is from modifing the implementation Visual Studio 2017.
So there is copy-right risk!
This project is motivated by interest.
