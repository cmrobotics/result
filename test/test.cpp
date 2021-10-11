#include <result.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <iostream>

TEST_CASE("test version")
{
    REQUIRE(RESULT_VERSION_STRING == std::string("0.1.0"));
}

TEST_CASE("test correct result")
{
    using namespace result;
    Result<int> age_result = 32;
    bool is_correct = std::visit(overload{
        [](int& age)       { return age == 32; },
        [](Error&)   { return false; },
    }, age_result);
    REQUIRE(is_correct);
}

TEST_CASE("test error result")
{
    using namespace result;
    std::string error_message = std::string("Error getting age from Database");
    Result<int> age_result = Error{ error_message };
    bool is_correct = std::visit(overload{
        [](int&)       { return false; },
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

TEST_CASE("compose results with final error")
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