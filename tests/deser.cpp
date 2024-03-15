#include "equality.hpp"
#include <gtest/gtest.h>

constexpr auto check_range = miniser::deser::option::check_range;
constexpr auto strict_real = miniser::deser::option::strict_real;

using namespace test_deser;

template <typename T>
void signed_check(std::string_view over, std::string_view under) {
  auto max = std::numeric_limits<T>::max();
  auto min = std::numeric_limits<T>::min();
  std::string max_s = std::to_string(max);
  std::string min_s = std::to_string(min);

  check_eq<T>("0", 0);
  check_eq<T>("1", 1);
  check_eq<T>("-1", -1);
  check_eq<T>(max_s, max);
  check_eq<T>(min_s, min);
  check_eq<T>(under, std::nullopt, check_range);
  check_eq<T>(over, std::nullopt, check_range);
}

template <typename T> void unsigned_check(std::string_view over) {
  auto max = std::numeric_limits<T>::max();
  std::string max_s = std::to_string(max);

  check_eq<T>("0", 0);
  check_eq<T>("1", 1);
  check_eq<T>("-1", std::nullopt);
  check_eq<T>(max_s, max);
  check_eq<T>(over, std::nullopt, check_range);
}

TEST(Deserialize, Int8) { signed_check<int8_t>("128", "-129"); }
TEST(Deserialize, Uint8) { unsigned_check<uint8_t>("256"); }

TEST(Deserialize, Int16) { signed_check<int16_t>("32768", "-32769"); }
TEST(Deserialize, Uint16) { unsigned_check<uint16_t>("65536"); }

TEST(Deserialize, Int32) { signed_check<int32_t>("2147483648", "-2147483649"); }
TEST(Deserialize, Uint32) { unsigned_check<uint32_t>("4294967296"); }

// TODO: this seems to be a bug in yyjson
TEST(Deserialize, Int64) {
  signed_check<int64_t>("18446744073709551616", "-9223372036854775809");
}
TEST(Deserialize, Uint64) { unsigned_check<uint64_t>("18446744073709551616"); }

TEST(Deserialize, Bool) {
  check_eq<bool>("true", true);
  check_eq<bool>("false", false);
  check_eq<bool>("True", std::nullopt);
  check_eq<bool>("False", std::nullopt);
  check_eq<bool>("1", std::nullopt);
}

TEST(Deserialize, Double) {
  check_eq<double>("1", 1);
  check_eq<double>("42", 42);
  check_eq<double>("1", std::nullopt, strict_real);
  check_eq<double>("42", std::nullopt, strict_real);
  check_eq<double>("1.1", 1.1);
  check_eq<double>("-1.6", -1.6);
  check_eq<double>("false", std::nullopt);
}

TEST(Deserialize, String) {
  check_eq<std::string>(R"("\"yo\"")", "\"yo\"");
  check_eq<std::string>("\"hello\"", "hello");
  check_eq<std::string>("false", std::nullopt);
  check_eq<std::string>("1", std::nullopt);
}

TEST(Deserialize, StringView) {
  check_borrowed_eq<std::string_view>(R"("\"yo\"")", "\"yo\"");
  check_borrowed_eq<std::string_view>("\"hello\"", "hello");
  check_borrowed_eq<std::string_view>("false", std::nullopt);
  check_borrowed_eq<std::string_view>("1", std::nullopt);
}

struct Plain {
  int i;
  std::string name;
  bool f;

  // TODO: this should use <=>, but clang 14 doesn't implement it for
  // std::string
  bool operator==(const Plain &other) const {
    return boost::pfr::eq_fields(*this, other);
  }
};

TEST(Deserialize, Plain) {
  check_eq<Plain>("1", std::nullopt);
  check_eq<Plain>("{}", std::nullopt);
  check_eq<Plain>(R"({"i":1,"f":true,"name":"abc"})", Plain{1, "abc", true});
  check_eq<Plain>(R"({"i":false,"f":true,"name":"abc"})", std::nullopt);
  check_eq<Plain>(R"({"f":true,"name":"abc"})", std::nullopt);
}

struct Nested {
  int i;
  std::string name;
  bool f;
  Plain p;

  bool operator==(const Nested &other) const {
    return boost::pfr::eq_fields(*this, other);
  }
};

TEST(Deserialize, Nested) {
  check_eq<Nested>("1", std::nullopt);
  check_eq<Nested>("{}", std::nullopt);
  check_eq<Nested>(
      R"({"i":1,"f":true,"name":"abc","p":{"i": 2,"name":"def","f":false}})",
      Nested{1, "abc", true, Plain{2, "def", false}});
  check_eq<Nested>(R"({"i":1,"f":true,"name":"abc"})", std::nullopt);
  check_eq<Nested>(R"({"i":false,"f":true,"name":"abc"})", std::nullopt);
  check_eq<Nested>(R"({"f":true,"name":"abc"})", std::nullopt);
  check_eq<Nested>(
      R"({"i":1,"f":true,"name":"abc","p":{"i": false,"name":"def","f":false}})",
      std::nullopt);
}

struct MaybeNested {
  int i;
  std::string name;
  bool f;
  std::optional<Plain> p;

  bool operator==(const MaybeNested &other) const {
    return boost::pfr::eq_fields(*this, other);
  }
};

TEST(Deserialize, MaybeNested) {
  check_eq<MaybeNested>("1", std::nullopt);
  check_eq<MaybeNested>("{}", std::nullopt);
  check_eq<MaybeNested>(
      R"({"i":1,"f":true,"name":"abc","p":{"i":2,"name":"def","f":false}})",
      MaybeNested{1, "abc", true, Plain{2, "def", false}});
  check_eq<MaybeNested>(R"({"i":1,"f":true,"name":"abc"})",
                        MaybeNested{1, "abc", true, std::nullopt});
  check_eq<MaybeNested>(R"({"i":false,"f":true,"name":"abc"})", std::nullopt);
  check_eq<MaybeNested>(
      R"({"f":true,"name":"abc","p":{"i":2,"name":"def","f":false}})",
      std::nullopt);
  check_eq<MaybeNested>(
      R"({"i":1,"f":true,"name":"abc","p":{"i":false,"name":"def","f":false}})",
      MaybeNested{1, "abc", true, std::nullopt});
}

TEST(Deserialize, Vector) {
  check_eq<std::vector<Plain>>("1", std::nullopt);
  check_eq<std::vector<Plain>>("{}", std::nullopt);
  check_eq<std::vector<Plain>>(R"({"i":1,"f":true,"name":"abc"})",
                               std::nullopt);
  check_eq<std::vector<Plain>>(R"({"i":false,"f":true,"name":"abc"})",
                               std::nullopt);
  check_eq<std::vector<Plain>>(R"({"f":true,"name":"abc"})", std::nullopt);
  check_eq<std::vector<Plain>>(R"([{"i":1,"f":true,"name":"abc"}])",
                               std::vector{Plain{1, "abc", true}});
  check_eq<std::vector<Plain>>(
      R"([{"i":1,"f":true,"name":"abc"}, {"i":2,"f":false,"name":"def"}])",
      std::vector{
          Plain{1, "abc", true},
          Plain{2, "def", false},
      });
  check_eq<std::vector<Plain>>(R"([])", std::vector<Plain>{});
  check_eq<std::vector<Plain>>(
      R"([{"i":1,"f":true,"name":"abc"}, {"i":false,"f":false,"name":"def"}])",
      std::nullopt);
}
