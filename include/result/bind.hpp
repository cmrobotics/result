#ifndef RESULT_BIND_HPP
#define RESULT_BIND_HPP

namespace result {

template<typename from_type, typename to_type>
auto bind(Result<from_type>& result, const std::function<Result<to_type>(from_type)>& safe_function) -> Result<to_type>{
    return std::visit(overload{
        [&safe_function](from_type& data) {
            Result<to_type> mapped_data = safe_function(data);
            return std::visit(overload{
                [](to_type& data) {
                    return static_cast<Result<to_type>>(data);
                },
                [](Error& error)    { 
                    return static_cast<Result<to_type>>(error);
                },
            }, mapped_data);
        },
        [](Error& error)    { 
            return static_cast<Result<to_type>>(error);
        },
    }, result);
}

} // namespace result
#endif
