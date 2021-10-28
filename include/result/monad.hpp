#ifndef RESULT_MONAD_HPP
#define RESULT_MONAD_HPP

namespace result {

template<typename from_type, typename to_type, typename error_type>
auto bind(GenericResult<from_type, error_type>& result, std::function<GenericResult<to_type, error_type>(from_type)>& safe_function) -> GenericResult<to_type, error_type>{
    return std::visit(overload{
        [&safe_function](from_type& data) {
            return static_cast<GenericResult<to_type, error_type>>(safe_function(data));
        },
        [](error_type& error)    {
            return static_cast<GenericResult<to_type, error_type>>(error);
        },
    }, result);
}

template<typename from_type, typename to_type, typename error_type>
auto map(GenericResult<from_type, error_type>& result, std::function<to_type(from_type)>& safe_function) -> GenericResult<to_type, error_type>{
    std::function<GenericResult<to_type, error_type>(from_type)> functor_equivalent = [&safe_function](from_type data) {
        return static_cast<GenericResult<to_type, error_type>>(safe_function(data));
    };
    return bind(result, functor_equivalent);
}

template<typename from_type, typename error_type, typename to_error_type>
auto bind_error(GenericResult<from_type, error_type>& result, std::function<GenericResult<from_type, to_error_type>(error_type)>& safe_function) -> GenericResult<from_type, to_error_type>{
    return std::visit(overload{
        [](from_type& data) {
            return static_cast<GenericResult<from_type, to_error_type>>(data);
        },
        [&safe_function](error_type& error)    {
            return static_cast<GenericResult<from_type, to_error_type>>(safe_function(error));            
        },
    }, result);
}

template<typename from_type, typename error_type, typename to_error_type>
auto map_error(GenericResult<from_type, error_type>& result, std::function<to_error_type(error_type)>& safe_function) -> GenericResult<from_type, to_error_type>{
    std::function<GenericResult<from_type, to_error_type>(error_type)> functor_equivalent = [&safe_function](error_type data) {
        return static_cast<GenericResult<from_type, to_error_type>>(safe_function(data));
    };
    return bind_error(result, functor_equivalent);
}

} // namespace result
#endif
