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
#include "herr-log.hh"

int main() {
    Logger* logger = new Logger(Style::All, std::cout);
    logger->trace("This is a trace message.");
    logger->debug("This is a debug message.");
    logger->info("This is an info message.");
    logger->error("This is an error message.");
}
```

### Custom output stream (e.g. logfiles)
In place of log_file, any other stream can also be put.

```cpp
#include "herr-log.hh"
#include <fstream>

int main() {
    std::ofstream log_file("example.log"); // Creating output stream as a log file
    
    Logger* logger = new Logger(Style::All, log_file); // Inserting log_file as our output stream
    logger->trace("This is a trace message.");
    logger->debug("This is a debug message.");
    logger->info("This is an info message.");
    logger->error("This is an error message.");
}
```

### Custom output message types
For example, only error and info message are needed.

```cpp
#include "herr-log.hh"
#include <fstream>

int main() {
    std::ofstream log_file("log.txt");
    
    Logger* logger = new Logger(Style::Info | Style::Error, log_file); // Only info and error logs will be shown
    logger->trace("This is a trace message.");
    logger->debug("This is a debug message.");
    logger->info("This is an info message.");
    logger->error("This is an error message.");
}
```