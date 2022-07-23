# Lazy Commit-Revert
Build a data structure, modify it, commit changes, modify it more, revert uncommitted changes.
This is a design idea useful for some kinds of local search, among other things.
Here a well-optimized templated header-only implementation is given, with some usage examples.

# Example

```cpp
#include <iostream>
#include <vector>

// include the header
#include "lcr.h"
// for convenience
using namespace lcrx;

int main() {
  // create lcr ints, using global handler
  lcr<int> i(2);
  std::vector<lcr<int>> vec({1,2,3,4});

  // access underlying values using operator() or get()
  std::cout << i() << std::endl;
  std::cout << vec[0]() << std::endl;

  // change some values
  vec[2]() = 6; // vec = {1,2,6,4}
  vec[3]() = 2; // vec = {1,2,6,2}

  // commit the changes
  lcr_global_handler.commit();

  // make some more changes
  i() = 3; // i = 3
  vec[2]() = 18; // vec = {1,2,18,2}
  vec[1]() = -6; // vec = {1,-6,18,2}

  // revert the new changes
  lcr_global_handler.revert();
  // the following things are now (lazily) true:
  // i = 2
  // vec = {1,2,6,2}
  // it's important to access through the accessors () every time, since it propogates the lazy evaluation:
  std::cout << "i=" << i() << std::endl;
}
```

# Complexity

Everything is `O(1)`!
