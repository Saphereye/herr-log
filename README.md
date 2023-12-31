# Herr Log
A single header lightweight logging library.

## Download
Simply download the project and import `herrlog.hh`.

## Usage
Herr Log supports the following log levels.
| Log Level | Use Case |
| --------- | -------- |
| Trace     | Capture fine-grained details and trace specific actions, offering insights into the program's behavior (e.g., variable values, function calls). |
| Info      | Report significant events, milestones, or key actions in the program's execution (e.g., program start, major processes). |
| Debug     | Intended for temporary and detailed reporting during development or debugging phases to comprehend and analyze specific parts of the code. Use sparingly in production. |
| Warn | Report errors or potential areas where errors have not been handled gracefully. Does not terminate the program. |
| Error     | Report critical errors requiring immediate attention. Results in program termination. |
| Fatal | Report critical errors, terminate the program, and generate a crash dump (abort). |

All these can be activated/deactivated independently by setting the style using `Logger::set_type`.

### Basic usage
```cpp
#include "herrlog.hh"

int main() {
    Logger::trace("This is trace message number 1"); // Initialization is not required
}
```
By default, the style is `LogStyle::All`, i.e. all log types are enabled. The output is directed to the console using anse colors and the datetime format is "%Y-%m-%d %H:%M:%S".

### Custom output stream (e.g. logfiles)
In place of log_file, any other stream can also be put.

```cpp
#include "herrlog.hh"

int main() {
    Logger::set_output_file_name("test.log"); // The output will be redirected to "test.log"
    Logger::trace("This is trace message number 1");
}
```

### Custom output message types
For example, if only error and info messages are needed.

```cpp
#include "herrlog.hh"

int main() {
    Logger::set_output_file_name("test.log");
    Logger::set_type(LogType::Error | LogType::Debug); // Only Error and Debug logs will be shown
    Logger::error("Erroneous error"); // This will be printed
    Logger::trace("Tracing something"); // This wouldn't be printed
}
```

### Inbuilt formatting
```cpp
#include "herrlog.hh"

int main() {
    Logger::trace("This is {} message number {}", "trace", 1); // Will be printed as "[TRACE 2023-12-31 12:41:09] This is trace message number 1"
}
```

### Custom datetime format
The default datetime format is `%Y-%m-%d %H:%M:%S`. These variables can then be used to specify a custom date time format.
```cpp
#include "herrlog.hh"

int main() {
    Logger::set_datetime_format("%H:%M:%S");
    Logger::trace("Hello World"); // Will be printed as "[TRACE 12:35:24] Hello World"
}
```