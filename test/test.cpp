#include <result.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>


TEST_CASE("test correct result")
{
    using namespace result;
    Result<uint8_t> age_result = 32;
    bool is_correct = std::visit(overload{
        [](uint8_t& age)       { return age == 32; },
        [](Error&)   { return false; },
    }, age_result);
    REQUIRE(is_correct);
}

TEST_CASE("test error result")
{
    using namespace result;
    std::string error_message = "Error getting age from Database";
    Result<uint8_t> age_result = Error{ error_message };
    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, age_result);
    REQUIRE(is_correct);
}

struct Employee {
    uint32_t employee_id;
    std::string name;
    uint8_t age;
};

struct Contract {
    uint32_t contract_id;
    std::string contents;
    uint32_t employee_id;
};


TEST_CASE("compose successful results")
{
    using namespace result;
    uint32_t employee_id = 2343423;
    Result<Employee> db_employee = Employee{ employee_id, "Joan", 44 };
    Result<Contract> db_contract = Contract{ 623523, "This contract...", employee_id };
    std::function<Result<Contract>(Employee)> find_contract_by_employee_id = [&db_contract](Employee) { return db_contract; };
    Result<Contract> contract_from_employee = bind(db_employee, find_contract_by_employee_id);

    bool is_correct = std::visit(overload{
        [&employee_id](Contract& contract)       { return contract.employee_id == employee_id; },
        [](Error&)   { return false; },
    }, contract_from_employee);
    REQUIRE(is_correct);
}

TEST_CASE("compose results with first error")
{
    using namespace result;
    std::string error_message = "Employee not found";
    Result<Employee> db_employee = Error{ error_message };
    Result<Contract> db_contract = Contract{ 623523, "This contract...", 4523454 };
    std::function<Result<Contract>(Employee)> find_contract_by_employee_id = [&db_contract](Employee) { return db_contract; };
    Result<Contract> contract_from_employee = bind(db_employee, find_contract_by_employee_id);

    bool is_correct = std::visit(overload{
        [](Contract)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, contract_from_employee);
    REQUIRE(is_correct);
}

TEST_CASE("compose results with second error")
{
    using namespace result;
    Result<Employee> db_employee = Employee{ 2343423, "Joan", 44 };
    std::string error_message = "No contract found for employee";
    Result<Contract> db_contract = Error{ error_message };
    std::function<Result<Contract>(Employee)> find_contract_by_employee_id = [&db_contract](Employee) { return db_contract; };
    Result<Contract> contract_from_employee = bind(db_employee, find_contract_by_employee_id);

    bool is_correct = std::visit(overload{
        [](Contract)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, contract_from_employee);
    REQUIRE(is_correct);
}

TEST_CASE("map from correct result")
{
    using namespace result;
    std::string name("Joan");
    std::string error_message = "Database server not reacheable";
    Result<Employee> db_employee = Error{ error_message };
    std::function<std::string(Employee)> get_name = [](Employee employee) { return employee.name; };
    Result<std::string> name_of_employee = map(db_employee, get_name);

    bool is_correct = std::visit(overload{
        [](std::string)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, name_of_employee);
    REQUIRE(is_correct);
}

TEST_CASE("map from error")
{
    using namespace result;
    std::string name("Joan");
    Result<Employee> db_employee = Employee{ 2343423, name, 44 };
    std::function<std::string(Employee)> get_name = [](Employee employee) { return employee.name; };
    Result<std::string> name_of_employee = map(db_employee, get_name);

    bool is_correct = std::visit(overload{
        [&name](std::string name_result)       { return name_result == name; },
        [](Error)   { return false; },
    }, name_of_employee);
    REQUIRE(is_correct);
}

TEST_CASE("chain error")
{
    using namespace result;
    Error parent_error { "Database server not reacheable" };
    Result<Employee> db_employee = parent_error;
    std::function<std::string(Employee)> get_name = [](Employee employee) { return employee.name; };
    Result<std::string> name_of_employee = map(db_employee, get_name);
    std::string error_message = std::string("Employee can't be found due to technical problems. Contact your system administrator");
    Result<std::string> name_of_employee_processed = chain_if_error(name_of_employee, error_message);

    bool is_correct = std::visit(overload{
        [](std::string)       { return false; },
        [&parent_error, &error_message](Error& error)   {
            return error.message == error_message && std::visit(overload{
                [](None&)       { return false; },
                [&parent_error](std::shared_ptr<Error>& inner_error) { return inner_error->message == parent_error.message; },
            }, error.optional_parent_error);
        },
    }, name_of_employee_processed);
    REQUIRE(is_correct);
}

TEST_CASE("chain error from correct result")
{
    using namespace result;
    std::string name("Joan");
    Error parent_error { "Database server not reacheable" };
    Result<Employee> db_employee = Employee{ 2343423, name, 44 };
    std::function<std::string(Employee)> get_name = [](Employee employee) { return employee.name; };
    Result<std::string> name_of_employee = map(db_employee, get_name);
    std::string error_message = std::string("Employee can't be found due to technical problems. Contact your system administrator");
    Result<std::string> name_of_employee_processed = chain_if_error(name_of_employee, error_message);

    bool is_correct = std::visit(overload{
        [&name](std::string& retrieved_name)       { return retrieved_name == name ; },
        [&](Error&)   { return false; },
    }, name_of_employee_processed);
    REQUIRE(is_correct);
}

TEST_CASE("unsuccessful unsafe error using std::exception")
{
    using namespace result;
    std::string error_message = "Not possible to open database connection";
    auto expected_error = std::runtime_error(error_message);
    std::function<uint8_t()> get_age_unsafe_and_successful = [&expected_error]() {
        throw expected_error;
        // this line will never be executed
        return 32;
    };
    Result<uint8_t> age_result = from_throwable<uint8_t>(get_age_unsafe_and_successful);

    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, age_result);
    REQUIRE(is_correct);
}

TEST_CASE("unsuccessful unsafe error using std::string")
{
    using namespace result;
    std::string error_message = "Not possible to open database connection";
    std::function<uint8_t()> get_age_unsafe_and_successful = [&error_message]() {
        throw error_message;
        // this line will never be executed
        return 32;
    };
    Result<uint8_t> age_result = from_throwable<uint8_t>(get_age_unsafe_and_successful);

    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, age_result);
    REQUIRE(is_correct);
}

TEST_CASE("unsuccessful unsafe error using char*")
{
    using namespace result;
    std::string error_message = "Not possible to open database connection";
    std::function<uint8_t()> get_age_unsafe_and_successful = [&error_message]() {
        throw error_message.c_str();
        // this line will never be executed
        return 32;
    };
    Result<uint8_t> age_result = from_throwable<uint8_t>(get_age_unsafe_and_successful);

    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, age_result);
    REQUIRE(is_correct);
}

TEST_CASE("unsuccessful unsafe error using any type")
{
    using namespace result;
    uint16_t error_code = 57;
    std::function<uint8_t()> get_age_unsafe_and_successful = [&error_code]() {
        throw error_code;
        // this line will never be executed
        return 32;
    };
    Result<uint8_t> age_result = from_throwable<uint8_t>(get_age_unsafe_and_successful);

    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [](Error& error)   { return error.message == std::string("Unknown Exception"); },
    }, age_result);
    REQUIRE(is_correct);
}

TEST_CASE("successful unsafe correct result")
{
    using namespace result;
    std::function<uint8_t()> get_age_unsafe_and_successful = []() { return 32; };
    Result<uint8_t> age_result = from_throwable<uint8_t>(get_age_unsafe_and_successful);

    bool is_correct = std::visit(overload{
        [](uint8_t& age)       { return age == 32; },
        [](Error&)   { return false; },
    }, age_result);
    REQUIRE(is_correct);
}


TEST_CASE("correct generic result")
{
    using namespace result;
    GenericResult<uint8_t, std::string> age_result = 32;
    bool is_correct = std::visit(overload{
        [](uint8_t& age)       { return age == 32; },
        [](std::string&)   { return false; },
    }, age_result);
    REQUIRE(is_correct);
}



TEST_CASE("incorrect generic result")
{
    using namespace result;
    std::string error_message = std::string("this is an error");
    GenericResult<uint8_t, std::string> error_result = error_message;
    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [&error_message](std::string& error)   { return  error == error_message; },
    }, error_result);
    REQUIRE(is_correct);
}

TEST_CASE("correct generic result map")
{
    using namespace result;
    using ErrorMessage = std::string;
    struct Age {
        uint8_t value;
    };
    GenericResult<uint8_t, ErrorMessage> age_result = 32;
    std::function<Age(uint8_t)> safe_function = [](uint8_t age) { return Age{age}; };
    GenericResult<Age, ErrorMessage> age_string_result = map(age_result, safe_function);
    bool is_correct = std::visit(overload{
        [](Age& age)       { return age.value == 32; },
        [](ErrorMessage&)   { return false; },
    }, age_string_result);
    REQUIRE(is_correct);
}

TEST_CASE("generic result bind when error")
{
    using namespace result;
    using ErrorMessage = std::string;
    struct Age {
        uint8_t value;
    };
    std::string error_message = "this is an error";
    GenericResult<uint8_t, ErrorMessage> age_result = error_message;
    std::function<GenericResult<Age, ErrorMessage>(uint8_t)> safe_function = \
        [](uint8_t age) { return static_cast<GenericResult<Age, ErrorMessage>>(Age{age}); };
    GenericResult<Age, ErrorMessage> age_string_result = bind(age_result, safe_function);
    bool is_correct = std::visit(overload{
        [](Age&)       { return false; },
        [&error_message](ErrorMessage& error)   { return error == error_message; },
    }, age_string_result);
    REQUIRE(is_correct);
}

TEST_CASE("incorrect generic result map_error")
{
    using namespace result;
    using ErrorMessage = std::string;
    struct MyError {
        std::string message;
    };
    std::string error_message = "this is an error";
    GenericResult<uint8_t, ErrorMessage> age_result = error_message;
    std::function<MyError(ErrorMessage)> safe_function = [](ErrorMessage message) { return MyError{message}; };
    GenericResult<uint8_t, MyError> age_result_error = map_error(age_result, safe_function);
    bool is_correct = std::visit(overload{
        [](uint8_t&)       { return false; },
        [&error_message](MyError& my_error)   { return my_error.message == error_message; },
    }, age_result_error);
    REQUIRE(is_correct);
}

TEST_CASE("generic result bind_error when successful")
{
    using namespace result;
    using ErrorMessage = std::string;
    struct MyError {
        std::string message;
    };
    std::string error_message = "this is an error";
    GenericResult<uint8_t, ErrorMessage> age_result = 32;
    std::function<GenericResult<uint8_t, MyError>(ErrorMessage)> safe_function = [](ErrorMessage message) { \
        return static_cast<GenericResult<uint8_t, MyError>>(MyError{message}); };
    GenericResult<uint8_t, MyError> age_result_error = bind_error(age_result, safe_function);
    bool is_correct = std::visit(overload{
        [](uint8_t& age)       { return age == 32; },
        [&](MyError&)   { return false; },
    }, age_result_error);
    REQUIRE(is_correct);
}
