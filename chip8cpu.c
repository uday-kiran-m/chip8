#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "chip8gui.h"

uint16_t PROGRAM_COUNTER = 0x200;
uint8_t REGISTERS[16]; // 16 8 bit registers V0-VF use indexing
uint16_t REG_I;
uint16_t STACK[12];
extern unsigned char *MEMptr;
int STACK_index = -1;



int stackPUSH(int PC)
{
    if (STACK_index < 11)
    {
        STACK_index++;
        STACK[STACK_index] = PC;
        return 0;
    }
    else
    {
        return -1;
    }
}

int stackPOP()
{
    if (STACK_index < 0)
    {
        return -1;
    }
    return STACK[STACK_index--];
}

void op_0(int opcode)
{
    // 0NNN instruction execute machine code at NNN
    if (opcode >> 4 != 0xE)
    {
        PROGRAM_COUNTER = opcode - 2;
    }
    else
    {
        // 00EE instruction return from subroutine
        if (opcode - 0xE0 == 0xE)
        {
            PROGRAM_COUNTER = stackPOP();
            if (PROGRAM_COUNTER == -1)
            {
                printf("ERROR Stack underflow\n");
                return;
            }
        }
        // 00E0 instruction clear the screen
        else
        {
            clearscr();
        }
    }
}
void op_1(int opcode)
{
    // 1NNN jump to NNN address
    PROGRAM_COUNTER = (opcode - 0x1000) - 2;
}

void op_2(int opcode)
{
    // 2NNN execute subroutine at NNN
    stackPUSH(PROGRAM_COUNTER);
    PROGRAM_COUNTER = (opcode - 0x2000) - 2;
}

void op_3(int opcode)
{
    // 3XNN check if reg VX == NN and skip following instruction if so
    int v_reg = (opcode & 0x0F00) >> 8;
    int val = opcode - 0x3000 - (v_reg << 8);
    if (REGISTERS[v_reg] == val)
    {
        PROGRAM_COUNTER += 2; // skipping the next instruction
    }
}

void op_4(int opcode)
{
    // 4XNN check if reg VX != NN and skip following instruction if so
    int v_reg = (opcode & 0x0F00) >> 8;
    int val = opcode - 0x4000 - (v_reg << 8);
    if (REGISTERS[v_reg] != val)
    {
        PROGRAM_COUNTER += 2; // skipping the next instruction
    }
}

void op_5(int opcode)
{
    // 5XY0 skip next instruction if VX == VY
    int vx_reg = (opcode & 0x0F00) >> 8;
    int vy_reg = (opcode & 0x00F0) >> 4;
    switch(opcode & 0x000F){
        case 0:
            if (REGISTERS[vx_reg] == REGISTERS[vy_reg])
            {
                PROGRAM_COUNTER += 2; // skipping following instruction
            }
            break;
        default:
        printf("Unknown opcode %x\n",opcode);
    }
}

void op_6(int opcode)
{
    // 6XNN store NN in VX
    int v_reg = (opcode & 0x0F00) >> 8;
    int val = opcode & 0x00FF;
    REGISTERS[v_reg] = val;
}

void op_7(int opcode)
{
    // 7XNN add NN in VX
    int v_reg = (opcode & 0x0F00) >> 8;
    int val = opcode & 0x00FF;
    REGISTERS[v_reg] += val;
}

void op_8(int opcode)
{ // 8XYN getting reg VX and VY for further N
    int vx_reg = (opcode & 0x0F00) >> 8;
    int vy_reg = (opcode & 0x00F0) >> 4;
    int vf = 0;
    switch (opcode & 0x000F)
    {
    case 0:
        // 8XY0 put value of VY in VX
        REGISTERS[vx_reg] = REGISTERS[vy_reg];
        break;
    case 1:
        // 8XY1 VX = VX or VY
        REGISTERS[vx_reg] = REGISTERS[vx_reg] | REGISTERS[vy_reg];
        break;
    case 2:
        // 8XY2 VX = VX and VY
        REGISTERS[vx_reg] = REGISTERS[vx_reg] & REGISTERS[vy_reg];
        break;
    case 3:
        // 8XY3 VX = VX xor VY
        REGISTERS[vx_reg] = REGISTERS[vx_reg] ^ REGISTERS[vy_reg];
        break;
    case 4:
        // 8XY4 VX = VX + VY , VF=1 if carry
        int sum = REGISTERS[vx_reg] + REGISTERS[vy_reg];
        REGISTERS[vx_reg] = sum & 0xFF;
        if (sum > 255)
        {
            REGISTERS[0xF] = 1;
        }
        else
        {
            REGISTERS[0xF] = 0;
        }
        
        break;
    case 5:
        // 8XY5 VX = VX - VY, VF=1 if no borrow
        int t1 = REGISTERS[vx_reg];
        int t2 = REGISTERS[vy_reg];
        
        if (REGISTERS[vx_reg] >= REGISTERS[vy_reg])
        {
            vf = 1;
        }
        else
        {
            vf = 0;
        }
        REGISTERS[vx_reg] = REGISTERS[vx_reg] - REGISTERS[vy_reg];
        REGISTERS[0xF] = vf;
        break;
    case 6:
        // 8XY6 VX = VY >> 1, VF = least sign bit of VY before shift
        REGISTERS[0xF] = REGISTERS[vy_reg] & 0x01; // capturing last bit
        REGISTERS[vx_reg] = REGISTERS[vy_reg] >> 1;
        break;
    case 7:
        // 8XY7 set VX = VY-VX, VF=1 if no borrow
        if (REGISTERS[vy_reg] >= REGISTERS[vx_reg])
        {
            vf = 1;
        }
        else
        {
            vf = 0;
        }
        REGISTERS[vx_reg] = REGISTERS[vy_reg] - REGISTERS[vx_reg];
        REGISTERS[0xF] = vf;
        break;
    case 0xE:
        // 8XYE VX = VY << 1, VF = most sign bit of VY before shift
        vf = REGISTERS[vy_reg] & 0x80; // capturing first bit
        REGISTERS[vx_reg] = REGISTERS[vy_reg] << 1;
        REGISTERS[0xF] = vf;
        break;
    default:
        printf("Unknown op_8 code: %x\n", opcode);
    }
}

void op_9(int opcode)
{
    // 9XY0 skip next instruction if VX != VY
    int vx_reg = (opcode & 0x0F00) >> 8;
    int vy_reg = (opcode & 0x00F0) >> 4;

    switch(opcode&0x000F){
        case 0:
            if (REGISTERS[vx_reg] != REGISTERS[vy_reg])
            {
                PROGRAM_COUNTER += 2;
            }
            break;
        default:
        printf("unknown opcode %x\n",opcode);
    }
}

void op_A(int opcode)
{
    // ANNN store NNN in reg I
    REG_I = opcode & 0x0FFF;
}

void op_B(int opcode)
{
    // BNNN jump to NNN + V0
    PROGRAM_COUNTER = (opcode & 0x0FFF) + REGISTERS[0x0] - 2;
}

void op_C(int opcode)
{
    // CXNN set VX = random with mask of NN?
    int v_reg = (opcode & 0x0F00) >> 8;
    int mask = (opcode & 0x00FF);
    REGISTERS[v_reg] = rand() & mask;
}

void op_D(int opcode)
{
    // DXYN draw at VX,VY with Nbytes of sprite data starting from reg I, VF=1 if any pixels changed to unset
    int vx_reg = (opcode & 0x0F00) >> 8;
    int vy_reg = (opcode & 0x00F0) >> 4;
    int nbytes = (opcode & 0x000F);
    drawchar(REG_I,REGISTERS[vx_reg],REGISTERS[vy_reg],nbytes);
    // clearscr();
}

void op_E(int opcode)
{
    
    int v_reg = (opcode % 0x0F00) >> 8;
    switch(opcode&0x00FF){
        case 0x9E:
            // EX9E skip following instruction if hex key pressed value == VX

            break;
        case 0xA1:
            // EXA1 skip following instruction if hex key pressed value != VX

            break;
        default:
            printf("Unknown opcode %x\n",opcode);
    }
}

void op_F(int opcode)
{
    int v_reg = (opcode % 0x0F00) >> 8;
    switch(opcode&0x00FF){
        case 0x07:
            // FX07 put delay timer value in VX

            break;
        case 0x0A:
            // FX0A wait for keypress and store in VX

            break;
        case 0x15:
            // FX15 delay timer = VX

            break;
        case 0x18:
            // FX18 sound timer = VX

            break;
        case 0x1E:
            // FX1E reg I = I + VX
            REG_I = REG_I + REGISTERS[v_reg];
            break;
        case 0x29:
            // FX29 set I to memaddress of character stored in VX
            REG_I = 0x50 + REGISTERS[v_reg];

            break;
        case 0x33:
            // FX33 set bcd of VX at I,I+1,I+2
            //                       Z, Y, X
            uint8_t number = REGISTERS[v_reg];
            uint8_t X = (number/1) %10;
            uint8_t Y = (number/10)%10;
            uint8_t Z = (number/100)%10;
            
            MEMptr[REG_I] = Z;
            MEMptr[REG_I+1] = Y;
            MEMptr[REG_I+2] = X;

            break;
        case 0x55:
            // FX55 store V0 to VX at I to I+X+1
            for(int i = 0;i<=v_reg;i++){
                MEMptr[REG_I+i] = REGISTERS[i];
            }
            break;
        case 0x65:
            // FX65 get V0 to VX from I to I+X+1
            for(int i = 0;i<=v_reg;i++){
                REGISTERS[i]= MEMptr[REG_I+i];
            }
            break;
        default:
            printf("Unknown opcode %x\n",opcode);
    }
}

void cpuRunner(int ROMsize)
{
    srand(time(NULL));
    while (PROGRAM_COUNTER < (ROMsize + 0x200))
    {
        // printf("%x", PROGRAM_COUNTER);
        int ev = getevent();
        if( ev!= -1){
            if (ev == 0){
                break;
            }

        }
        int opcode = MEMptr[PROGRAM_COUNTER] << 8 | MEMptr[PROGRAM_COUNTER + 1];
        printf("Current opcode: %x\n", opcode);
        switch (opcode >> 12)
        {
        case 0:
            op_0(opcode);
            // printf("Exec 0 code\n");
            break;
        case 1:
            op_1(opcode);
            // printf("Exec 1 code\n");
            break;
        case 2:
            op_2(opcode);
            // printf("Exec 2 code\n");
            break;
        case 3:
            op_3(opcode);
            // printf("Exec 3 code\n");
            break;
        case 4:
            op_4(opcode);
            // printf("Exec 4 code\n");
            break;
        case 5:
            op_5(opcode);
            // printf("Exec 5 code\n");
            break;
        case 6:
            op_6(opcode);
            // printf("Exec 6 code\n");
            break;
        case 7:
            op_7(opcode);
            // printf("Exec 7 code\n");
            break;
        case 8:
            op_8(opcode);
            // printf("Exec 8 code\n");
            break;
        case 9:
            op_9(opcode);
            // printf("Exec 9 code\n");
            break;
        case 0xA:
            op_A(opcode);
            // printf("Exec A code\n");
            break;
        case 0xB:
            op_B(opcode);
            // printf("Exec B code\n");
            break;
        case 0xC:
            op_C(opcode);
            // printf("Exec C code\n");
            break;
        case 0xD:
            op_D(opcode);
            // printf("Exec D code\n");
            break;
        case 0xE:
            op_E(opcode);
            // printf("Exec E code\n");
            break;
        case 0xF:
            op_F(opcode);
            // printf("Exec F code\n");
            break;
        default:
            printf("Unknown opcodeL %x", opcode);
            break;
        }
        PROGRAM_COUNTER += 2;
    }
}
