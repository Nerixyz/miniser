# miniser

`miniser` is a small header-only C++ 20 JSON (de-)serialization library using [Boost.PFR][Boost.PFR] and [yyjson](https://ibireme.github.io/yyjson/).

It doesn't require any action from the user or codegen to be able to (de-)serialize structs (see [limitations](#limitations)).

## Usage

From [examples/basic.cpp](examples/basic.cpp):

```cpp
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
  std::cout << serialized->view() << '\n';

  std::optional<Bar> deserialized =
      miniser::deserialize<Bar>(serialized->view());
  std::cout << deserialized->name << '\n';
}
```

This will print:

```text
{"name":"bar","foo":[{"i":42,"b":true},{"i":0,"b":null}]}
bar
```

## Limitations

The limitations of [Boost.PFR][Boost.PFR-lim] apply (only simple aggregates are supported).
Furthermore, renaming isn't supported.

[Boost.PFR]: www.boost.org/doc/libs/master/doc/html/boost_pfr.html
[Boost.PFR-lim]: https://www.boost.org/doc/libs/master/doc/html/boost_pfr/limitations_and_configuration.html
