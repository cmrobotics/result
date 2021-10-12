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

TEST_CASE("unsuccessful unsafe error")
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

