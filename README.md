# Use Regex to Hide Linux Processes
## Usage

1. Edit the .c file and change the line:

```c
static const char *process_to_filter = "^evil_script\\.py$";
```

Replace "^evil_script\\.py$" with your desired regular expression pattern to filter processes.

2. Build the project:


```bash
make
```

3. Install the project:

```bash
make install
```