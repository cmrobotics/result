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
    std::string error_message = std::string("Error getting age from Database");
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
    std::string error_message = std::string("Employee not found");
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
    std::string error_message = std::string("No contract found for employee");
    Result<Contract> db_contract = Error{ error_message };
    std::function<Result<Contract>(Employee)> find_contract_by_employee_id = [&db_contract](Employee) { return db_contract; };
    Result<Contract> contract_from_employee = bind(db_employee, find_contract_by_employee_id);

    bool is_correct = std::visit(overload{
        [](Contract)       { return false; },
        [&error_message](Error& error)   { return error.message == error_message; },
    }, contract_from_employee);
    REQUIRE(is_correct);
}


TEST_CASE("unsuccessful unsafe error")
{
    using namespace result;
    std::string error_message = std::string("Not possible to open database connection");
    auto error = std::runtime_error(error_message);
    std::function<uint8_t()> get_age_unsafe_and_successful = [&error]() {
        throw error;
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


TEST_CASE("successful unsafe error")
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

