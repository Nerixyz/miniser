#include "miniser/miniser.hpp"
#include <iostream>

struct Foo {
  int i;
  std::optional<bool> b;
};

struct Bar {
  std::string name;
  std::vector<Foo> foo;
};

int main() {
  Bar bar{
      .name = "bar",
      .foo =
          {
              Foo{
                  .i = 42,
                  .b = true,
              },
              Foo{
                  .i = 0,
              },
          },
  };

  std::optional<miniser::serialized> serialized = miniser::serialize(bar);
  // {"name":"bar","foo":[{"i":42,"b":true},{"i":0,"b":null}]}
  std::cout << serialized->view() << '\n';

  std::optional<Bar> deserialized =
      miniser::deserialize<Bar>(serialized->view());
  // bar
  std::cout << deserialized->name << '\n';
}
