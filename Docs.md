# Voo Docs

(Coming soon...)

```
.-----------------------------------------.
| Instruction Formats                     |
|-----------------------------------------|
| O | oooooooo iiiiiiii iiiiiiii iiiiiiii |
| A | oooooooo aaaaiiii iiiiiiii iiiiiiii |
| B | oooooooo aaaabbbb iiiiiiii iiiiiiii |
| C | oooooooo aaaabbbb cccciiii iiiiiiii |
'-----------------------------------------'

.---------------.
| Conditions    |
|---------------|
| 00 | c/nae/b  |
| 01 | nc/ae/nb |
| 02 | p        |
| 03 | np       |
| 04 | z        |
| 05 | nz       |
| 06 | s        |
| 07 | ns       |
| 08 | o        |
| 09 | no       |
| 0A | a        |
| 0B | na       |
| 0C | g        |
| 0D | le       |
| 0E | ge       |
| 0F | l        |
'---------------'

.------------------------------------------.
| Instructions                             |
|------------------------------------------|
| 00 | B | add  byte r(a), r(b), i         | r(a): r(a) + r(b) + i
| 01 | B | add  half r(a), r(b), i         | r(a): r(a) + r(b) + i
| 02 | B | add  word r(a), r(b), i         | r(a): r(a) + r(b) + i
| 03 | B | add  long r(a), r(b), i         | r(a): r(a) + r(b) + i
| 04 | B | sub  byte r(a), r(b), i         | r(a): r(a) - r(b) - i
| 05 | B | sub  half r(a), r(b), i         | r(a): r(a) - r(b) - i
| 06 | B | sub  word r(a), r(b), i         | r(a): r(a) - r(b) - i
| 07 | B | sub  long r(a), r(b), i         | r(a): r(a) - r(b) - i
| 08 | B | xhg       s(a), s(b)            | tmp: s(a); s(a): s(b); s(b): tmp
| 09 | B | mov       s(a), s(b)            | s(a): s(b)
| 0A | B | mov       s(a), r(b)            | s(a): r(b)
| 0B | B | mov       r(a), s(b)            | r(a): s(b)
| 0C | A | psh       s(a)                  | push(s(a))
| 0D | A | pop       s(a)                  | pop(s(a))
| 0E | O | nop                             |
| 0F | O | hlt                             | rFLAGS: rFLAGS & ~1
| 10 | A | not  byte r(a), i               | r(a): ~r(a) + i
| 11 | A | not  half r(a), i               | r(a): ~r(a) + i
| 12 | A | not  word r(a), i               | r(a): ~r(a) + i
| 13 | A | not  long r(a), i               | r(a): ~r(a) + i
| 14 | A | neg  byte r(a), i               | r(a): ~r(a) + 1 + i
| 15 | A | neg  half r(a), i               | r(a): ~r(a) + 1 + i
| 16 | A | neg  word r(a), i               | r(a): ~r(a) + 1 + i
| 17 | A | neg  long r(a), i               | r(a): ~r(a) + 1 + i
| 18 | B | adc  byte r(a), r(b), i         | r(a): r(a) + r(b) + i + C
| 19 | B | adc  half r(a), r(b), i         | r(a): r(a) + r(b) + i + C
| 1A | B | adc  word r(a), r(b), i         | r(a): r(a) + r(b) + i + C
| 1B | B | adc  long r(a), r(b), i         | r(a): r(a) + r(b) + i + C
| 1C | B | sbb  byte r(a), r(b), i         | r(a): r(a) - r(b) - i - C
| 1D | B | sbb  half r(a), r(b), i         | r(a): r(a) - r(b) - i - C
| 1E | B | sbb  word r(a), r(b), i         | r(a): r(a) - r(b) - i - C
| 1F | B | sbb  long r(a), r(b), i         | r(a): r(a) - r(b) - i - C
| 20 | B | and  byte r(a), r(b)            | r(a): r(a) & r(b)
| 21 | B | and  half r(a), r(b)            | r(a): r(a) & r(b)
| 22 | B | and  word r(a), r(b)            | r(a): r(a) & r(b)
| 23 | B | and  long r(a), r(b)            | r(a): r(a) & r(b)
| 24 | B | or   byte r(a), r(b)            | r(a): r(a) | r(b)
| 25 | B | or   half r(a), r(b)            | r(a): r(a) | r(b)
| 26 | B | or   word r(a), r(b)            | r(a): r(a) | r(b)
| 27 | B | or   long r(a), r(b)            | r(a): r(a) | r(b)
| 28 | B | xor  byte r(a), r(b)            | r(a): r(a) ^ r(b)
| 29 | B | xor  half r(a), r(b)            | r(a): r(a) ^ r(b)
| 2A | B | xor  word r(a), r(b)            | r(a): r(a) ^ r(b)
| 2B | B | xor  long r(a), r(b)            | r(a): r(a) ^ r(b)
| 2C | B | shl  byte r(a), r(b), i         | r(a): r(a) << (r(b) + i)
| 2D | B | shl  half r(a), r(b), i         | r(a): r(a) << (r(b) + i)
| 2E | B | shl  word r(a), r(b), i         | r(a): r(a) << (r(b) + i)
| 2F | B | shl  long r(a), r(b), i         | r(a): r(a) << (r(b) + i)
| 30 | B | shr  byte r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 31 | B | shr  half r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 32 | B | shr  word r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 33 | B | shr  long r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 34 | B | sar  byte r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 35 | B | sar  half r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 36 | B | sar  word r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 37 | B | sar  long r(a), r(b), i         | r(a): r(a) >> (r(b) + i)
| 38 | B | tst  byte r(a), r(b)            | tmp: r(a) & r(b)
| 39 | B | tst  half r(a), r(b)            | tmp: r(a) & r(b)
| 3A | B | tst  word r(a), r(b)            | tmp: r(a) & r(b)
| 3B | B | tst  long r(a), r(b)            | tmp: r(a) & r(b)
| 3C | B | cmp  byte r(a), r(b)            | tmp: r(a) - r(b)
| 3D | B | cmp  half r(a), r(b)            | tmp: r(a) - r(b)
| 3E | B | cmp  word r(a), r(b)            | tmp: r(a) - r(b)
| 3F | B | cmp  long r(a), r(b)            | tmp: r(a) - r(b)
| 40 | A | tst  byte r(a), i               | tmp: r(a) & i
| 41 | A | tst  half r(a), i               | tmp: r(a) & i
| 42 | A | tst  word r(a), i               | tmp: r(a) & i
| 43 | A | tst  long r(a), i               | tmp: r(a) & i
| 44 | A | tst  byte r(a), sext i          | tmp: r(a) & sext(i)
| 45 | A | tst  half r(a), sext i          | tmp: r(a) & sext(i)
| 46 | A | tst  word r(a), sext i          | tmp: r(a) & sext(i)
| 47 | A | tst  long r(a), sext i          | tmp: r(a) & sext(i)
| 48 | A | cmp  byte r(a), i               | tmp: r(a) - i
| 49 | A | cmp  half r(a), i               | tmp: r(a) - i
| 4A | A | cmp  word r(a), i               | tmp: r(a) - i
| 4B | A | cmp  long r(a), i               | tmp: r(a) - i
| 4C | A | cmp  byte r(a), sext i          | tmp: r(a) - sext(i)
| 4D | A | cmp  half r(a), sext i          | tmp: r(a) - sext(i)
| 4E | A | cmp  word r(a), sext i          | tmp: r(a) - sext(i)
| 4F | A | cmp  long r(a), sext i          | tmp: r(a) - sext(i)
| 50 | B | xhg  byte r(a), r(b)            | tmp: r(a); r(a): r(b); r(b): tmp
| 51 | B | xhg  half r(a), r(b)            | tmp: r(a); r(a): r(b); r(b): tmp
| 52 | B | xhg  word r(a), r(b)            | tmp: r(a); r(a): r(b); r(b): tmp
| 53 | B | xhg  long r(a), r(b)            | tmp: r(a); r(a): r(b); r(b): tmp
| 54 | B | mov  byte r(a), r(b)            | r(a): r(b)
| 55 | B | mov  half r(a), r(b)            | r(a): r(b)
| 56 | B | mov  word r(a), r(b)            | r(a): r(b)
| 57 | B | mov  long r(a), r(b)            | r(a): r(b)
| 58 | B | mov  half r(a), byte r(b)       | r(a): r(b)
| 59 | B | mov  word r(a), byte r(b)       | r(a): r(b)
| 5A | B | mov  word r(a), half r(b)       | r(a): r(b)
| 5B | B | mov  long r(a), byte r(b)       | r(a): r(b)
| 5C | B | mov  long r(a), half r(b)       | r(a): r(b)
| 5D | B | mov  long r(a), word r(b)       | r(a): r(b)
| 5E | O | int       i                     | push(rFLAGS); push(rCODE); push(rIP); rCODE, rIP: IVT[i]; rFLAGS: (rFLAGS & ~(I|IOPL)) | T 
| 5F | O | iret                            | pop(rIP); pop(rCODE); pop(rFLAGS)
| 60 | O | cl{i}                           | rFLAGS: rFLAGS & ~i (1, I, V, IOPL only when IOPL is 0)
| 61 | O | st{i}                           | rFLAGS: rFLAGS | i (1, I, V, IOPL only when IOPL is 0)
| 62 | A | set{b}    r(a)                  | r(a): 0 or 1 depending on the condition status
| 63 | C | lea       r(a), [s(c):r(b) + i] | r(a): r(b) + i
| 64 | B | jmp       s(b):r(a) + i         | rCODE: s(b); rIP: (r(a) + i) << 2
| 65 | A | jmp       r(a) + i              | rIP: rIP + (r(a) + i) << 2
| 66 | B | call      s(b):r(a) + i         | push(rCODE); push(rIP); rCODE: s(b), rIP: (r(a) + i) << 2
| 67 | A | call      r(a) + i              | push(rIP); rIP: rIP + (r(a) + i) << 2
| 68 | O | ret       r(a) + i (abs)        | pop(rIP); pop(rCODE)
| 69 | O | ret       r(a) + i (rel)        | pop(rIP)
| 6A | C | j{c}      s(b):r(a) + i         | if the condition is set then rCODE: s(b); rIP: (r(a) + i) << 2
| 6B | B | j{b}      r(a) + i              | if the condition is set then rIP: rIP + (r(a) + i) << 2
| 6C | A | pop  byte r(a), i               | for i do pop(r(a))
| 6D | A | pop  half r(a), i               | for i do pop(r(a))
| 6E | A | pop  word r(a), i               | for i do pop(r(a))
| 6F | A | pop  long r(a), i               | for i do pop(r(a))
| 70 | A | psh  byte r(a), i               | for i do push(r(a))
| 71 | A | psh  half r(a), i               | for i do push(r(a))
| 72 | A | psh  word r(a), i               | for i do push(r(a))
| 73 | A | psh  long r(a), i               | for i do push(r(a))
| 74 | A | psh  byte i                     | push(i)
| 75 | A | psh  half i                     | push(i)
| 76 | A | psh  word i                     | push(i)
| 77 | A | psh  long i                     | push(i)
| 78 | A | psh  byte sext i                | push(sext(i))
| 79 | A | psh  half sext i                | push(sext(i))
| 7A | A | psh  word sext i                | push(sext(i))
| 7B | A | psh  long sext i                | push(sext(i))
| 7C | O | pshf                            | push(rFLAGS)
| 7D | O | popf                            | pop(rFLAGS)
| 7E | A | psh       r(16 + a)             | push(r(16 + a))
| 7F | A | pop       r(16 + a)             | pop(r(16 + a))
| 80 | A | mov  byte r(a), i               | r(a): i
| 81 | A | mov  half r(a), i               | r(a): i
| 82 | A | mov  word r(a), i               | r(a): i
| 83 | A | mov  long r(a), i               | r(a): i
| 84 | A | mov  byte r(a), sext i          | r(a): sext(i)
| 85 | A | mov  half r(a), sext i          | r(a): sext(i)
| 86 | A | mov  word r(a), sext i          | r(a): sext(i)
| 87 | A | mov  long r(a), sext i          | r(a): sext(i)
| 88 | C | sto  byte [s(c):r(b) + i], r(a) | [s(c):r(b) + i]: r(a)
| 89 | C | sto  half [s(c):r(b) + i], r(a) | [s(c):r(b) + i]: r(a)
| 8A | C | sto  word [s(c):r(b) + i], r(a) | [s(c):r(b) + i]: r(a)
| 8B | C | sto  long [s(c):r(b) + i], r(a) | [s(c):r(b) + i]: r(a)
| 8C | C | lod  byte r(a), [s(c):r(b) + i] | r(a): [s(c):r(b) + i]
| 8D | C | lod  half r(a), [s(c):r(b) + i] | r(a): [s(c):r(b) + i]
| 8E | C | lod  word r(a), [s(c):r(b) + i] | r(a): [s(c):r(b) + i]
| 8F | C | lod  long r(a), [s(c):r(b) + i] | r(a): [s(c):r(b) + i]
| 90 | A | mul  byte r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 91 | A | mul  half r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 92 | A | mul  word r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 93 | A | mul  long r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 94 | A | div  byte r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 95 | A | div  half r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 96 | A | div  word r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 97 | A | div  long r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 98 | A | imul byte r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 99 | A | imul half r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 9A | A | imul word r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 9B | A | imul long r(2):r(0), r(a)       | r2, r0: r0 * r(a)
| 9C | A | idiv byte r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 9D | A | idiv half r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 9E | A | idiv word r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| 9F | A | idiv long r(2):r(0), r(a)       | r2: r0 MOD r(a); r0: r0 / r(a)
| A0 | - |                                 |
| A1 | - |                                 |
| A2 | - |                                 |
| A3 | - |                                 |
| A4 | - |                                 |
| A5 | - |                                 |
| A6 | - |                                 |
| A7 | - |                                 |
| A8 | - |                                 |
| A9 | - |                                 |
| AA | - |                                 |
| AB | - |                                 |
| AC | - |                                 |
| AD | - |                                 |
| AE | - |                                 |
| AF | - |                                 |
| B0 | A | loop      r(a) + i (rel)        | if r1 != 0 then --r1; jump
| B1 | B | loop{b}   r(a) + i (rel)        | if r1 != 0 and the condition is set then --r1, jump
| B2 | A | enter     r(a) + i              | enter the frame: push(rBP); rBP: rSP; rSP: rSP - (r(a) + i)
| B3 | O | leave                           | leave the frame: rSP: rBP; pop(rBP)
| B4 | - |                                 |
| B5 | - |                                 |
| B6 | - |                                 |
| B7 | - |                                 |
| B8 | - |                                 |
| B9 | - |                                 |
| BA | - |                                 |
| BB | - |                                 |
| BC | - |                                 |
| BD | - |                                 |
| BE | - |                                 |
| BF | - |                                 |
| C0 | - |                                 |
| C1 | - |                                 |
| C2 | - |                                 |
| C3 | - |                                 |
| C4 | - |                                 |
| C5 | - |                                 |
| C6 | - |                                 |
| C7 | - |                                 |
| C8 | - |                                 |
| C9 | - |                                 |
| CA | - |                                 |
| CB | - |                                 |
| CC | - |                                 |
| CD | - |                                 |
| CE | - |                                 |
| CF | - |                                 |
| D0 | O | out  byte i, r(2)               | out(port: i, data: r2)
| D1 | O | out  half i, r(2)               | out(port: i, data: r2)
| D2 | O | out  word i, r(2)               | out(port: i, data: r2)
| D3 | O | out  long i, r(2)               | out(port: i, data: r2)
| D4 | O | out  byte r(0), r(2)            | out(port: r0, data: r2)
| D5 | O | out  half r(0), r(2)            | out(port: r0, data: r2)
| D6 | O | out  word r(0), r(2)            | out(port: r0, data: r2)
| D7 | O | out  long r(0), r(2)            | out(port: r0, data: r2)
| D8 | O | in   byte i, r(2)               | in(port: i, data: r2)
| D9 | O | in   half i, r(2)               | in(port: i, data: r2)
| DA | O | in   word i, r(2)               | in(port: i, data: r2)
| DB | O | in   long i, r(2)               | in(port: i, data: r2)
| DC | O | in   byte r(0), r(2)            | in(port: r0, data: r2)
| DD | O | in   half r(0), r(2)            | in(port: r0, data: r2)
| DE | O | in   word r(0), r(2)            | in(port: r0, data: r2)
| DF | O | in   long r(0), r(2)            | in(port: r0, data: r2)
| E0 | - |                                 |
| E1 | - |                                 |
| E2 | - |                                 |
| E3 | - |                                 |
| E4 | - |                                 |
| E5 | - |                                 |
| E6 | - |                                 |
| E7 | - |                                 |
| E8 | - |                                 |
| E9 | - |                                 |
| EA | - |                                 |
| EB | - |                                 |
| EC | - |                                 |
| ED | - |                                 |
| EE | - |                                 |
| EF | - |                                 |
| F0 | - |                                 |
| F1 | - |                                 |
| F2 | - |                                 |
| F3 | - |                                 |
| F4 | - |                                 |
| F5 | - |                                 |
| F6 | - |                                 |
| F7 | - |                                 |
| F8 | - |                                 |
| F9 | - |                                 |
| FA | - |                                 |
| FB | - |                                 |
| FC | - |                                 |
| FD | - |                                 |
| FE | - |                                 |
| FF | - |                                 |
'------------------------------------------'

.-------------------------------------.
| Ports                   | R | W | A |
|-------------------------|-----------|
| 00 | stdin  | -         | 1 | 0 | 0 |
| 01 | stdout | -         | 0 | 1 | 0 |
| 02 | stderr | -         | 0 | 1 | 0 |
| 03 | voolog | `.voolog` | 1 | 0 | 1 |
| 04 | voodsk | `.voodsk` | 1 | 0 | 1 |
'-------------------------------------'

.-------------------------------------.
| Registers                           |
|-------------------------------------|
| r0...13  : 0                        |
| rBP      : 0x1000                   |
| rSP      : 0x1000                   |
| rFLAGS   : 1                        |
| rIP      : 0x1000                   |
| rCLOCK   : 0                        |
| r19...23 : 0                        |
|-------------------------------------|
| s0...15  : 0                        |
'-------------------------------------'

```
