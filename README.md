As assembler for a custom RISC instruction set architecture.

The `SIMPLE` instruction set architecture

| Mnemonic | Opcode | Operand | Formal Specification                           | Description                                                                                            |
|----------|--------|---------|------------------------------------------------|--------------------------------------------------------------------------------------------------------|
| data     |        | value   |                                                | Reserve a memory location,<br>initialized to the value specified                                       |
| ldc      | 0      | value   | `B := A;`<br>`A := value;`                     |                                                                                                        |
| adc      | 1      | value   | `A := A + value;`                              | Add the value specified to the accumulator                                                             |
| ldl      | 2      | offset  | `B := A;`<br>`A := memory[SP + offset];`       | Load Local                                                                                             |
| stl      | 3      | offset  | `memory[SP + offset] := A;`<br>`A := B;`       | Store Local                                                                                            |
| ldnl     | 4      | offset  | `A := memory[A + offset];`                     | Load non-local                                                                                         |
| stnl     | 5      | offset  | `memory[A + offset] := B;`                     | Store non-local                                                                                        |
| add      | 6      |         | `A := B + A;`                                  | Addition                                                                                               |
| sub      | 7      |         | `A := B - A;`                                  | Subtraction                                                                                            |
| shl      | 8      |         | `A := B << A;`                                 | Shift Left                                                                                             |
| shr      | 9      |         | `A := B >> A;`                                 | Shift Right                                                                                            |
| adj      | 10     | value   | `SP := SP + value;`                            | Adjust Stack Pointer                                                                                   |
| a2sp     | 11     |         | `SP := A;`<br>`A := B;`                        | Transfer A to Stack                                                                                    |
| sp2a     | 12     |         | `B := A;`<br>`A := SP;`                        | Transfer Stack top to A                                                                                |
| call     | 13     | offset  | `B := A;`<br>`A := PC;`<br>`PC := PC + offset` | Call Procedure                                                                                         |
| return   | 14     |         | `PC := A;`<br>`A := B;`                        | Return from procedure                                                                                  |
| brz      | 15     | offset  | `if A == 0 then:`<br>    `PC := PC + offset`   | If accumulator is zero, branch to the specified offset.                                                |
| brlz     | 16     | offset  | `if A < 0:`<br>    `PC := PC + offset`         | If accumulator is less than zero, then branch to the specified offset.                                 |
| br       | 17     | offset  | `PC := PC + offset`                            | Branch to the specified offset.                                                                        |
| HALT     | 18     |         |                                                | Stops the emulator. This is not a 'real' instruction, but needed to tell your emulator when to finish. |
| SET      |        | value   |                                                | Set the label on this line to the specified value (rather than the PC).                                |