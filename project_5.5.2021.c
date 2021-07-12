/* 
Class Project: The logical conclusion (v1.1)
CSCI-2500 Spring 2021
Prof. Slota 
*/

/******************************************************************************/
/* Usual suspects to include  */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define BIT type as a char (i.e., one byte)
typedef char BIT;
#define TRUE 1
#define FALSE 0
#define UNDEF -1

// useful constants
BIT ONE[32] = {TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT ZERO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT FOUR[32] = {FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT REG_THIRTY_ONE[5] = {TRUE, TRUE, TRUE, TRUE, TRUE};
BIT THIRTY_TWO[32] = {FALSE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};
BIT THIRTY_ONE[32] = {TRUE, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, 
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
  FALSE};


/******************************************************************************/
/* Function prototypes */
/******************************************************************************/
BIT not_gate(BIT A);
BIT or_gate(BIT A, BIT B);
BIT or_gate3(BIT A, BIT B, BIT C);
BIT and_gate(BIT A, BIT B);
BIT and_gate3(BIT A, BIT B, BIT C);
BIT xor_gate(BIT A, BIT B);
BIT nor_gate(BIT A, BIT B);
BIT nand_gate(BIT A, BIT B);

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3);
BIT multiplexor2(BIT S, BIT I0, BIT I1);
void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output);
BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3);

void copy_bits(BIT* A, BIT* B);
void print_binary(BIT* A);
void convert_to_binary(int a, BIT* A, int length);
void convert_to_binary_char(int a, char* A, int length);
int binary_to_integer(BIT* A);

int get_instructions(BIT Instructions[][32]);

void Instruction_Memory(BIT* ReadAddress, BIT* Instruction);
void Control(BIT* OpCode,
  BIT* RegDst, BIT* Jump, BIT* Branch, BIT* MemRead, BIT* MemToReg,
  BIT* ALUOp, BIT* MemWrite, BIT* ALUSrc, BIT* RegWrite, BIT* JALControl);
void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2);
void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData);
void ALU_Control(BIT* ALUOp, BIT* Jump,BIT* JRControl,BIT* funct, BIT* ALUControl, BIT* RegWrite);
void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result);
void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData);
void Extend_Sign16(BIT* Input, BIT* Output);
void updateState();
  
/******************************************************************************/
/* Functions provided for your convenience */
/******************************************************************************/
BIT not_gate(BIT A)
{
  if (A)
    return FALSE;
  else
    return TRUE;
}

BIT or_gate(BIT A, BIT B)
{
  if (A || B)
    return TRUE;
  else
    return FALSE;
}

BIT or_gate3(BIT A, BIT B, BIT C)
{
  return or_gate(A, or_gate(B, C));
}

BIT and_gate(BIT A, BIT B)
{
  if (A && B)
    return TRUE;
  else
    return FALSE;
}

BIT and_gate3(BIT A, BIT B, BIT C)
{
  return and_gate(A, and_gate(B, C));
}

BIT xor_gate(BIT A, BIT B)
{
  if (A ^ B)
    return TRUE;
  else
    return FALSE;
}

BIT nor_gate(BIT A, BIT B)
{
  return not_gate(or_gate(A, B));
}

BIT nand_gate(BIT A, BIT B)
{
  return not_gate(and_gate(A, B));
}

void decoder2(BIT I0, BIT I1, BIT* O0, BIT* O1, BIT* O2, BIT* O3)
{
  // Note: The input -> output mapping is slightly modified from before
  *O0 = and_gate(not_gate(I1), not_gate(I0));
  *O1 = and_gate(not_gate(I1), I0);
  *O2 = and_gate(I1, not_gate(I0));
  *O3 = and_gate(I1, I0);
  
  return;
}

BIT multiplexor2(BIT S, BIT I0, BIT I1)
{
  BIT x0 = and_gate(not_gate(S), I0);
  BIT x1 = and_gate(S, I1);
  return or_gate(x0, x1);  
}

void multiplexor2_32(BIT S, BIT* I0, BIT* I1, BIT* Output)
{
  for (int i = 0; i < 32; ++i) {
    BIT x0 = and_gate(not_gate(S), I0[i]);
    BIT x1 = and_gate(S, I1[i]);
    Output[i] = or_gate(x0, x1);
  }
}

BIT multiplexor4(BIT S0, BIT S1, BIT I0, BIT I1, BIT I2, BIT I3)
{
  BIT x0, x1, x2, x3 = FALSE;
  decoder2(S0, S1, &x0, &x1, &x2, &x3);
  
  BIT y0 = and_gate(x0, I0);
  BIT y1 = and_gate(x1, I1);
  BIT y2 = and_gate(x2, I2);
  BIT y3 = and_gate(x3, I3);
  
  BIT z0 = or_gate(y0, y1);
  BIT z1 = or_gate(y2, y3);
  
  return or_gate(z0, z1);  
}


/******************************************************************************/
/* Helper functions */
/******************************************************************************/
void copy_bits(BIT* A, BIT* B)
{
  for (int i = 0; i < 32; ++i)
    B[i] = A[i]; 
}

void print_binary(BIT* A)
{
  for (int i = 31; i >= 0; --i)
    printf("%d", A[i]); 
}

void convert_to_binary(int a, BIT* A, int length)
{
  if (a >= 0) {
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 1 ? TRUE : FALSE);
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 0 ? TRUE : FALSE);
      a /= 2;
    }
  }
}

void convert_to_binary_char(int a, char* A, int length)
{
  if (a >= 0) {
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 1 ? '1' : '0');
      a /= 2;
    }
  } else {
    a += 1;
    for (int i = 0; i < length; ++i) {
      A[i] = (a % 2 == 0 ? '1' : '0');
      a /= 2;
    }
  }
}
  
int binary_to_integer(BIT* A)
{
  unsigned a = 0;
  unsigned mult = 1;
  
  for (int i = 0; i < 32; ++i) {
    a += A[i]*mult;
    mult *= 2;
  }
  
  return (int)a;
}

void string_to_binary(char* str, BIT* A, int size)
{
  int base = 0; //Set the base value for the given register
  if(strcmp(str, "zero")==0)
    base = 0;
  else if(str[0]=='v')
    base=2;
  else if(str[0]=='a')
    base=4;
  else if(str[0]=='t')
    base=8;
  else if(str[0]=='s' && str[1] != 'p')
    base=16;
  else if(strcmp(str, "sp")==0)
    base=29;
  else if(strcmp(str, "ra")==0)
    base=31;
  int i=0; 
  if(strcmp(str, "zero")!=0 && strcmp(str, "sp")!=0 && strcmp(str, "ra")!=0) 
    i = str[1] - '0';
  int sum = base + i; //Registers types that have a range (s0, t0, etc) need to be shifted
  convert_to_binary(sum, A, size); //Convert the register # to binary
}

//5-to-32 decoder from lab06
void decoder3(BIT* I, BIT EN, BIT* O)
{
  O[0] = and_gate3(not_gate(I[2]), not_gate(I[1]), not_gate(I[0]));
  O[1] = and_gate3(not_gate(I[2]), not_gate(I[1]), I[0]);
  O[2] = and_gate3(not_gate(I[2]), I[1], not_gate(I[0]));
  O[3] = and_gate3(not_gate(I[2]), I[1], I[0]);
  O[4] = and_gate3(I[2], not_gate(I[1]), not_gate(I[0]));
  O[5] = and_gate3(I[2], not_gate(I[1]), I[0]);
  O[6] = and_gate3(I[2], I[1], not_gate(I[0]));
  O[7] = and_gate3(I[2], I[1], I[0]);
  
  O[0] = and_gate(EN, O[0]);
  O[1] = and_gate(EN, O[1]);
  O[2] = and_gate(EN, O[2]);
  O[3] = and_gate(EN, O[3]);
  O[4] = and_gate(EN, O[4]);
  O[5] = and_gate(EN, O[5]);
  O[6] = and_gate(EN, O[6]);
  O[7] = and_gate(EN, O[7]);
  
  return;
}

void decoder5(BIT* I, BIT* O)
{
   BIT EN[4] = {FALSE};
   decoder2(I[3], I[4], &EN[0], &EN[1], &EN[2], &EN[3]);
   decoder3(I, EN[3], &O[24]);
   decoder3(I, EN[2], &O[16]);
   decoder3(I, EN[1], &O[8]);
   decoder3(I, EN[0], &O[0]);
}

//one bit ALU and adder
void adder1(BIT A, BIT B, BIT CarryIn, BIT* CarryOut, BIT* Sum)
{
  BIT x0 = xor_gate(A, B);
  *Sum = xor_gate(CarryIn, x0);
  BIT y0 = and_gate(x0, CarryIn);
  BIT y1 = and_gate(A, B);
  *CarryOut = or_gate(y0, y1);
}

void ALU1(BIT A, BIT B, BIT Binvert, BIT CarryIn, BIT Less, 
  BIT Op0, BIT Op1, BIT* Result, BIT* CarryOut, BIT* Set)
{
  B = xor_gate(B,Binvert);
  BIT sum1 = UNDEF;
  adder1(A,B,CarryIn,CarryOut,&sum1);
  *Result =  multiplexor4(Op0,Op1, and_gate(A,B), or_gate(A,B), sum1, Less);
  *Set = sum1;
}

/******************************************************************************/
/* Parsing functions */
/******************************************************************************/

// TODO: Implement any helper functions to assist with parsing

int get_instructions(BIT Instructions[][32])
{
  char line[256] = {0};
  int instruction_count = 0;
  while (fgets(line, 256, stdin) != NULL) 
  {
    char inst[5]={0}; //Strings to parse
    char r1[5]={0};
    char r2[5]={0};
    char r3[5]={0};
    int num=0;
    sscanf(line, "%s", inst); //Check type of instruction
    if(strcmp(inst, "lw")==0 || strcmp(inst, "sw")==0 || 
       strcmp(inst, "beq")==0 || strcmp(inst, "addi")==0) //I-type Instructions
    {
      sscanf(line, "%s %s %s %d", inst, r1, r2, &num);
      BIT opcode[6]={0}; //Set opcode according to given instruction
      if(strcmp(inst, "lw")==0)
      {
        opcode[0]=1; opcode[1]=1; opcode[2]=0; opcode[3]=0; opcode[4]=0; opcode[5]=1;
      }
      else if(strcmp(inst, "sw")==0)
      {
        opcode[0]=1; opcode[1]=1; opcode[2]=0; opcode[3]=1; opcode[4]=0; opcode[5]=1;
      }
      else if(strcmp(inst, "beq")==0)
      {
        opcode[0]=0; opcode[1]=0; opcode[2]=1; opcode[3]=0; opcode[4]=0; opcode[5]=0;
      }
      else if(strcmp(inst, "addi")==0)
      {
        opcode[0]=0; opcode[1]=0; opcode[2]=0; opcode[3]=1; opcode[4]=0; opcode[5]=0;
      }
      BIT rs[5]={0};
      BIT rt[5]={0};
      BIT offset[16]={0};
      if(strcmp(inst, "beq")==0) ///beq has a different order for rs and rt
      {
        string_to_binary(r1, rs, 5); //Convert registers to binary
        string_to_binary(r2, rt, 5);
      }
      else
      {
        string_to_binary(r1, rt, 5);
        string_to_binary(r2, rs, 5);
      }
      convert_to_binary(num, offset, 16); 
      for(int i=0; i<32; i++) //Combine all the parts to store the binary instruction
      {
        if(i < 16)
          Instructions[instruction_count][i] = offset[i];
        else if(i >= 16 && i < 21)
          Instructions[instruction_count][i] = rt[i-16];
        else if(i >=21 && i < 26)
          Instructions[instruction_count][i] = rs[i-21];
        else
          Instructions[instruction_count][i] = opcode[i-26];
      }
    }
    else if(strcmp(inst, "and")==0 || strcmp(inst, "or")==0 || 
            strcmp(inst, "add")==0 || strcmp(inst, "sub")==0 || 
            strcmp(inst, "slt")==0 || strcmp(inst, "jr")==0) //R-type Instructions
    {
      if(strcmp(inst, "jr")==0) //jr input is different from the other R instructions
        sscanf(line, "%s %s", inst, r2);
      else
        sscanf(line, "%s %s %s %s", inst, r1, r2, r3);
      BIT opcode[6]={0,0,0,0,0,0}; //All R-types have 000000 opcode
      BIT funct[6]={0}; //Set funct according to given instruction
      if(strcmp(inst, "and")==0) 
      {
        funct[0]=0; funct[1]=0; funct[2]=1; funct[3]=0; funct[4]=0; funct[5]=1;
      }
      else if(strcmp(inst, "or")==0)
      {
        funct[0]=1; funct[1]=0; funct[2]=1; funct[3]=0; funct[4]=0; funct[5]=1;
      }
      else if(strcmp(inst, "add")==0)
      {
        funct[0]=0; funct[1]=0; funct[2]=0; funct[3]=0; funct[4]=0; funct[5]=1;
      }
      else if(strcmp(inst, "sub")==0)
      {
        funct[0]=0; funct[1]=1; funct[2]=0; funct[3]=0; funct[4]=0; funct[5]=1;
      }
      else if(strcmp(inst, "slt")==0)
      {
        funct[0]=0; funct[1]=1; funct[2]=0; funct[3]=1; funct[4]=0; funct[5]=1;
      }
      else if(strcmp(inst, "jr")==0)
      {
        funct[0]=0; funct[1]=0; funct[2]=0; funct[3]=1; funct[4]=0; funct[5]=0;
      }
      BIT shamt[5]={0,0,0,0,0}; //shamt is always 00000 since no shifting instructions
      BIT rd[5]={0};
      BIT rs[5]={0};
      BIT rt[5]={0};
      if(strcmp(inst, "jr")!=0) //jr is different
      {
        string_to_binary(r1, rd, 5); //Convert registers to binary
        string_to_binary(r3, rt, 5);
        string_to_binary(r2, rs, 5);
      }
      else
      {
        string_to_binary(r2, rd, 5);
        string_to_binary(r2, rs, 5);
      }
      
      for(int i=0; i<32; i++) //Combine all the parts to store the binary instruction
      {
        if(i < 6)
          Instructions[instruction_count][i] = funct[i];
        else if(i >= 6 && i < 11)
          Instructions[instruction_count][i] = shamt[i-6];
        else if(i >= 11 && i < 16)
          Instructions[instruction_count][i] = rd[i-11];
        else if(i >= 16 && i < 21)
          Instructions[instruction_count][i] = rt[i-16];
        else if(i >= 21 && i < 26)
          Instructions[instruction_count][i] = rs[i-21];
        else
          Instructions[instruction_count][i] = opcode[i-26];
      }
    }
    else if(strcmp(inst, "j")==0 || strcmp(inst, "jal")==0) //J-type Instructions
    {
      sscanf(line, "%s %d", inst, &num);
      BIT opcode[6]={0}; //Set opcode according to given instruction
      if(strcmp(inst, "j")==0) 
      {
        opcode[0]=0; opcode[1]=1; opcode[2]=0; opcode[3]=0; opcode[4]=0; opcode[5]=0;
      }
      else if(strcmp(inst, "jal")==0)
      {
        opcode[0]=1; opcode[1]=1; opcode[2]=0; opcode[3]=0; opcode[4]=0; opcode[5]=0;
      }
      BIT address[26]={0};
      convert_to_binary(num, address, 26); //Convert the address to binary
      for(int i=0; i<32; i++) //Combine all the parts to store the binary instruction
      {
        if(i < 26)
          Instructions[instruction_count][i] = address[i];
        else
          Instructions[instruction_count][i] = opcode[i-26];
      }
    }
    instruction_count++; //Increment instruction_count
  }
  return instruction_count;
}


/******************************************************************************/
/* Program state - memory spaces, PC, and control */
/******************************************************************************/
BIT PC[32]                  = {FALSE};
BIT MEM_Instruction[32][32] = {FALSE};
BIT MEM_Data[32][32]        = {FALSE};
BIT MEM_Register[32][32]    = {FALSE};

BIT RegDst    = FALSE;
BIT Jump      = FALSE;
BIT Branch    = FALSE;
BIT MemRead   = FALSE;
BIT MemToReg  = FALSE;
BIT ALUOp[2]  = {FALSE};
BIT MemWrite  = FALSE;
BIT ALUSrc    = FALSE;
BIT RegWrite  = FALSE;
BIT Zero      = FALSE;
BIT ALUControl[4] = {FALSE};
BIT JALControl = FALSE;
BIT JRControl = FALSE;

void print_instruction()
{
  unsigned pc = binary_to_integer(PC);
  printf("PC: %d\n", pc);
  printf("Instruction: ");
  print_binary(MEM_Instruction[pc]);
  printf("\n");
}

void print_state()
{
  printf("Data: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Data[i]));
  } 
  printf("\n");  
  
  printf("Register: ");
  for (int i = 0; i < 32; ++i) {
    printf("%d ", binary_to_integer(MEM_Register[i]));
  } 
  printf("\n");
}


/******************************************************************************/
/* Functions that you will implement */
/******************************************************************************/
void Instruction_Memory(BIT* ReadAddress, BIT* Instruction)
{
  // TODO: Implement instruction memory
  // Input: 32-bit instruction address
  // Output: 32-bit binary instruction -- already saved in get_instruction()
  // Note: Useful to use a 5-to-32 decoder here
  BIT REG_five_bit[5] = {FALSE};
  for (int i = 0; i < 5; ++i){
    REG_five_bit[i] = ReadAddress[i]; //copy first five digits (shouldn't need rest (shouldnt be greater than 32 I think))
  }
  //use 5-to-32 decoder to find instruction from mem_address
  BIT O[32] = {FALSE};
  decoder5(REG_five_bit,O);
  //Instruction = MEM_address[binary_to_integer(ReadAddress)];
  BIT done = FALSE;
  for (int i = 0; i < 32; ++i){
    BIT tempres[32] = {FALSE};
    BIT tmp = FALSE;
    multiplexor2_32(done, MEM_Instruction[i], Instruction, tempres); //if not done, instruction = MEM_instruction[i]
    copy_bits(tempres,Instruction);                                  //else done, instruction = instruction
    tmp = done;
    done = or_gate(O[i], tmp);                                       //done is FALSE until O[i] == 1, then done is TRUE
  }
}

void Control(BIT* OpCode,
  BIT* RegDst, BIT* Jump, BIT* Branch, BIT* MemRead, BIT* MemToReg,
  BIT* ALUOp, BIT* MemWrite, BIT* ALUSrc, BIT* RegWrite, BIT* JALControl)
{
  // TODO: Set control bits for everything
  // Input: opcode field from the instruction
  // OUtput: all control lines get set 
  // Note: Can use SOP or similar approaches to determine bits

  //RegDst
  //True for R-Type instructions (OPCODE : 000000)
  *RegDst = not_gate(or_gate(
                    or_gate3(OpCode[0],OpCode[1],OpCode[2]),
                    or_gate3(OpCode[3],OpCode[4],OpCode[5])));

  //Jump
  //True for jump Instructions (OPCODE : 010000, 110000, {jr handled in ALUControl})
  *Jump = and_gate(
            and_gate(OpCode[1],not_gate(OpCode[2])),
            and_gate3(not_gate(OpCode[3]),not_gate(OpCode[4]),not_gate(OpCode[5]))
            );

  //Branch
  //True for branch instructions (OPCODE : 001000)
  *Branch = and_gate(
            and_gate3(not_gate(OpCode[0]),not_gate(OpCode[1]),OpCode[2]),
            and_gate3(not_gate(OpCode[3]),not_gate(OpCode[4]),not_gate(OpCode[5]))
            );
          
  //MemRead
  //True for lw (OPCODE : 110001)
  *MemRead = and_gate(
            and_gate3(OpCode[0],OpCode[1],not_gate(OpCode[2])),
            and_gate3(not_gate(OpCode[3]),not_gate(OpCode[4]),OpCode[5])
            );

  //MemToReg
  //True for lw
  *MemToReg = and_gate(
            and_gate3(OpCode[0],OpCode[1],not_gate(OpCode[2])),
            and_gate3(not_gate(OpCode[3]),not_gate(OpCode[4]),OpCode[5])
            );

  //ALUOp
  //2-bits
  ALUOp[1] = *RegDst;
  ALUOp[0] = *Branch;

  //MemWrite
  //True for sw (OPCODE : 110101)
  *MemWrite = and_gate(
            and_gate3(OpCode[0],OpCode[1],not_gate(OpCode[2])),
            and_gate3(OpCode[3],not_gate(OpCode[4]),OpCode[5])
            );

  //ALUSrc
  //True for lw and sw and addi (addi is 000100)
  *ALUSrc = or_gate(or_gate(*MemToReg,*MemWrite),
            and_gate(
            and_gate3(not_gate(OpCode[0]),not_gate(OpCode[1]),not_gate(OpCode[2])),
            and_gate3(OpCode[3],not_gate(OpCode[4]),not_gate(OpCode[5])
            )));


  //JALControl
  *JALControl = and_gate(
            and_gate3(OpCode[0],OpCode[1],not_gate(OpCode[2])),
            and_gate3(not_gate(OpCode[3]),not_gate(OpCode[4]),not_gate(OpCode[5]))
            );


  //RegWrite
  //True for jal, addi, r-types, and lw (OPCODE : 110000, 000100, 000000, 110001)

  *RegWrite = or_gate(or_gate(*JALControl, and_gate(
            and_gate3(not_gate(OpCode[0]),not_gate(OpCode[1]),not_gate(OpCode[2])),
            and_gate3(OpCode[3],not_gate(OpCode[4]),not_gate(OpCode[5])
            ))),or_gate(*RegDst,*MemToReg));
  
}

void Read_Register(BIT* ReadRegister1, BIT* ReadRegister2,
  BIT* ReadData1, BIT* ReadData2)
{
  // TODO: Implement register read functionality
  // Input: two 5-bit register addresses
  // Output: the values of the specified registers in ReadData1 and ReadData2
  // Note: Implementation will be very similar to instruction memory circuit
  
  BIT O1[32] = {FALSE};
  decoder5(ReadRegister1,O1);
  BIT O2[32] = {FALSE};
  decoder5(ReadRegister2,O2);
  //ReadData1 = MEM_register[binary_to_integer(ReadRegister1)];
  //ReadData2 = MEM_register[binary_to_integer(ReadRegister2)];
  BIT done1 = FALSE; BIT done2 = FALSE;
  for (int i = 0; i < 32; ++i){
    BIT tempres1[32] = {FALSE};
    BIT tempres2[32] = {FALSE};
    BIT tmp1 = FALSE;
    BIT tmp2 = FALSE;

    //ReadData1
    multiplexor2_32(done1, MEM_Register[i], ReadData1, tempres1);
    copy_bits(tempres1,ReadData1);
    tmp1 = done1;
    done1 = or_gate(O1[i], tmp1);

    //ReadData2
    multiplexor2_32(done2, MEM_Register[i], ReadData2, tempres2);
    copy_bits(tempres2,ReadData2);
    tmp2 = done2;
    done2 = or_gate(O2[i], tmp2);
  }
  
}

void Write_Register(BIT RegWrite, BIT* WriteRegister, BIT* WriteData)
{
  // TODO: Implement register write functionality
  // Input: one 5-bit register address, data to write, and control bit
  // Output: None, but will modify register file
  // Note: Implementation will again be similar to those above
  //use 5-to-32 decoder to find instruction from mem_address
  
  BIT O[32] = {FALSE};
  decoder5(WriteRegister,O);
  BIT temporary[32] = {FALSE};
  multiplexor2_32(RegWrite,ZERO, O, temporary);
  copy_bits(temporary,O); //O = all zeros if control is false, otherwise = 1 at index WriteRegister
  //MEM_register[binary_to_integer(WriteRegister)] = WriteData;
  for (int i = 0; i < 32; ++i){
    BIT tempres[32] = {FALSE};
    multiplexor2_32(O[i], MEM_Register[i], WriteData, tempres);
    copy_bits(tempres,MEM_Register[i]);
  }
  
}

void ALU_Control(BIT* ALUOp, BIT* Jump, BIT* JRControl, BIT* funct, BIT* ALUControl, BIT* RegWrite)
{
  // TODO: Implement ALU Control circuit
  // Input: 2-bit ALUOp from main control circuit, 
  //        1-bit Jump control bit (for jr)
  //        6-bit funct field from the binary instruction
  // Output:4-bit ALUControl for input into the ALU
  // Note: Can use SOP or similar approaches to determine bits

  //ALUControl listed in the order where index 0 is the least signifigant
  

    //jr handler (because jr is an R-type for some reason)
    *JRControl =  and_gate(
            and_gate3(not_gate(funct[0]),not_gate(funct[1]),not_gate(funct[2])),
            and_gate3(funct[3],not_gate(funct[4]),not_gate(funct[5])
            ));
    *Jump = or_gate(*Jump,*JRControl);
    *RegWrite = and_gate(not_gate(*JRControl), *RegWrite);


    ALUControl[0]= and_gate(and_gate(not_gate(ALUOp[0]),ALUOp[1]),
                            or_gate(and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),not_gate(funct[3])),
                              and_gate3(funct[2],not_gate(funct[1]),funct[0]
                              )),
                              and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),funct[3]),
                              and_gate3(not_gate(funct[2]),funct[1],not_gate(funct[0])))
                              )); 

//funct for add : 
    ALUControl[1] = or_gate(or_gate(and_gate(not_gate(ALUOp[0]),not_gate(ALUOp[1])),                                        and_gate(ALUOp[0],not_gate(ALUOp[1]))),
                            and_gate(and_gate(not_gate(ALUOp[0]),ALUOp[1]),
                            or_gate3(
                              and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),not_gate(funct[3])),
                              and_gate3(not_gate(funct[2]),not_gate(funct[1]),not_gate(funct[0]))),
                              and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),not_gate(funct[3])),
                              and_gate3(not_gate(funct[2]),funct[1],not_gate(funct[0]))),
                              and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),funct[3]),
                              and_gate3(not_gate(funct[2]),funct[1],not_gate(funct[0]))))));

    ALUControl[2] = or_gate(and_gate(ALUOp[0],not_gate(ALUOp[1])),
                            and_gate(and_gate(not_gate(ALUOp[0]),ALUOp[1]),
                            or_gate(
                              and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),not_gate(funct[3])),
                              and_gate3(not_gate(funct[2]),funct[1],not_gate(funct[0]))),
                              and_gate(
                              and_gate3(funct[5],not_gate(funct[4]),funct[3]),
                              and_gate3(not_gate(funct[2]),funct[1],not_gate(funct[0]))))
                            ));

    //Most signifigant bit is always 0
    ALUControl[3] = FALSE;

    //
  
}


void ALU(BIT* ALUControl, BIT* Input1, BIT* Input2, BIT* Zero, BIT* Result)
{   
  // TODO: Implement 32-bit ALU
  // Input: 4-bit ALUControl, two 32-bit inputs
  // Output: 32-bit result, and zero flag bit
  // Note: Can re-use prior implementations (but need new circuitry for zero)
  /*
  ALUControl bits
  INDEX             3 2 1 0
  ---------------------------
  add               0 0 1 0
  subtract          0 1 1 0
  AND               0 0 0 0
  OR                0 0 0 1
  set-on-less-than  0 1 1 1
  */
  BIT binvert, CarryIn, Less, Op0, Op1, CarryOut, Set;
  Less = FALSE; Set = FALSE; CarryOut = FALSE;
  //binvert = 1 when subtract or slt
  binvert = or_gate(and_gate3(ALUControl[2],ALUControl[1],not_gate(ALUControl[0])),
                    and_gate3(ALUControl[2],ALUControl[1],ALUControl[0]));
  CarryIn = binvert;
  Op0 = or_gate(and_gate3(not_gate(ALUControl[2]),not_gate(ALUControl[1]),ALUControl[0]),
                and_gate3(ALUControl[2],ALUControl[1],ALUControl[0])); //=1 if OR or SLT
  Op1 = or_gate3(and_gate3(not_gate(ALUControl[2]),ALUControl[1],not_gate(ALUControl[0])),
                 and_gate3(ALUControl[2],ALUControl[1],not_gate(ALUControl[0])),
                 and_gate3(ALUControl[2],ALUControl[1],ALUControl[0])); //=1 if ADD or SUB or SLT

  ALU1(Input1[0], Input2[0], binvert, CarryIn, Less, 
    Op0, Op1, &Result[0], &CarryOut, &Set);
  for (int i = 1; i < 32; ++i) {
    ALU1(Input1[i], Input2[i], binvert, CarryOut, Less, 
      Op0, Op1, &Result[i], &CarryOut, &Set);
  }
  Less = Set;
  ALU1(Input1[0], Input2[0], binvert, CarryIn, Less, 
    Op0, Op1, &Result[0], &CarryOut, &Set);
  //set Zero
  BIT zero = TRUE;
  for (int i = 0; i < 32; ++i){
    *Zero = and_gate3(zero,not_gate(Result[i]),TRUE);
    zero = *Zero;
  }
}

void Data_Memory(BIT MemWrite, BIT MemRead, 
  BIT* Address, BIT* WriteData, BIT* ReadData)
{
  // TODO: Implement data memory
  // Input: 32-bit address, control flags for read/write, and data to write
  // Output: data read if processing a lw instruction
  // Note: Implementation similar as above
  
  BIT five_bit[5] = {FALSE};
  for (int i = 0; i < 5; ++i){
    five_bit[i] = Address[i];
  }
  //use 5-to-32 decoder to find instruction from mem_address
  BIT O[32] = {FALSE};
  BIT done = FALSE;
  decoder5(five_bit,O);
  //MEM_Data[binary_to_integer(Address)] = WriteData;
  for (int i = 0; i < 32; ++i){
    BIT tmp = FALSE;
    BIT tempres[32] = {FALSE};
    //write to memory
    BIT write_or_nah[32] = {FALSE};
    multiplexor2_32(O[i], MEM_Data[i], WriteData, tempres);
    multiplexor2_32(MemWrite, MEM_Data[i], tempres, write_or_nah);   
    copy_bits(write_or_nah,MEM_Data[i]);
    //read
    BIT read_or_nah[32] = {FALSE};
    multiplexor2_32(done, MEM_Data[i], ReadData, tempres);
    multiplexor2_32(MemRead, ZERO, tempres, read_or_nah);   
    copy_bits(read_or_nah,ReadData);
    tmp = done;
    done = or_gate(O[i], tmp);
  }
  
}

void Extend_Sign16(BIT* Input, BIT* Output)
{
  // TODO: Implement 16-bit to 32-bit sign extender
  // Copy Input to Output, then extend 16th Input bit to 17-32 bits in Output
  copy_bits(Input,Output);
  for (int i = 16; i < 32; i++){
    Output[i] = Input[15];
  }
}

// Datapath helper functions
void datapathAdder(BIT* A, BIT* B, BIT* Sum) {
  BIT CarryIn = FALSE;
  BIT* CarryOut = &CarryIn; // 
  for (int i=0; i<32; i++) {
    adder1(A[i], B[i], CarryIn, CarryOut, &Sum[i]);
  }
}

void datapathShiftLeft2 (BIT* A, BIT* R) {
  copy_bits(A, R);
  for (int i=31; i>=2; i--) {
    R[i] = A[i-2];
  }
  R[1] = FALSE;
  R[0] = FALSE;
}

void multiplexor4_32(BIT S0, BIT S1, BIT* I0, BIT* I1, BIT* I2, BIT* I3, BIT* Output) {
  
  BIT x0, x1, x2, x3 = FALSE;
  for (int i=0; i<32; i++) {
    decoder2(S0, S1, &x0, &x1, &x2, &x3);
    
    BIT y0 = and_gate(x0, I0[i]);
    BIT y1 = and_gate(x1, I1[i]);
    BIT y2 = and_gate(x2, I2[i]);
    BIT y3 = and_gate(x3, I3[i]);
    
    BIT z0 = or_gate(y0, y1);
    BIT z1 = or_gate(y2, y3);
    
    Output[i] = or_gate(z0, z1); 
  }
}

void multiplexor4_5(BIT S0, BIT S1, BIT* I0, BIT* I1, BIT* I2, BIT* I3, BIT* Output) {
  
  BIT x0, x1, x2, x3 = FALSE;
  for (int i=0; i<5; i++) {
    decoder2(S0, S1, &x0, &x1, &x2, &x3);
    
    BIT y0 = and_gate(x0, I0[i]);
    BIT y1 = and_gate(x1, I1[i]);
    BIT y2 = and_gate(x2, I2[i]);
    BIT y3 = and_gate(x3, I3[i]);
    
    BIT z0 = or_gate(y0, y1);
    BIT z1 = or_gate(y2, y3);
    
    Output[i] = or_gate(z0, z1); 
  }
}

void updateState()
{
  // TODO: Implement the full datapath here
  // Essentially, you'll be figuring out the order in which to process each of 
  // the sub-circuits comprising the entire processor circuit. It makes it 
  // easier to consider the pipelined version of the process, and handle things
  // in order of the pipeline. The stages and operations are:
  // Fetch - load instruction from instruction memory
  // Decode - set control bits and read from the register file
  // Execute - process ALU
  // Memory - read/write data memory
  // Write Back - write to the register file
  // Update PC - determine the final PC value for the next instruction

  // Stage 1: Fetch
  BIT PC1[32] = {0};
  datapathAdder(PC, ONE, PC1); // PC1 = PC+1

  BIT binary_instruction[32];
  Instruction_Memory(PC, binary_instruction);

  BIT OpCode[6];
  int count = 0;
  for (int i=26; i<32; i++) {
    OpCode[count] = binary_instruction[i];
    count++;
  }

  // Stage 2: Decode
  Control(OpCode, &RegDst, &Jump, &Branch, &MemRead, &MemToReg, ALUOp, &MemWrite, &ALUSrc, &RegWrite, &JALControl);

  // Creating arrays to hold sections of instruction
  BIT JumpAddress[32];
  copy_bits(binary_instruction, JumpAddress);
  for (int i=26; i<32; i++) {
    JumpAddress[i] = 0;
  }
  BIT SixteentoTwenty[5];
  count = 0;
  for (int i=16; i<21; i++) {
    SixteentoTwenty[count] = binary_instruction[i];
    count++;
  }
  BIT EleventoFifteen[5];
  count = 0;
  for (int i=11; i<16; i++) {
    EleventoFifteen[count] = binary_instruction[i];
    count++;
  }
  BIT TwentyonetoTwentyfive[5];
  count = 0;
  for(int i=21; i<26; i++) {
    TwentyonetoTwentyfive[count] = binary_instruction[i];
    count++;
  }
  BIT ZerotoFifteen[32];
  copy_bits(binary_instruction, ZerotoFifteen);

  // 21-25, 16-20 of binary_instruction
  BIT ReadData1[32];
  BIT ReadData2[32];
  Read_Register(TwentyonetoTwentyfive, SixteentoTwenty, ReadData1, ReadData2);

  // 11-15 or 16-20 of binary_instruction
  BIT WriteRegister[5];
  BIT ZERO_5[5] = {FALSE};
  multiplexor4_5(RegDst, JALControl, SixteentoTwenty, EleventoFifteen, REG_THIRTY_ONE, ZERO_5, WriteRegister);

  BIT ZerotoFifteenExtended[32];
  Extend_Sign16(ZerotoFifteen, ZerotoFifteenExtended);

  // Stage 3: Execute
  // no shifting of ZerotoFifteenExtended
  BIT TopRightSum[32];
  datapathAdder(PC1, ZerotoFifteenExtended, TopRightSum);

  // ALU
  BIT ALUArgument2[32];
  multiplexor2_32(ALUSrc, ReadData2, ZerotoFifteenExtended, ALUArgument2);
  ALU_Control(ALUOp, &Jump, &JRControl, ZerotoFifteen, ALUControl, &RegWrite);

  BIT ALUResult[32];
  BIT Zero;
  ALU(ALUControl, ReadData1, ALUArgument2, &Zero, ALUResult);
  
  // Stage 4: Memory
  BIT FirstMuxResult[32];
  multiplexor2_32(and_gate(Branch,Zero), PC1, TopRightSum, FirstMuxResult);

  BIT ReadData[32];
  Data_Memory(MemWrite, MemRead, ALUResult, ReadData2, ReadData);

  BIT WriteData[32];
  multiplexor4_32(MemToReg, JALControl, ALUResult, ReadData, PC1, ZERO, WriteData);

  // Stage 5: Write Back
  Write_Register(RegWrite, WriteRegister, WriteData);

  // Stage 6: Update PC
  multiplexor4_32(Jump, JRControl, FirstMuxResult, JumpAddress, ZERO, ReadData1, PC); // PC updated

}


/******************************************************************************/
/* Main */
/******************************************************************************/

int main()
{

  setbuf(stdout, NULL);
    
  // parse instructions into binary format
  int counter = get_instructions(MEM_Instruction);
  
  // load program and run
  copy_bits(ZERO, PC);
  copy_bits(THIRTY_TWO, MEM_Register[29]);
  
  while (binary_to_integer(PC) < counter) {
    print_instruction();
    updateState();
    print_state();
  }

  return 0;
}

