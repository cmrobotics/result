`Result` is a header-only library for safe, correct and complete error handling in C++ 

This library guarantees the following properties:
- If a function can generate an error, when it is called there should be a compile-time guarantee that the error is checked.
- Errors shall not create side-effects and shall preserve safety and determinism of functions.
This implies exceptions are not allowed as mechanism. The library supports an easy integration for [functions that can throw exceptions](include/result/from_throwable.hpp).
- Errors and correct results shall be mutually exclusive. This implies using union types and therefore C++ [variant](https://en.cppreference.com/w/cpp/utility/variant)
- Results shall be monadic-composible. The library adds a [bind](include/result/bind.hpp) method to compose results. As monadic behaviour is supported,
it is also added the [map](include/result/bind.hpp) method to support its role as functor.

## Usage

For usage see [unit tests](test/test.cpp)

## Test

```shell
make build && make test

```
