#ifndef RESULT_HPP
#define RESULT_HPP

#include <functional>
#include <memory>
#include <variant>

namespace result {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };

struct Error;
struct None {};
using OptionalError = std::variant<None, std::shared_ptr<Error>>;
struct Error { 
    std::string message; 
    OptionalError optional_parent_error = None{};
};

template<typename result_type>
using Result = std::variant<result_type, Error>;

} // namespace result

#include "result/bind.hpp"
#include "result/from_throwable.hpp"
#include "result/version.hpp"

#endif
