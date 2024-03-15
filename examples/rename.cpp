#include "miniser/miniser.hpp"
#include <iostream>

struct Foo {
  int yourInt;
};

struct Bar {
  int my_int;
  Foo foo;
};

namespace miniser {
template <> inline constexpr rename rename_fields<Bar> = rename::camel_case;
template <> inline constexpr rename rename_fields<Foo> = rename::snake_case;
} // namespace miniser

int main() {
  Bar bar{
      .my_int = 1,
      .foo{
          .yourInt = 2,
      },
  };

  std::optional<miniser::serialized> serialized = miniser::serialize(bar);
  // {"myInt":1,"foo":{"your_int":2}}
  std::cout << serialized->view() << '\n';

  std::optional<Bar> deserialized =
      miniser::deserialize<Bar>(serialized->view());
  // 1
  std::cout << deserialized->my_int << '\n';
}
