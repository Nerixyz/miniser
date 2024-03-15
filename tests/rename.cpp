#include "equality.hpp"
#include <gtest/gtest.h>

namespace rename_test {
struct Foo {
  int my_int;
  int mySecondInt;

  bool operator==(const Foo &other) const {
    return boost::pfr::eq_fields(*this, other);
  }
};

struct Camel {
  int my_int;
  int mySecondInt;

  bool operator==(const Camel &other) const {
    return boost::pfr::eq_fields(*this, other);
  }
};

struct Snake {
  int my_int;
  int mySecondInt;

  bool operator==(const Snake &other) const {
    return boost::pfr::eq_fields(*this, other);
  }
};

} // namespace rename_test

using namespace rename_test;

namespace miniser {
template <> inline constexpr rename rename_fields<Camel> = rename::camel_case;
template <> inline constexpr rename rename_fields<Snake> = rename::snake_case;
} // namespace miniser

TEST(Reaname, None) {
  static_assert(miniser::detail::name_of_field<0, Foo> == "my_int");
  static_assert(miniser::detail::name_of_field<1, Foo> == "mySecondInt");

  test_ser::check_eq(Foo{1, 2}, R"({"my_int":1,"mySecondInt":2})");
  test_deser::check_eq<Foo>(R"({"my_int":1,"mySecondInt":2})", Foo{1, 2});
}

TEST(Reaname, Camel) {
  static_assert(miniser::detail::name_of_field<0, Camel> == "myInt");
  static_assert(miniser::detail::name_of_field<1, Camel> == "mySecondInt");

  test_ser::check_eq(Camel{1, 2}, R"({"myInt":1,"mySecondInt":2})");
  test_deser::check_eq<Camel>(R"({"myInt":1,"mySecondInt":2})", Camel{1, 2});
}

TEST(Reaname, Snake) {
  static_assert(miniser::detail::name_of_field<0, Snake> == "my_int");
  static_assert(miniser::detail::name_of_field<1, Snake> == "my_second_int");

  test_ser::check_eq(Snake{1, 2}, R"({"my_int":1,"my_second_int":2})");
  test_deser::check_eq<Snake>(R"({"my_int":1,"my_second_int":2})", Snake{1, 2});
}
