#include "miniser/miniser.hpp"
#include <gtest/gtest.h>

template <typename T> void check_eq(T in, std::string_view expected) {
  auto serialized = miniser::serialize<T>(in);
  EXPECT_TRUE(serialized.has_value()) << expected;
  EXPECT_EQ(serialized->view(), expected) << expected;
  EXPECT_EQ(serialized->view(), serialized->to_string()) << expected;
}

template <typename T> void signed_check() {
  auto max = std::numeric_limits<T>::max();
  auto min = std::numeric_limits<T>::min();
  std::string max_s = std::to_string(max);
  std::string min_s = std::to_string(min);

  check_eq<T>(0, "0");
  check_eq<T>(1, "1");
  check_eq<T>(-1, "-1");
  check_eq<T>(max, max_s);
  check_eq<T>(min, min_s);
}

template <typename T> void unsigned_check() {
  auto max = std::numeric_limits<T>::max();
  std::string max_s = std::to_string(max);

  check_eq<T>(0, "0");
  check_eq<T>(1, "1");
  check_eq<T>(max, max_s);
}

TEST(Serialize, Int8) { signed_check<int8_t>(); }
TEST(Serialize, Uint8) { unsigned_check<uint8_t>(); }

TEST(Serialize, Int16) { signed_check<int16_t>(); }
TEST(Serialize, Uint16) { unsigned_check<uint16_t>(); }

TEST(Serialize, Int32) { signed_check<int32_t>(); }
TEST(Serialize, Uint32) { unsigned_check<uint32_t>(); }

TEST(Serialize, Int64) { signed_check<int64_t>(); }
TEST(Serialize, Uint64) { unsigned_check<uint64_t>(); }

TEST(Serialize, Bool) {
  check_eq<bool>(true, "true");
  check_eq<bool>(false, "false");
}

TEST(Serialize, Double) {
  check_eq<double>(1, "1.0");
  check_eq<double>(42, "42.0");
  check_eq<double>(1.1, "1.1");
  check_eq<double>(-1.6, "-1.6");
}

TEST(Serialize, String) {
  check_eq<std::string>("\"yo\"", R"("\"yo\"")");
  check_eq<std::string>("hello", "\"hello\"");
}

TEST(Serialize, StringView) {
  check_eq<std::string_view>("\"yo\"", R"("\"yo\"")");
  check_eq<std::string_view>("hello", "\"hello\"");
}

struct Plain {
  int i;
  std::string name;
  bool f;

  auto operator<=>(const Plain &other) const = default;
};

TEST(Serialize, Plain) {
  check_eq<Plain>(Plain{1, "abc", true}, R"({"i":1,"name":"abc","f":true})");
}

struct Nested {
  int i;
  std::string name;
  bool f;
  Plain p;

  auto operator<=>(const Nested &other) const = default;
};

TEST(Serialize, Nested) {
  check_eq<Nested>(
      Nested{1, "abc", true, Plain{2, "def", false}},
      R"({"i":1,"name":"abc","f":true,"p":{"i":2,"name":"def","f":false}})");
}

struct MaybeNested {
  int i;
  std::string name;
  bool f;
  std::optional<Plain> p;

  auto operator<=>(const MaybeNested &other) const = default;
};

TEST(Serialize, MaybeNested) {
  check_eq<MaybeNested>(
      MaybeNested{1, "abc", true, Plain{2, "def", false}},
      R"({"i":1,"name":"abc","f":true,"p":{"i":2,"name":"def","f":false}})");
  check_eq<MaybeNested>(MaybeNested{1, "abc", true, std::nullopt},
                        R"({"i":1,"name":"abc","f":true,"p":null})");
}

TEST(Serialize, Vector) {
  check_eq<std::vector<Plain>>(std::vector{Plain{1, "abc", true}},
                               R"([{"i":1,"name":"abc","f":true}])");
  check_eq<std::vector<Plain>>(
      std::vector{
          Plain{1, "abc", true},
          Plain{2, "def", false},
      },
      R"([{"i":1,"name":"abc","f":true},{"i":2,"name":"def","f":false}])");
  check_eq<std::vector<Plain>>(std::vector<Plain>{}, R"([])");
}
