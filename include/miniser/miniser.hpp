#pragma once

#include <miniser/deser.hpp>
#include <miniser/ser.hpp>

#include <string_view>

namespace miniser {

namespace detail {

template <typename T, void (*cleanup)(T *)> class managed_yydoc {
public:
  managed_yydoc(T *doc) : doc_(doc) {}
  ~managed_yydoc() {
    if (this->doc_) {
      cleanup(this->doc_);
    }
  }
  managed_yydoc(managed_yydoc &&other) noexcept : doc_(other.doc_) {
    other.doc_ = nullptr;
  }
  managed_yydoc(const managed_yydoc &) = delete;

  managed_yydoc &operator=(managed_yydoc &&other) noexcept {
    if (this->doc_) {
      cleanup(this->doc_);
    }

    this->doc_ = other.doc_;
    other.doc_ = nullptr;
    return *this;
  }
  managed_yydoc &operator=(const managed_yydoc &) = delete;

  T *operator()() { return this->doc_; }

private:
  T *doc_ = nullptr;
};

using yydoc = managed_yydoc<yyjson_doc, yyjson_doc_free>;
using yydoc_mut = managed_yydoc<yyjson_mut_doc, yyjson_mut_doc_free>;

} // namespace detail

template <typename T> class borrowed {
public:
  borrowed(detail::yydoc doc, T value)
      : doc_(std::move(doc)), value_(std::forward<T>(value)) {}
  borrowed(const borrowed &) = delete;
  borrowed(borrowed &&) = default;
  borrowed &operator=(borrowed &&) = default;
  borrowed &operator=(const borrowed &) = delete;
  ~borrowed() = default;

  const T &operator*() const noexcept { return this->value_; }
  const T *operator->() const noexcept { return &this->value_; }

private:
  detail::yydoc doc_;
  T value_;
};

template <typename T>
std::optional<T> deserialize(std::string_view str,
                             const deser::context &ctx = {},
                             yyjson_read_flag flags = 0) {
  detail::yydoc doc = yyjson_read(str.data(), str.size(), flags);
  if (!doc()) {
    return std::nullopt;
  }

  return deser::deserialize(std::type_identity<T>{}, yyjson_doc_get_root(doc()),
                            ctx);
}

template <typename T>
std::optional<borrowed<T>> deserialize_borrowed(std::string_view str,
                                                const deser::context &ctx = {},
                                                yyjson_read_flag flags = 0) {
  detail::yydoc doc = yyjson_read(str.data(), str.size(), flags);
  if (!doc()) {
    return std::nullopt;
  }

  auto de = deser::deserialize(std::type_identity<T>{},
                               yyjson_doc_get_root(doc()), ctx);
  if (!de.has_value()) {
    return std::nullopt;
  }
  return borrowed<T>(std::move(doc), std::forward<T>(*de));
}

class serialized {
public:
  serialized(char *str, size_t len) : str_(str), len_(len) {}
  ~serialized() {
    if (this->str_) {
      // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
      free(this->str_);
    }
  }
  serialized(serialized &&other) noexcept : str_(other.str_), len_(other.len_) {
    other.str_ = nullptr;
    other.len_ = 0;
  }
  serialized(const serialized &) = delete;

  serialized &operator=(serialized &&other) noexcept {
    if (this->str_) {
      // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
      free(this->str_);
    }

    this->str_ = other.str_;
    this->len_ = other.len_;
    other.str_ = nullptr;
    other.len_ = 0;
    return *this;
  }
  serialized &operator=(const serialized &) = delete;

  [[nodiscard]] std::string_view view() const {
    return {this->str_, this->len_};
  }

  [[nodiscard]] std::string to_string() const {
    return {this->str_, this->len_};
  }

private:
  char *str_ = nullptr;
  size_t len_ = 0;
};

template <typename T>
std::optional<serialized> serialize(const T &value,
                                    yyjson_write_flag flags = 0) {
  detail::yydoc_mut doc = yyjson_mut_doc_new(nullptr);
  if (!doc()) {
    return std::nullopt;
  }

  auto *root = ser::serialize(value, doc());
  if (!root) {
    return std::nullopt;
  }

  yyjson_mut_doc_set_root(doc(), root);

  size_t size = 0;
  auto *str = yyjson_mut_write(doc(), flags, &size);
  if (!str) {
    return std::nullopt;
  }

  return serialized(str, size);
}

} // namespace miniser
