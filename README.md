# 16-bit CPU Emulator in C

A small CPU emulator written in C to learn how a processor works: the fetch-decode-execute cycle, registers, flags, instruction encoding, and raw memory handling with fixed-width types and pointers.

It emulates a simple 16-bit CPU with 8 registers and 64 KB of memory, based on a basic 4-byte-instruction ISA from a YouTube tutorial (add the link here).

## Status

Working minimal core. Three instructions are implemented so far:

| Instruction | Opcode | Effect |
|---|---|---|
| `LD Rn, imm` | `0x00` | `Rn = imm` |
| `ADD Rn, imm` | `0x40` | `Rn = Rn + imm`, sets Z, N, C |
| `HALT` | `0xFE` | stops the CPU |

Any other opcode stops the CPU with a fault message that shows the opcode and its address.

## CPU model

| Part | Details |
|---|---|
| Memory | 64 KB (`uint8_t mem[65536]`), 16-bit addresses |
| Registers | 8 general-purpose 16-bit registers, `R0`-`R7` |
| Program counter | 16-bit, starts at `0x0000` |
| Flags | `Z` zero, `N` negative (bit 15 of the result), `C` carry (result did not fit in 16 bits) |
| Byte order | Big-endian for 16-bit values (high byte first) |

## Instruction format

Every instruction is exactly 4 bytes:

```
| byte 0 | byte 1   | byte 2      | byte 3      |
| opcode | register | data (high) | data (low)  |
```

For example, `LD R1, 5` is encoded as `00 01 00 05`.

## Build and run

Requires `gcc`.

```bash
gcc -Wall -Wextra -g -fsanitize=address,undefined c_emulator.c -o c_emulator
./c_emulator
```

The program currently runs a hardcoded demo in `main`:

```
LD  R1, 5
ADD R1, 3
HALT
```

Expected output:

```
PC=000C  Z=0 N=0 C=0
R0=0000  R1=0008  R2=0000  R3=0000  R4=0000  R5=0000  R6=0000  R7=0000
```

## How it works

`main` copies a program into emulated memory, then repeats `cpu_step` until the CPU halts. Each step does the following:

1. **Fetch:** read the 4 instruction bytes at the program counter and advance the counter by 4
2. **Decode:** split out the opcode, register number, and 16-bit data value
3. **Execute:** run the matching `case` in a `switch`, updating registers and flags

## Roadmap

- [ ] `SUB`, and `ADD`/`SUB`/`LD` with a register operand
- [ ] `CMP` and branches (`BRA`, `BEQ`, `BGT`, `BLT`)
- [ ] Load and store instructions (`ST`, `STL`, `STH`, `LD` from a memory address)
- [ ] `NOOP`
- [ ] Bounds checking on instruction fetch and memory access
- [ ] Load programs from a `.bin` file
- [ ] A small assembler with labels (two passes)

## Project layout

```
c_emulator.c   # the emulator
```

## What I learned

- How the fetch-decode-execute cycle works in code
- Fixed-width integer types (`uint8_t`, `uint16_t`), masking, and shifts
- Why flags exist and how carry, zero, and negative are computed
- Passing structs by pointer, `static` storage, and `memset`/`memcpy`
