# Herr Log
A single header lightweight logging library.

## Download
Simply copy paste `herr-log.hh` into you directory and import it.

## Usage
Herr Log supports the following log levels.
- Error
- Info
- Trace
- Debug
All these can be activated/deactivated independently.

### Basic usage
```cpp
#include "herrlog.hh"

int main() {
    Logger::trace("This is trace message number 1");
}
```

### Custom output stream (e.g. logfiles)
In place of log_file, any other stream can also be put.

```cpp
#include "herrlog.hh"

int main() {
    Logger::init(LogType::All, "test.log");
    Logger::trace("This is trace message number 1");
}
```

### Custom output message types
For example, only error and info message are needed.

```cpp
#include "herrlog.hh"

int main() {
    Logger::init(LogType::All, "test.log");
    Logger::trace("This is {} message number {}", "trace", 1);
}
```