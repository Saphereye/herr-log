# Herr Log
A single header lightweight logging library.

## Download
Simply download the project and import `herrlog.hh`.

## Usage
Herr Log supports the following log levels.
| Log Level | Use Case |
| --------- | -------- |
| Error     | To report critical errors that require immediate attention and may result in program termination or malfunction. |
| Info      | For reporting significant events, milestones, or key actions in the program's execution (e.g., program start, major processes). |
| Trace     | To capture fine-grained details and trace specific actions, providing insights into the program's behavior (e.g., variable values, function calls). |
| Debug     | Intended for temporary and detailed reporting during development or debugging phases to understand and analyze specific parts of the code. Use sparingly in production. |

All these can be activated/deactivated independently.

### Basic usage
```cpp
#include "herrlog.hh"

int main() {
    Logger::trace("This is trace message number 1"); // Initialization is not required
}
```

### Custom output stream (e.g. logfiles)
In place of log_file, any other stream can also be put.

```cpp
#include "herrlog.hh"

int main() {
    Logger::init(LogType::All, "test.log"); // The output will be redirected to "test.log"
    Logger::trace("This is trace message number 1");
}
```

### Custom output message types
For example, if only error and info messages are needed.

```cpp
#include "herrlog.hh"

int main() {
    Logger::init(LogType::Error || LogType::Info, "test.log");
    Logger::error("Erroneous error"); // This will be printed
    Logger::trace("Tracing something"); // This wouldn't be printed
}
```

### Inbuilt formatting
```cpp
#include "herrlog.hh"

int main() {
    Logger::init(LogType::All, "test.log");
    Logger::trace("This is {} message number {}", "trace", 1); // Will be printed as "This is trace message number 1"
}
```
