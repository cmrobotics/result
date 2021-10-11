#ifndef RESULT_FROM_THROWABLE_HPP
#define RESULT_FROM_THROWABLE_HPP
    
#include <string>

namespace result {

template<typename to_type> 
auto from_throwable(const std::function<to_type()>&  unsafe_function) -> Result<to_type>{
    try {
        return unsafe_function();
    }
    catch (const std::exception &e) { 
        std::string error_message = e.what();
        return Error{ error_message };
    }
    catch (const std::string    &e) { 
        return static_cast<Result<to_type>>(Error{ e });
    }
    catch (const char           *e) { 
        return static_cast<Result<to_type>>(Error{ std::string(e) }); 
    }
    catch (...) { 
        return static_cast<Result<to_type>>(Error{ std::string("Unknown Exception") }); 
    }
}

} // namespace result

#endif
