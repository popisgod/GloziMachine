#include "lc3_config.h"

int main(int argc, const char* argv[]) {
 
    if (argc < 2)
    {
        /* show usage string */
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int j = 1; j < argc; ++j)
    {
        if (!read_image(argv[j]))
        {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }


    /* since exactly one condition flag should be set at any given time, set the Z flag */
    reg[R_COND] = FL_ZRO;

    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;
    
    int running = 1;
    while (running)
    {
        /* FETCH */
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >> 12;

        switch (op)
        {
            case OP_ADD:
            {
                /* destination register (DR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* first operand (SR1) */
                uint16_t r1 = (instr >> 6) & 0x7;
                /* whether we are in immediate mode */
                uint16_t imm_flag = (instr >> 5) & 0x1;

                if (imm_flag)
                {
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                    reg[r0] = reg[r1] + imm5;
                }
                else
                {
                    uint16_t r2 = instr & 0x7;
                    reg[r0] = reg[r1] + reg[r2];
                }

                update_flags(r0);
                break;
            }
            case OP_AND:
            {
                /* destination register (DR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* first operand (SR1) */
                uint16_t r1 = (instr >> 6) & 0x7;
                /* whether we are in immediate mode */
                uint16_t imm_flag = (instr >> 5) & 0x1;

                if (imm_flag)
                {
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                    reg[r0] = reg[r1] & imm5;
                }
                else
                {
                    uint16_t r2 = instr & 0x7;
                    reg[r0] = reg[r1] & reg[r2];
                }

                update_flags(r0);
                break;
            }
            case OP_NOT:
            {
                /* destination register (DR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* source register (SR) */
                uint16_t r1 = (instr >> 6) & 0x7;

                reg[r0] = ~reg[r1];

                update_flags(r0); 
                break; 
            }
            case OP_BR:
            {
                uint16_t flagN = (instr >> 11) & 1;
                uint16_t flagZ = (instr >> 10) & 1;
                uint16_t flagP = (instr >> 9) & 1;
                /* PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                if (reg[R_COND] & (flagN) || reg[R_COND] & (flagZ) || reg[R_COND] & (flagP))
                {
                    reg[R_PC] = reg[R_pC] + pc_offset;
                }

                break;
            }
            case OP_JMP:
            {
                /* base register (baseR) */
                uint16_t r0 = (instr >> 9) & 0x7;

                /* load program counter (PC) with base register (baseR) */
                reg[R_PC] = reg[r0];

                break;
            }
            case OP_JSR:
            {
                /* incremented program counter (PC) saved in register 7 (R7) */
                reg[R_R7] = reg[R_PC];

                if ((instr >> 11) & 1)
                {
                    reg[R_PC] = reg[R_PC] + sign_extend(instr & 0x7FF, 11);
                }
                else
                {
                    reg[R_PC] = reg[(instr >> 6) & 0x7];
                }

                break;
            }
            case OP_LD:
            {
                /* destination register (DR) */
                uint16_t r0 = reg[(instr >> 9) & 0x7];
                /* PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                r0 = mem_read(reg[R_PC] + pc_offset);

                update_flags(r0); 
                break; 
            }
            case OP_LDI:
            {
                /* destination register (DR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                /* add pc_offset to the current PC, look at that memory location to get the final address */
                reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
                
                update_flags(r0);
                break;
            }
            case OP_LDR:
            {
                /* destination register (DR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* base register (BaseR) */
                uint16_t r1 = (instr >> 6) & 0x7; 
                /* offset 6*/
                uint16_t offset = sign_extend(instr & 0x1F, 5); 

                reg[r0] = mem_read(reg[r1] + offset); 

                update_flags(r0);
                break; 
            }
            case OP_LEA:
            {
                /* destination register (DR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                
                reg[r0] = reg[R_PC] + pc_offset; 

                update_flags(r0);
                break;
            }
            case OP_ST:
            {
                /* source register (SR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                memory[reg[R_PC] + pc_offset] = reg[r0];

                break; 
            }
            case OP_STI:
            {
                /* source register (SR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9);
                memory[memory[reg[R_PC] + pc_offset]] = reg[r0];

                break; 
            }
            case OP_STR:
            {
                /* source register (SR) */
                uint16_t r0 = (instr >> 9) & 0x7;
                /* base register (BaseR) */
                uint16_t r1 = (instr >> 6) & 0x7;
                /* offset 6*/
                uint16_t offset = sign_extend(instr & 0x1F, 5);
                memory[reg[r1] + offset] = reg[r0];

                break;
            }
            case OP_TRAP:
            {
                reg[R_PC] = reg[R_R7];
                PC = memory[instr & 0xFF];

                break;
            }
            case OP_RES:
            case OP_RTI:
            default:
            {

                break;
            }
                
        }
    }
    @{Shutdown}
}
