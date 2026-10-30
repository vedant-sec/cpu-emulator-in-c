#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MEM_SIZE 65536//since the data-field is 16bit , mem-size = 2^16=>4kb
#define NUM_REGS 8

typedef struct {
    uint8_t  mem[MEM_SIZE];
    uint16_t reg[NUM_REGS];
    uint16_t pc;
    bool z, n, c;
    bool halted;
} CPU;

enum {
    OPCODE_LOAD_IMM = 0x00,
    OPCODE_ADD_IMM = 0x40,
    OPCODE_HALT = 0xFE,
};

//static void cpu_init(CPU *cpu) {
//    memset(cpu, 0, sizeof *cpu);
//}
//

// custum function for cpu initialization 
static void cpu_init(CPU *cpu){
	for(int i = 0 ; i < MEM_SIZE ; i++) cpu->mem[i]=0;
	for(int i = 0 ; i < NUM_REGS;i++) cpu->reg[i] =0 ;
	cpu->pc = 0;
	cpu->z = cpu->n=cpu->c=false ;
	cpu->halted = false ;

}

static void cpu_dump(const CPU *cpu) {
    printf("PC=%04X  Z=%d N=%d C=%d\n", cpu->pc, cpu->z, cpu->n, cpu->c);
    for (int i = 0; i < NUM_REGS; i++)
        printf("R%d=%04X  ", i, cpu->reg[i]);
    printf("\n");
}

static void cpu_step(CPU *cpu){
	uint8_t op = cpu->mem[cpu->pc]; //op:byte0 , the opcode  
	uint8_t r = cpu->mem[cpu->pc+1] & 0x07 ;// r byte 1 , register number. & 0x07 -> masking , keeps only the low 3 bits (00000111),forcing the value into 0-7. Only for safety measure . Done because we only have 8 registers (ie 111 in binary ) so we don't need to read remaining 5 bits , only the last 3 bits are required .
	
	// byte 2 and 3 are data-bytes , they are combined into one 16-bit number , 
	// mem[pc+2] << 8 moves the high bytes up into 8-15 
	// | mem[pc+3] places the low byte in 0-7
	// following the big endian assembly step 
	uint16_t val = (cpu->mem[cpu->pc + 2] << 8) | cpu->mem[cpu->pc + 3];
	



	cpu->pc += 4; // SINCE each instruction is 4 bytes , we need to increment the program counter by 4-bytes after each fetch&execute 
	
	// DECODE + EXECUTE
    switch (op) {
    case OPCODE_LOAD_IMM:
        cpu->reg[r] = val;
        break;
    case OPCODE_ADD_IMM: {
        uint32_t sum = (uint32_t)cpu->reg[r] + val;
        cpu->reg[r] = (uint16_t)sum;
        cpu->c = sum > 0xFFFF;
        cpu->z = cpu->reg[r] == 0;
        cpu->n = (cpu->reg[r] & 0x8000) != 0;
        break;
    }
    case OPCODE_HALT:
        cpu->halted = true;
        break;
    default:
        printf("OPCODE: [%02X] not recogonized at address %04X\n ", op, cpu->pc - 4);// an OP-code is 1-byte in our instruction set , so its always 2 hex-digits
        cpu->halted = true;
    }


}



int main(void){ // compiler make sure no argument is passed to main
	static CPU cpu ; // a CPU variable named cpu is created , 
			 // static changes where it lives and how long it lasts . 
			 // It lives in program's static data area which has much less memory restriction 
			 // and lasts for the duration of entirety of the program .
			 //  It also gets auto initiazed to zero . 
			 //  Its only visible inside main .
	cpu_init(&cpu);
	uint8_t prog[] = {
        0x00, 0x01, 0x00, 0x05,   // LD  R1, 5
        0x40, 0x01, 0x00, 0x03,   // ADD R1, 3
        0xFE, 0xFF, 0xFE, 0xFF,   // HALT
    	};
    	memcpy(cpu.mem, prog, sizeof prog);

    	while (!cpu.halted)
        cpu_step(&cpu);

    	cpu_dump(&cpu);               // expect R1=0008
    	return 0;



}
