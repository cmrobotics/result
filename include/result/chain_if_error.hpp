#ifndef RESULT_CHAIN_IF_ERROR_HPP
#define RESULT_CHAIN_IF_ERROR_HPP

#include <string>

namespace result {

template<typename type>
auto chain_if_error(Result<type>& result, std::string& error_message) -> Result<type>{
    return std::visit(overload{
        [](type& data) {
            return static_cast<Result<type>>(data);
        },
        [&error_message](Error& parent_error)    {
            OptionalError optional_parent_error = std::make_shared<Error>(parent_error);
            return static_cast<Result<type>>(Error{ error_message, optional_parent_error});
        },
    }, result);
}

} // namespace result

#endif
