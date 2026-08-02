# Project 4: Hex Dump Utility with Memory Address Mapping

## Technical Objective
Implements a `hexdump -C` clone in C that parses arbitrary binary files using low-level POSIX I/O, converting raw bytes into offset-annotated hex and ASCII output.

**Business Impact Summary:** Binary data — firmware images, malware samples, corrupted files — often hides its most important details (embedded payloads, header tampering, data appended past the expected EOF) below what standard file-type checks or high-level tools can see. This utility gives analysts a lightweight, dependency-free way to inspect any binary at the byte level, including on hardened or minimal systems where standard tools may not be installed.

## The "Why": Engineering Value & Threat Impact
*   **Operational Risk / Threat Model:** Malware and tampered binaries frequently conceal shellcode, magic-byte mismatches, or trailing hidden data that never surfaces in a file-type or checksum check — only raw byte inspection reveals it.
*   **Engineering Mastery:** Demonstrates direct control over POSIX file descriptors and unbuffered I/O (`open`/`read`/`close`) rather than relying on `stdio` abstractions, plus precise offset tracking and byte-to-ASCII mapping with zero dynamic memory allocation.
*   **Defensive Utility:** Gives incident responders and forensic analysts a portable, single-file tool for byte-level inspection on systems where `hexdump`/`xxd` aren't guaranteed to be present, such as minimal containers or stripped-down endpoints.

## Architecture & System Boundary
*   **Language & Toolchain:** C (C11) / GCC, compiled with `-Wall -Wextra -Wpedantic`
*   **Operating System Focus:** POSIX-compliant systems (Linux syscalls); developed and tested on Kali Linux
*   **Core APIs/Primitives Used:** `open()`, `read()`, `close()` — no `stdio` buffering, no heap allocation

## Technical Execution (What & How)
*   **Low-level file I/O:** Reads the file in fixed 16-byte chunks directly via `read()` into a stack buffer, avoiding `fopen`/`fread` so byte handling stays explicit and unbuffered.
*   **Offset & column formatting:** Tracks the running byte offset manually across reads and renders each row as an 8-digit hex offset followed by 16 hex bytes split into two 8-byte groups, matching `hexdump -C` convention.
*   **ASCII mapping & edge cases:** Maps each byte to its printable ASCII character (`0x20`–`0x7e`) or `.` otherwise, and pads the final partial row so columns stay aligned without special-casing empty files.

## How to Build & Run Locally
```bash
gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -o hexdump src/hexdump.c
./hexdump <file>
```

> **Note on reference tools:** `hexdump`/`xxd` are handy for spot-checking output but aren't guaranteed to be on every system (e.g., minimal Docker images). Kali Linux ships both by default. On stripped-down environments, install via `apt install bsdmainutils` (hexdump) or `apt install xxd`. This project has no runtime dependency on either — they're only useful as an optional manual cross-check.
