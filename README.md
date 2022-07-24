# Lazy Commit-Revert
Build a data structure, modify it, commit changes, modify it more, revert uncommitted changes.
This is a design idea useful for some kinds of local search, among other things.
Here a well-optimized templated header-only implementation is given, with some usage examples.

# Explanation

Everything is inside a single header `lcr.h`,
under the namespace `lcrx`.  
There are two primary classes: `lcr_handler`, and `lcr<T>`.

### `lcr<T>`

A wrapper for a `T`,
along with an associated `lcr_handler`.
To access the internal data,
the `operator()` function (or `get()`) should be used every time
to obtain a `T&`.
This will propogate any changes made by the handler.
The initialization can either take a handler,
or by default use the global handler.
If the handler needs to be changed later,
`assign(lcr_handler&)` can be used.

### `lcr_handler`

The handler has two primary operations:
- `commit()`. All `lcr<T>` associated with the handler will be copied into a backup (lazily).
- `revert()`. All `lcr<T>` associated with the handler will be restored from backup (lazily).

There is also `lcr_global_handler`,
a static handler which serves as a default handler
for all `lcr<T>` wrappers.

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


  // create a custom handler
  lcr_handler local_handler;
  // assign the handler to i
  i.assign(local_handler);

  // i is now handled by the local handler, while each of the items in vec are handled by the global handler
  // so the global handler has no effect on i
  i() = 4;
  global_handler.revert();
  // i = 4 still
  std::cout << "i=" << i() << std::endl;
  local_handler.revert();
  // now i = 2 again
  std::cout << "i=" << i() << std::endl;
}
```

# Complexity

Everything is `O(1)`!

# Usage

To use in your own project, simply include `lcr.h`.
To run the tests, build with `./build.sh` and execute with `build/lcr_test`.
