## Introduction

`Result` is a header-only minimalistic library for safe, correct and complete error handling in C++

This library guarantees the following properties:
- When a function that can generate an error is called, there should be a compile-time guarantee that the error is checked.
- Errors shall not create side-effects, and shall preserve safety and determinism.
This implies exceptions are not allowed as mechanism. The library supports an easy integration for [functions that can throw exceptions](include/result/from_throwable.hpp).
- Errors and correct results shall be mutually exclusive. This implies using union types and therefore C++ [variant](https://en.cppreference.com/w/cpp/utility/variant)
- Results shall be monadic-composible. This implies that two `Result<T>` should be safely directly composable without any need for manual error checking.
The library adds a [bind](include/result/monad.hpp) method to compose results. As it's a monad, a functor [map](include/result/monad.hpp) method is also added
to transform safely the `Result<T>` types without doing any manual error checking either.
- Errors shall be chainable, creating something analogous to a stack-trace of errors. There is also a a mechanism to chain errors without manually
checking for them using [chain_if_error](include/result/chain_if_error.hpp).

## Add library to your project

### CMake Projects

First add to `CMakeLists.txt` the following `find_package` entry:
```
find_package(result CONFIG REQUIRED)
```

Finally, add to `CMakeLists.txt` the library in the included directories:
```
include_directories(include
  # Other include dirs go here
  ${result_INCLUDE_DIRS}
)
```

### ROS2 Projects
First follow the previous section for `CMake Projects` to get the `CMakeLists.txt` ready.

The next step is adding the dependency in `package.xml`:
```xml
<build>result</build>
```

Clone the result repository in your workspace `/src` folder
```bash
cd $WORKSPACE/src && git clone https://github.com/cmrobotics/result.git
```

Finally, add the include in your C++ sources:
```c++
#include <result.hpp>
```

## Usage

For usage see [unit tests](test/test.cpp).

We wrote unit tests so that they serve for a repository of examples as well as for functionality validation.

## Test

```shell
make && make test
```
