`Result` is a header-only minimalistic library for safe, correct and complete error handling in C++ 

This library guarantees the following properties:
- If a function can generate an error, when it is called there should be a compile-time guarantee that the error is checked.
- Errors shall not create side-effects and shall preserve safety and determinism of functions.
This implies exceptions are not allowed as mechanism. The library supports an easy integration for [functions that can throw exceptions](include/result/from_throwable.hpp).
- Errors and correct results shall be mutually exclusive. This implies using union types and therefore C++ [variant](https://en.cppreference.com/w/cpp/utility/variant)
- Results shall be monadic-composible. This implies that two `Result<T>` should be safely directly composable without any need for manual error checking. 
The library adds a [bind](include/result/monad.hpp) method to compose results. As it's a monad, a functor [map](include/result/monad.hpp) method is also added
to transform safely the `Result<T>` types without doing any manual error checking either.

## Usage

For usage see [unit tests](test/test.cpp)
We wrote unit tests so that they serve for a repository of examples as well as for functionality validation.

## Test

```shell
make build && make test

```
