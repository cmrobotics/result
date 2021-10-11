`Result` is a header-only library for safe, correct and complete error handling in C++ 

This library guarantees the following properties:
- If a function that generates errors is called, there should be guaranteed at compile-time that both the errors and the correct results are checked
- Errors shall not create side-effects and preserve safety and determinism of functions.
This implies exceptions are not allowed as mechanism. The library does support easy integration of usafe functions that can 
throw exceptions to the library is a safe way.
 - Errors and correct results shall be mutually exclusive. This implies using union types and therefore C++ `variant`  
 - Results shall be monadic-composible. The library adds a `bind` method to compose results. As monadic behaviour is supported,
 it is also added the `map` method to support its role as functor.

## Usage

```cpp
#include <include/result.hpp>

using namespace cmr.result;

// exclaim a string
std::string value = exclaim("hello");
std::clog << value; // => hello!
```

## Test

```shell
# build test binaries
make

# run tests
make test

# run bench tests
make bench
```

The default test binaries will be built in release mode. You can make Debug test binaries as well:

```shell
make clean
make debug
make test
```

Enable additional sanitizers to catch hard-to-find bugs, for example:

```shell
export LDFLAGS="-fsanitize=address,undefined,integer"
export CXXFLAGS="-fsanitize=address,undefined,integer"

make
```
