#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include <stdint.h>
#include "chip8cpu.h"
#include "chip8gui.h"
#define CHIP8_ROM "roms/puzzle2.ch8"


int ROM_SIZE;
unsigned char* MEMptr;

int initmem(){
    //creating memory space 0x00-0xFFF
    MEMptr = (char*)malloc(0xFFF);
    if (MEMptr == NULL){
        return -1;
    }
    return 0;
}

int loadrom(){
    FILE* chipROM = fopen(CHIP8_ROM,"r");
    if (chipROM==NULL){
        printf("cannot read rom\n");
        return -1;
    }
    //calculating total size
    fseek(chipROM,0L,SEEK_END);
    ROM_SIZE = ftell(chipROM);
    rewind(chipROM);

    //loading rom to memory at 0x200
    int rom_index = 0x200;
    while (rom_index<ROM_SIZE+0x200){
        MEMptr[rom_index]=fgetc(chipROM);
        rom_index++;
    }
    printf("Loaded rom %s\n",CHIP8_ROM);
    printf("ROM SIZE: %d\n",ROM_SIZE);
    fclose(chipROM);
    return 0;
}

void loadchars(){
    uint8_t fontset[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
    for(int i=0;i<80;i++){
        MEMptr[0x50+i] = fontset[i];
    }
}

void end(){
    free((void*)MEMptr);
    printf("freed ram\n");
}

int main(){
    if (initmem()==-1||loadrom()==-1){
        return 1;
    }
    initgui();
    printf("loaded rom in mem\n");
    printf("starting exec..\n");
    cpuRunner(MEMptr,ROM_SIZE);
    end();
    return 0;
}