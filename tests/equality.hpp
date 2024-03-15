#pragma once

#include "miniser/miniser.hpp"
#include <gtest/gtest.h>

namespace test_deser {

template <typename T>
void check_eq(std::string_view in, std::optional<T> expected,
              miniser::deser::option opts = miniser::deser::option::none) {
  EXPECT_EQ(miniser::deserialize<T>(in, {opts}), expected) << in;
}

template <typename T>
void check_borrowed_eq(
    std::string_view in, std::optional<T> expected,
    miniser::deser::option opts = miniser::deser::option::none) {
  auto des = miniser::deserialize_borrowed<T>(in, {opts});
  if (expected) {
    EXPECT_EQ(**des, expected) << in;
  } else {
    EXPECT_EQ(des.has_value(), expected.has_value()) << in;
  }
}

} // namespace test_deser

namespace test_ser {

template <typename T> void check_eq(T in, std::string_view expected) {
  auto serialized = miniser::serialize<T>(in);
  EXPECT_TRUE(serialized.has_value()) << expected;
  EXPECT_EQ(serialized->view(), expected) << expected;
  EXPECT_EQ(serialized->view(), serialized->to_string()) << expected;
}

} // namespace test_ser
