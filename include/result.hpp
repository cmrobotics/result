#ifndef RESULT_HPP
#define RESULT_HPP

#include <functional>
#include <memory>
#include <variant>

namespace result {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

struct Error;
struct None {};
// std::shared_ptr used to make recursive types variant-compatible
// All Error have non-null `optional_parent_error` or None type
// There will **never** be a nullptr `std::shared_ptr<Error>`
using OptionalError = std::variant<None, std::shared_ptr<Error>>;
struct Error {
    std::string message;
    OptionalError optional_parent_error = None{};
};

template<typename result_type>
using Result = std::variant<result_type, Error>;

} // namespace result

#include "result/monad.hpp"
#include "result/from_throwable.hpp"
#include "result/chain_if_error.hpp"
#include "result/version.hpp"

#endif
