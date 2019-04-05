#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

//#define DATA_LEN 6

void cpu_ram_write(struct cpu *cpu, unsigned char address, unsigned char value)
{
  cpu-> ram[address] = value;
}
/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *path)
{
  // char data[DATA_LEN] = {
  //   // From print8.ls8
  //   0b10000010, // LDI R0,8
  //   0b00000000,
  //   0b00001000,
  //   0b01000111, // PRN R0
  //   0b00000000,
  //   0b00000001  // HLT
  // };

  int address = 0;

  // for (int i = 0; i < DATA_LEN; i++) {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-code
  FILE *f = fopen(path, "r");
  if (f == NULL)
  {
    printf("could not open file");
    exit(1);
  }
  char line[256];
  while (fgets(line, sizeof(line),f) != NULL)
  {
    char *end;
    unsigned char val = strtoul(line, &end, 2);

    if (line == end)
    {
      continue;
    }
    cpu_ram_write(cpu, address++, val);
  }

}

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char address)
{
  return cpu-> ram[address];
}



/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      cpu->registers[regA] *= cpu->registers[regB];
      break;

    case ALU_ADD:
      cpu->registers[regA] += cpu->registers[regB];
      break;

    case ALU_SUB:
      cpu->registers[regA] -= cpu->registers[regB];
      break;

    case ALU_DIV:
      cpu->registers[regA] /= cpu->registers[regB];
      break;

    case ALU_CMP:
      if (cpu->registers[regA] == cpu->registers[regB])
      {
        cpu->flag = FL_EQUL;
      }
      if (cpu->registers[regA] > cpu->registers[regB])
      {
        cpu->flag = FL_GRTR;
      }
      if (cpu->registers[regA] < cpu->registers[regB])
      {
        cpu->flag = FL_LESS;
      }
      break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  unsigned char operand1; // location
  unsigned char operand2; // value

  while (running) 
  {
    
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char command = cpu_ram_read(cpu, cpu->pc);
    // 2. Figure out how many operands this next instruction requires
    unsigned char opCount = command & OPCM;
    // 3. Get the appropriate value(s) of the operands following this instruction

    
    
    //printf(" %d command %d pc\n", command, cpu->pc);

    switch(opCount)
    {
      case OPC1:
        operand1 = cpu_ram_read(cpu, cpu->pc + 1);
        break;
      case OPC2:
        operand1 = cpu_ram_read(cpu, cpu->pc + 1);
        operand2 = cpu_ram_read(cpu, cpu->pc + 2);
        break;
      default:
        break;
    }
    
    
    // 4. switch() over it to decide on a course of action.
    switch(command)
    {
      case HLT:
        running = 0;
        break;

      case CALL:
        cpu->registers[7] -= 1;
        cpu_ram_write(cpu, cpu->registers[7], cpu->pc + 1);
        cpu->pc = cpu->registers[operand1] -1;
        break;

      case PRN:
        printf("%d\n", cpu->registers[operand1]);
        cpu->pc ++;
        break;

      case LDI:
        cpu->registers[operand1] = operand2;
        cpu->pc += 2;
        break;

      case LD:
        cpu->registers[operand1] = cpu->registers[operand2];
        break;

      case MUL:
        alu(cpu, ALU_MUL, operand1, operand2);
        cpu->pc += 2;
        break;

      case DIV:
        alu(cpu, ALU_DIV, operand1, operand2);
        cpu->pc += 2;
        break;

      case CMP:
        alu(cpu, ALU_CMP, operand1, operand2);
        cpu->pc += 2;
        break;

      case ADD:
        alu(cpu, ALU_ADD, operand1, operand2);
        cpu->pc += 2;
        break;

      case SUB:
        alu(cpu, ALU_SUB, operand1, operand2);
        cpu->pc += 2;
        break;

      case JMP:
        cpu->pc = cpu->registers[operand1] - 1;
        break;

      case JEQ:
        if(cpu->flag == FL_EQUL)
        {
          cpu->pc = cpu->registers[operand1] - 1;
        }
        else
        {
          cpu->pc ++;
        }
        
        break;

      case JNE:
        if( cpu->flag == FL_INIT )
        {
          cpu->pc = cpu->registers[operand1] - 1;
        }
        else
        {
          cpu->pc ++;
        }
        break;

      // case JGE:
      //   if( cpu->flag == FL_GRTR || cpu->flag == FL_EQUL)
      //   {
      //     cpu->pc = cpu->registers[operand1];
      //   }
      //   break;

      // case JGT:
      //   if( cpu->flag == FL_GRTR )
      //   {
      //     cpu->pc = cpu->registers[operand1];
      //   }
      //   break;

      // case JLE:
      //   if( cpu->flag == FL_LESS || cpu->flag == FL_EQUL)
      //   {
      //     cpu->pc = cpu->registers[operand1];
      //   }
      //   break;

      // case JLT:
      //   if( cpu->flag == FL_LESS )
      //   {
      //     cpu->pc = cpu->registers[operand1];
      //   }
      //   break;

      case PUSH:
        cpu->registers[7] -= 1;
        cpu_ram_write(cpu , cpu->registers[7] ,cpu->registers[operand1]);
        cpu->pc ++;
        break;

      case POP:
        cpu->registers[operand1] = cpu_ram_read(cpu, cpu->registers[7]);
        cpu->registers[7] ++;
        cpu->pc ++;
        break;

      case RET:
        cpu->pc = cpu_ram_read(cpu, cpu->registers[7]);
        cpu->registers[7] ++;
        break;

      default:
      printf("inst %d \nskip more lines \n", cpu->pc);
        exit(1);
        break;
        
    }

    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
    cpu->pc ++;
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  cpu->registers = calloc(8, sizeof(unsigned char));
  cpu->ram = calloc(256, sizeof(unsigned char));
  cpu->registers[7] = 0xF4;
  cpu->flag = FL_INIT;
}
