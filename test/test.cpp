#include <result.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("test version")
{
    REQUIRE(RESULT_VERSION_STRING == std::string("0.1.0"));
}

TEST_CASE("test_exclaim")
{
    //std::string value = result::exclaim("hello");
    //REQUIRE(value == std::string("hello!"));
}

TEST_CASE("test_expensive")
{
    //std::size_t work = 100;
    //std::string result = result::expensive(work);
    //REQUIRE(result == std::string("Expensive work is finished"));
}