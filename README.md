# Go Language Module for Plugify

The Go Language Module for Plugify enables developers to write plugins in Go for the Plugify framework. This module provides a seamless integration for Go plugins, allowing them to be dynamically loaded and managed by the Plugify core.

## Features

- **Go Plugin Support**: Write your plugins in Go and integrate them effortlessly with the Plugify framework.
- **Automatic Exporting**: Easily export and import methods between plugins and the language module.
- **Initialization and Cleanup**: Handle plugin initialization, startup, and cleanup with dedicated module events.
- **Interoperability**: Communicate with plugins written in other languages through auto-generated interfaces.

## Getting Started

### Prerequisites

- Go Compiler
- Plugify Framework Installed

### Installation

1. Clone this repository:

    ```bash
    git clone https://github.com/untrustedmodders/cpp-lang-module.git
    cd cpp-lang-module
    git submodule update --init --recursive
    ```

2. Build the Go language module:

    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build .
    ```

### Usage

1. **Integration with Plugify**

   Ensure that your Go language module is available in the same directory as your Plugify setup.

2. **Write Go Plugins**

   Develop your plugins in Go using the Plugify Go API. Refer to the [Plugify Go Plugin Guide](https://docs.plugify.io/cpp-plugin-guide) for detailed instructions.

3. **Build and Install Plugins**

   Compile your Go plugins and place the shared libraries in a directory accessible to the Plugify core.

4. **Run Plugify**

   Start the Plugify framework, and it will dynamically load your Go plugins.

## Example

```c++
#include <plugify/cpp_plugin.h>
#include <plugin_export.h>
#include <iostream>

class ExamplePlugin : public plugify::IPluginEntry {
public:
	void OnPluginStart() override {
		std::cout << "Example Start!" << std::endl;
	}

	void OnPluginEnd() override {
		std::cout << "Example End!" << std::endl;
	}
} g_examplePlugin;

EXPOSE_PLUGIN(PLUGIN_API, &g_examplePlugin)
```

## Documentation

For comprehensive documentation on writing plugins in Go using the Plugify framework, refer to the [Plugify Documentation](https://docs.plugify.io).

## Contributing

Feel free to contribute by opening issues or submitting pull requests. We welcome your feedback and ideas!

## License

This Go Language Module for Plugify is licensed under the [MIT License](LICENSE).
