#pragma once

#include <boost/pfr.hpp>

namespace miniser {

enum class rename { none, camel_case, snake_case };

template <class T> inline constexpr rename rename_fields = rename::none;

namespace detail {

template <std::size_t I, class T>
inline constexpr auto name_of_field = boost::pfr::get_name<I, T>();

template <typename T>
concept rename_fields_camel = rename_fields<T> == rename::camel_case;

template <typename T>
concept rename_fields_snake = rename_fields<T> == rename::snake_case;

namespace casing {

consteval bool is_upper(char c) { return c >= 'A' && c <= 'Z'; }
consteval bool is_lower(char c) { return c >= 'a' && c <= 'z'; }

consteval char to_upper(char c) {
  if (is_lower(c)) {
    return static_cast<char>(c - ('a' - 'A'));
  }
  return c;
}

consteval char to_lower(char c) {
  if (is_upper(c)) {
    return static_cast<char>(c + ('a' - 'A'));
  }
  return c;
}

namespace camel {

consteval auto count_len(std::string_view s) noexcept {
  size_t count = 0;
  for (auto c : s) {
    if (c != '_') {
      count++;
    }
  }
  return count;
}

template <size_t L> consteval auto convert(std::string_view s) {
  auto res = std::array<char, L + 1>{};

  auto *out = res.data();
  bool was_underscore = false;
  for (auto c : s) {
    if (c == '_') {
      was_underscore = true;
      continue;
    }
    if (was_underscore) {
      c = to_upper(c);
      was_underscore = false;
    }
    *out = c;
    ++out;
  }

  return res;
}

template <std::size_t I, class T>
inline constexpr auto stored_name_of_field =
    convert<count_len(boost::pfr::get_name<I, T>())>(
        boost::pfr::get_name<I, T>());

} // namespace camel

namespace snake {

consteval auto count_len(std::string_view s) noexcept {
  size_t count = 0;
  bool was_upper = false;
  for (auto c : s) {
    if (is_upper(c)) {
      if (!was_upper) {
        was_upper = true;
        count++;
      }
    } else {
      was_upper = false;
    }
    count++;
  }
  return count;
}

template <size_t L> consteval auto convert(std::string_view s) {
  auto res = std::array<char, L + 1>{};

  auto *out = res.data();
  bool was_upper = false;
  for (auto c : s) {
    if (is_upper(c)) {
      if (!was_upper) {
        was_upper = true;
        *out = '_';
        ++out;
      }
      c = to_lower(c);
    } else {
      was_upper = false;
    }
    *out = c;
    ++out;
  }

  return res;
}

template <std::size_t I, class T>
inline constexpr auto stored_name_of_field =
    convert<count_len(boost::pfr::get_name<I, T>())>(
        boost::pfr::get_name<I, T>());

} // namespace snake

} // namespace casing

template <std::size_t I, rename_fields_camel T>
inline constexpr std::string_view name_of_field<I, T> =
    casing::camel::stored_name_of_field<I, T>.data();

template <std::size_t I, rename_fields_snake T>
inline constexpr std::string_view name_of_field<I, T> =
    casing::snake::stored_name_of_field<I, T>.data();

} // namespace detail

} // namespace miniser
