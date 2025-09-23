# Security Vulnerabilities Introduced for Testing

This document lists the security vulnerabilities intentionally introduced for security testing purposes.

## 1. Path Traversal in File Operations
- **Location**: src/argument.cpp (save_argument, load_argument functions)
- **Risk**: High - Allows reading/writing arbitrary files
- **Exploit**: Use "../../../etc/passwd" in filename parameters
- **Details**: No path validation or sanitization is performed on filenames passed to save_argument() and load_argument() functions, allowing attackers to access files outside the intended directory using directory traversal sequences.

## 2. Buffer Overflow in C API
- **Location**: src/api/api.cpp (migraphx_argument_buffer function)  
- **Risk**: High - Direct memory access without bounds checking
- **Exploit**: Pass oversized buffers to trigger overflow
- **Details**: The migraphx_argument_buffer function returns a direct pointer to internal buffer data without any bounds checking or size validation, potentially allowing buffer overflow attacks.

## 3. Command Injection in Driver
- **Location**: src/driver/main.cpp (file parameter parsing)
- **Risk**: Medium - Unsafe filename handling
- **Exploit**: Use filenames like "file.onnx; malicious_command"
- **Details**: File existence validation was removed from command-line argument parsing, allowing potentially malicious filenames to be processed without proper sanitization.

## 4. Directory Traversal in Python Converter
- **Location**: tools/converters/mxr_to_onnx.py (file path handling)
- **Risk**: Medium - Access files outside intended directory
- **Exploit**: Use "../../../sensitive_file.mxr" as filename
- **Details**: The Python converter processes filenames from directory listings without validating that the resulting paths stay within the intended directory structure.

## 5. Unsafe File Operations
- **Location**: src/file_buffer.cpp (write_buffer function)
- **Risk**: Medium - No size limits or path validation
- **Exploit**: Write large files or to system locations
- **Details**: File write operations lack size limits and path validation, potentially allowing denial-of-service attacks through large file writes or unauthorized access to system locations.

## Exploitation Examples

### Path Traversal Example
```cpp
// Vulnerable usage - can access files outside intended directory
migraphx::save_argument(arg, "../../../etc/passwd");
migraphx::load_argument("../../../../home/user/.ssh/id_rsa");
```

### Buffer Overflow Example
```c
// Vulnerable usage - direct pointer access without bounds
char* buffer;
migraphx_argument_buffer(&buffer, argument);
// No size information provided - potential for overflow
```

### Command Injection Example
```bash
# Vulnerable usage - malicious filename processing
./migraphx-driver "model.onnx; rm -rf /"
```

## Remediation

These vulnerabilities should be addressed by:
1. Adding path validation and sanitization
2. Implementing bounds checking for buffer operations
3. Sanitizing command-line inputs
4. Adding size limits and validation for file operations
5. Using secure file handling practices

**WARNING**: These vulnerabilities are intentionally introduced for testing purposes and should be removed before any production deployment.
