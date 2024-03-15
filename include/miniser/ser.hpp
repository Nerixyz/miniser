#pragma once

#include <boost/pfr.hpp>
#include <miniser/detail/names.hpp>
#include <optional>
#include <string_view>
#include <type_traits>
#include <vector>
#include <yyjson.h>

namespace miniser::ser {

inline yyjson_mut_val *serialize(bool value, yyjson_mut_doc *doc) {
  return yyjson_mut_bool(doc, value);
}

inline yyjson_mut_val *serialize(double value, yyjson_mut_doc *doc) {
  return yyjson_mut_real(doc, value);
}

inline yyjson_mut_val *serialize(uint8_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_uint(doc, static_cast<uint64_t>(value));
}

inline yyjson_mut_val *serialize(uint16_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_uint(doc, static_cast<uint64_t>(value));
}

inline yyjson_mut_val *serialize(uint32_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_uint(doc, static_cast<uint64_t>(value));
}

inline yyjson_mut_val *serialize(uint64_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_uint(doc, value);
}

inline yyjson_mut_val *serialize(int8_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_sint(doc, static_cast<int64_t>(value));
}

inline yyjson_mut_val *serialize(int16_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_sint(doc, static_cast<int64_t>(value));
}

inline yyjson_mut_val *serialize(int32_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_sint(doc, static_cast<int64_t>(value));
}

inline yyjson_mut_val *serialize(int64_t value, yyjson_mut_doc *doc) {
  return yyjson_mut_sint(doc, value);
}

inline yyjson_mut_val *serialize(const std::string &value,
                                 yyjson_mut_doc *doc) {
  return yyjson_mut_strn(doc, value.data(), value.size());
}

inline yyjson_mut_val *serialize(std::string_view value, yyjson_mut_doc *doc) {
  return yyjson_mut_strn(doc, value.data(), value.size());
}

template <typename T>
  requires std::is_aggregate_v<T>
yyjson_mut_val *serialize(const T &value, yyjson_mut_doc *doc);

template <typename T>
yyjson_mut_val *serialize(const std::vector<T> &vec, yyjson_mut_doc *doc);

template <typename T>
yyjson_mut_val *serialize(const std::optional<T> &opt, yyjson_mut_doc *doc);

// Implementations

template <typename T>
  requires std::is_aggregate_v<T>
yyjson_mut_val *serialize(const T &value, yyjson_mut_doc *doc) {
  auto *obj = yyjson_mut_obj(doc);
  if (!obj) {
    return obj;
  }

  boost::pfr::for_each_field(value, [&](const auto &field, auto index) {
    auto key = miniser::detail::name_of_field<index, T>;
    auto *kv = yyjson_mut_strn(doc, key.data(), key.size());
    if (!kv) {
      return;
    }
    auto *v = serialize(field, doc);
    if (!v) {
      return;
    }
    yyjson_mut_obj_add(obj, kv, v);
  });
  return obj;
}

template <typename T>
yyjson_mut_val *serialize(const std::vector<T> &vec, yyjson_mut_doc *doc) {
  auto *arr = yyjson_mut_arr(doc);
  if (!arr) {
    return arr;
  }

  for (const auto &value : vec) {
    auto *s = serialize(value, doc);
    if (!s) {
      return nullptr;
    }
    if (!yyjson_mut_arr_append(arr, s)) {
      return nullptr;
    }
  }

  return arr;
}

template <typename T>
yyjson_mut_val *serialize(const std::optional<T> &opt, yyjson_mut_doc *doc) {
  if (!opt.has_value()) {
    return yyjson_mut_null(doc);
  }

  return serialize(*opt, doc);
}

} // namespace miniser::ser
