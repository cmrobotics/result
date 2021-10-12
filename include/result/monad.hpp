#ifndef RESULT_MONAD_HPP
#define RESULT_MONAD_HPP

namespace result {

template<typename from_type, typename to_type>
auto bind(Result<from_type>& result, std::function<Result<to_type>(from_type)>& safe_function) -> Result<to_type>{
    return std::visit(overload{
        [&safe_function](from_type& data) {
            Result<to_type> mapped_data = safe_function(data);
            return std::visit(overload{
                [](to_type& inner_data) {
                    return static_cast<Result<to_type>>(inner_data);
                },
                [](Error& inner_error)    {
                    return static_cast<Result<to_type>>(inner_error);
                },
            }, mapped_data);
        },
        [](Error& error)    {
            return static_cast<Result<to_type>>(error);
        },
    }, result);
}

template<typename from_type, typename to_type>
auto map(Result<from_type>& result, std::function<to_type(from_type)>& safe_function) -> Result<to_type>{
    std::function<Result<to_type>(from_type)> functor_equivalent = [&safe_function](from_type data) {
        return static_cast<Result<to_type>>(safe_function(data));
    };
    return bind(result, functor_equivalent);
}

} // namespace result
#endif
