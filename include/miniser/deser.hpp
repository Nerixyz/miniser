#pragma once

#include <boost/pfr.hpp>
#include <limits>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>
#include <yyjson.h>

namespace miniser::deser {

enum class option {
  none = 0,
  /// Check range for (u)int{8,16,32}_t
  check_range = (1 << 0),
  /// Don't deserialize integers as double
  strict_real = (1 << 1),
};

struct context {
  option options = option::none;

  [[nodiscard]] bool has_option(option opt) const {
    return (static_cast<std::underlying_type_t<option>>(this->options) &
            static_cast<std::underlying_type_t<option>>(opt)) != 0;
  }
};

namespace detail {

template <typename T>
std::optional<T> get_integer(yyjson_val *value, const context &ctx);

} // namespace detail

// Declarations

std::optional<bool> deserialize(std::type_identity<bool>, yyjson_val *value,
                                const context &ctx);

std::optional<std::int8_t> deserialize(std::type_identity<std::int8_t>,
                                       yyjson_val *value, const context &ctx);
std::optional<std::int16_t> deserialize(std::type_identity<std::int16_t>,
                                        yyjson_val *value, const context &ctx);
std::optional<std::int32_t> deserialize(std::type_identity<std::int32_t>,
                                        yyjson_val *value, const context &ctx);
std::optional<std::int64_t> deserialize(std::type_identity<std::int64_t>,
                                        yyjson_val *value, const context &ctx);
std::optional<std::uint8_t> deserialize(std::type_identity<std::uint8_t>,
                                        yyjson_val *value, const context &ctx);
std::optional<std::uint16_t> deserialize(std::type_identity<std::uint16_t>,
                                         yyjson_val *value, const context &ctx);
std::optional<std::uint32_t> deserialize(std::type_identity<std::uint32_t>,
                                         yyjson_val *value, const context &ctx);
std::optional<std::uint64_t> deserialize(std::type_identity<std::uint64_t>,
                                         yyjson_val *value, const context &ctx);

std::optional<double> deserialize(std::type_identity<double>, yyjson_val *value,
                                  const context &ctx);

std::optional<std::string> deserialize(std::type_identity<std::string>,
                                       yyjson_val *value, const context &ctx);

// Warning: this must be used with deserialize_borrowed!
std::optional<std::string_view>
deserialize(std::type_identity<std::string_view>, yyjson_val *value,
            const context &ctx);

template <typename T>
std::optional<std::vector<T>> deserialize(std::type_identity<std::vector<T>>,
                                          yyjson_val *value,
                                          const context &ctx);

template <typename T>
  requires std::is_aggregate_v<T>
std::optional<T> deserialize(std::type_identity<T>, yyjson_val *value,
                             const context &ctx);

template <typename T>
std::optional<std::optional<T>>
deserialize(std::type_identity<std::optional<T>>, yyjson_val *value,
            const context &ctx);

// Implementations

inline std::optional<bool> deserialize(std::type_identity<bool>,
                                       yyjson_val *value, const context &) {
  if (!yyjson_is_bool(value)) {
    return std::nullopt;
  }
  return yyjson_get_bool(value);
}

template <typename T>
  requires std::is_aggregate_v<T>
std::optional<T> deserialize(std::type_identity<T>, yyjson_val *value,
                             const context &ctx) {
  if (!yyjson_is_obj(value)) {
    return std::nullopt;
  }

  T foo;
  bool ok = true;
  boost::pfr::for_each_field(foo, [&](auto &field, auto index) {
    if (!ok) {
      return;
    }
    auto key = boost::pfr::get_name<index, T>();
    auto *inner = yyjson_obj_getn(value, key.data(), key.size());
    auto xd = deserialize(
        std::type_identity<std::remove_reference_t<decltype(field)>>{}, inner,
        ctx);
    if (xd.has_value()) {
      field = std::move(*xd);
    } else {
      ok = false;
    }
  });
  if (ok) {
    return std::move(foo);
  }
  return std::nullopt;
}

template <typename T>
std::optional<std::vector<T>> deserialize(std::type_identity<std::vector<T>>,
                                          yyjson_val *value,
                                          const context &ctx) {
  if (!yyjson_is_arr(value)) {
    return std::nullopt;
  }

  std::vector<T> vec;
  vec.reserve(yyjson_arr_size(value));

  yyjson_val *inner = nullptr;
  yyjson_arr_iter iter = yyjson_arr_iter_with(value);
  while ((inner = yyjson_arr_iter_next(&iter))) {
    auto deserialized = deserialize(std::type_identity<T>{}, inner, ctx);
    if (!deserialized.has_value()) {
      return std::nullopt;
    }
    vec.push_back(std::move(*deserialized));
  }

  return vec;
}

inline std::optional<std::int8_t> deserialize(std::type_identity<std::int8_t>,
                                              yyjson_val *value,
                                              const context &ctx) {
  return detail::get_integer<std::int8_t>(value, ctx);
}

inline std::optional<std::int16_t> deserialize(std::type_identity<std::int16_t>,
                                               yyjson_val *value,
                                               const context &ctx) {
  return detail::get_integer<std::int16_t>(value, ctx);
}

inline std::optional<std::int32_t> deserialize(std::type_identity<std::int32_t>,
                                               yyjson_val *value,
                                               const context &ctx) {
  return detail::get_integer<std::int32_t>(value, ctx);
}

inline std::optional<std::int64_t> deserialize(std::type_identity<std::int64_t>,
                                               yyjson_val *value,
                                               const context &ctx) {
  return detail::get_integer<std::int64_t>(value, ctx);
}

inline std::optional<std::uint8_t> deserialize(std::type_identity<std::uint8_t>,
                                               yyjson_val *value,
                                               const context &ctx) {
  return detail::get_integer<std::uint8_t>(value, ctx);
}

inline std::optional<std::uint16_t>
deserialize(std::type_identity<std::uint16_t>, yyjson_val *value,
            const context &ctx) {
  return detail::get_integer<std::uint16_t>(value, ctx);
}

inline std::optional<std::uint32_t>
deserialize(std::type_identity<std::uint32_t>, yyjson_val *value,
            const context &ctx) {
  return detail::get_integer<std::uint32_t>(value, ctx);
}

inline std::optional<std::uint64_t>
deserialize(std::type_identity<std::uint64_t>, yyjson_val *value,
            const context &ctx) {
  return detail::get_integer<std::uint64_t>(value, ctx);
}

inline std::optional<double>
deserialize(std::type_identity<double>, yyjson_val *value, const context &ctx) {
  if (yyjson_is_real(value)) {
    return yyjson_get_real(value);
  }

  if (ctx.has_option(option::strict_real)) {
    return std::nullopt;
  }

  // try uint first to get a better range
  if (yyjson_is_uint(value)) {
    return static_cast<double>(yyjson_get_uint(value));
  }
  if (yyjson_is_int(value)) {
    return static_cast<double>(yyjson_get_sint(value));
  }
  return std::nullopt;
}

inline std::optional<std::string> deserialize(std::type_identity<std::string>,
                                              yyjson_val *value,
                                              const context &) {
  if (!yyjson_is_str(value)) {
    return std::nullopt;
  }
  const char *s = yyjson_get_str(value);
  size_t size = yyjson_get_len(value);
  if (!s) {
    return std::nullopt;
  }
  return std::string(s, size);
}

// Warning: this must be used with deserialize_borrowed!
inline std::optional<std::string_view>
deserialize(std::type_identity<std::string_view>, yyjson_val *value,
            const context &) {
  if (!yyjson_is_str(value)) {
    return std::nullopt;
  }
  const char *s = yyjson_get_str(value);
  size_t size = yyjson_get_len(value);
  if (!s) {
    return std::nullopt;
  }
  return std::string_view(s, size);
}

template <typename T>
std::optional<std::optional<T>>
deserialize(std::type_identity<std::optional<T>>, yyjson_val *value,
            const context &ctx) {
  if (yyjson_is_null(value)) {
    return std::optional<T>(std::nullopt);
  }
  return deserialize(std::type_identity<T>{}, value, ctx);
}

namespace detail {

template <typename T>
std::optional<T> get_integer(yyjson_val *value, const context &ctx) {
  static_assert(sizeof(T) <= 8);

  if constexpr (std::numeric_limits<T>::is_signed) {
    if (!yyjson_is_int(value)) {
      return std::nullopt;
    }

    std::int64_t sint = yyjson_get_sint(value);

    if (ctx.has_option(option::check_range)) {
      if (sint > static_cast<std::int64_t>(std::numeric_limits<T>::max())) {
        return std::nullopt;
      }
      if (sint < static_cast<std::int64_t>(std::numeric_limits<T>::min())) {
        return std::nullopt;
      }
    }
    return static_cast<T>(sint);
  } else {
    if (!yyjson_is_uint(value)) {
      return std::nullopt;
    }

    std::uint64_t uint = yyjson_get_uint(value);

    if (ctx.has_option(option::check_range)) {
      if (uint > static_cast<std::uint64_t>(std::numeric_limits<T>::max())) {
        return std::nullopt;
      }
    }
    return static_cast<T>(uint);
  }
}

} // namespace detail

} // namespace miniser::deser
