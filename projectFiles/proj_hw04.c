/*
Author: LiKun Li
FileName: proj_hw04.c
Purpose: This project simulates the running process of CPU. Detail of each function will be explained later.
(****PLESAE USE README FILE FOR GETTING MORE DETAILS****)
*/


#include <stdio.h>
#include "proj_hw04.h"
#include <stdlib.h>

/*
Function: WORD getInstruction(WORD curPC, WORD *instructionMemory)
This function returns the current program counter in insterction memory.
*/
WORD getInstruction(WORD curPC, WORD *instructionMemory) {
	return instructionMemory[curPC >> 2];
}

/*
Function: void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut)
This function extract the different parts of the instrction out from the encoded instruction.
*/
void extract_instructionFields(WORD instruction, InstructionFields *fieldsOut) {

	//extract opcode
	//0x3f = 1111 11
	//From 0 to 5 bits of instruction, shift 26 bits.
	fieldsOut->opcode = (instruction >> 26) & 0x3f;

	//extract rs
	//0x1f = 1111 1
	//From 5 to 10 bits of instruction, shift 21 bits.
	fieldsOut->rs = (instruction >> 21) & 0x1f;

	//extract rt
	//0x1f = 1111 1
	//From 10 to 15 bits of instruction, shift 16 bits.
	fieldsOut->rt = (instruction >> 16) & 0x1f;

	//extract rd
	//0x1f = 1111 1
	//From 15 to 20 bits of R-format instruction, shift 11 bits.
	fieldsOut->rd = (instruction >> 11) & 0x1f;

	//extract shamt
	//0x1f = 1111 1
	//From 20 to 25 bits of R-format instruction, shift 6 bits.
	fieldsOut->shamt = (instruction >> 6) & 0x1f;

	//extract funct
	//0x3f = 1111 11
	//Last 6 bits of the R-format instruction, doesn't need to shift bits.
	fieldsOut->funct = instruction & 0x3f;

	//extract imm16
	//0xfff = 1111 1111 1111 1111
	//Last 16 bits of the I-format instruciton, doesn't need to shift bits.
	fieldsOut->imm16 = instruction & 0xffff;

	//extract imm32
	//Sign Extened the imm16 to 32.
	fieldsOut->imm32 = instruction << 16 >> 16;

	//extract addr
	//0x3ffffff = 11 1111 1111 1111 1111 1111 1111
	//Last 26 bits of the J-format instruction, doesn't need to shift bits
	fieldsOut->address = instruction & 0x3ffffff;

}

/*
Function: fill_CPUControl
This function read the OPcode from fields, and then give the controlOut specified CPU control bits.
(***EXTRA: ANDI, ORI, BNE***)
*/
int fill_CPUControl(InstructionFields *fields, CPUControl *controlOut) {
	if (fields->opcode != 0) {
		//j
		if (fields->opcode == 2) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 0;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 0;
			controlOut->branch = 0;
			controlOut->jump = 1;
			return 1;
		}
		//beq
		else if (fields->opcode == 4) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 1;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 0;
			controlOut->branch = 1;
			controlOut->jump = 0;
			return 1;
		}
		//bne (EXTRA)
		else if (fields->opcode == 5) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 1;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 0;
			controlOut->branch = 1;
			controlOut->jump = 0;
			controlOut->extra2 = 1;
			return 1;
		}
		//I-FORMAT instructions

		//addi
		else if (fields->opcode == 8) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;

		}
		//addiu
		else if (fields->opcode == 9) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//slti
		else if (fields->opcode == 10) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 1;
			controlOut->ALU.op = 3;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//andi(EXTRA)
		else if (fields->opcode == 12) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 0;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			controlOut->extra1 = 1;
			return 1;
		}
		//ori(EXTRA)
		else if (fields->opcode == 13) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 1;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			controlOut->extra1 = 1;
			return 1;
		}
		//lw
		else if (fields->opcode == 35) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 2;
			controlOut->memRead = 1;
			controlOut->memWrite = 0;
			controlOut->memToReg = 1;
			controlOut->regDst = 0;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//sw
		else if (fields->opcode == 43) {
			controlOut->ALUsrc = 1;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 1;
			controlOut->memToReg = 0;
			controlOut->regDst = 0;
			controlOut->regWrite = 0;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
	}
	//R-FORMAT instrctions
	else {

		//add
		if (fields->funct == 32) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//addu
		else if (fields->funct == 33) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//sub
		else if (fields->funct == 34) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 1;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//subu
		else if (fields->funct == 35) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 1;
			controlOut->ALU.op = 2;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//and
		else if (fields->funct == 36) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 0;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//or
		else if (fields->funct == 37) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 0;
			controlOut->ALU.op = 1;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
		//slt
		else if (fields->funct == 42) {
			controlOut->ALUsrc = 0;
			controlOut->ALU.bNegate = 1;
			controlOut->ALU.op = 3;
			controlOut->memRead = 0;
			controlOut->memWrite = 0;
			controlOut->memToReg = 0;
			controlOut->regDst = 1;
			controlOut->regWrite = 1;
			controlOut->branch = 0;
			controlOut->jump = 0;
			return 1;
		}
	}
	return 0;
}
/*
Function: getALUinput1
This function returns the first input of ALU, which is rs.
*/
WORD getALUinput1(CPUControl *controlIn, InstructionFields *fieldsIn, WORD rsVal, WORD rtVal) {
	return rsVal;
}

/*
Function: getALUinput2
This function returns the second input of ALU, which is rt or imm.
NOTICE: If the input instruction is I-format and it's not Jump or Brach, then return imm32.
HOWEVER, when the input instruction is ANDI or ORI, then it should return imm16 instead.
Jump and Branch should have the value of rt as R-format do.
*/

WORD getALUinput2(CPUControl *controlIn, InstructionFields *fieldsIn, WORD rsVal, WORD rtVal) {
	if (fieldsIn->opcode != 0) {
		if (controlIn->jump == 1 || controlIn->branch == 1) {
			return rtVal;
		}
		//when extra1 == 1, the instruction is either andi or ori.
		else if (controlIn->extra1 == 1) {
			return fieldsIn->imm16;
		}
		else
		{
			return fieldsIn->imm32;
		}
	}
	return rtVal;
}

/*
Function: void execute_ALU(CPUControl *controlIn, WORD input1, WORD input2, ALUResult  *aluResultOut)
This function simulate the process of ALU.
When ALUOP is 0, ALU should do the AND process.
When ALUOP is 1, ALU should do the OR process.
When ALUOP is 2, ALU should do the ADD process. When bNegate is 1, do the subtraction; otherwise, do the addition.
When ALUOP is 3, ALU should do the LESS process. Do the comparation of input1 and input2. set the result to 1 when input1 < input2. Otherwise, set reult to 0.
If the result is zero, the Zero in stuct should be set to one ( notificate that the output is 0)
(***EXTRA: When Extra2 == 1, means bne, means the output of Aluoutzero should be set to reverse of the original output.
*/

void execute_ALU(CPUControl *controlIn, WORD input1, WORD input2, ALUResult  *aluResultOut) {
	//ALUOP0, AND
	if (controlIn->ALU.op == 0) {
		aluResultOut->result = input1 & input2;
	}
	//ALUOP1, OR
	else if (controlIn->ALU.op == 1) {
		aluResultOut->result = input1 | input2;
	}
	//ALUOP2, ADD
	else if (controlIn->ALU.op == 2) {
		if (controlIn->ALU.bNegate == 0) {
			aluResultOut->result = input1 + input2;
		}
		else if (controlIn->ALU.bNegate == 1) {
			aluResultOut->result = input1 - input2;
		}
	}
	//ALUOP3, LESS
	else if (controlIn->ALU.op == 3) {
		if (input1 < input2) {
			aluResultOut->result = 1;
		}
		else if (input1 > input2 || input1 == input2) {
			aluResultOut->result = 0;
		}
	}
	if (aluResultOut->result == 0) {//if the result is zero, set zero to 1
		aluResultOut->zero = 1;
		//when Extra2 == 1, reverse the result.
		if (controlIn->extra2 == 1) {
			aluResultOut->zero = 0;
		}
	}
	else {
		aluResultOut->zero = 0;
		//when Extra2 == 1, reverse the result.
		if (controlIn->extra2 == 1) {
			aluResultOut->zero = 1;
		}
	}

}
/*
Function: void execute_MEM(CPUControl *controlIn, ALUResult *aluResultIn, WORD rsVal, WORD rtVal, WORD *memory, MemResult  *resultOut)
This function implements data memory.
If memRead == 1, then if should extract the value from memory and put them to the resultOut.
If memWrite == 1, if should put the value of rt into memory.
Otherwise, this insctruction is not LW or SW, then set readVal to 0 (break the wire)
*/
void execute_MEM(CPUControl *controlIn, ALUResult *aluResultIn, WORD rsVal, WORD rtVal, WORD *memory, MemResult  *resultOut) {
	// memRead == 1, LW
	if (controlIn->memRead == 1) {
		resultOut->readVal = memory[aluResultIn->result >> 2];
	}
	// memWrite == 1, SW
	else if (controlIn->memWrite == 1) {
		memory[aluResultIn->result >> 2] = rtVal;
	}
	else {
		resultOut->readVal = 0;
	}

}

/*
Function: WORD getNextPC(InstructionFields *fields, CPUControl *controlIn, int aluZero, WORD rsVal, WORD rtVal, WORD oldPC)
This function decides what is the next program counter.
If the insctrution is j, jr, jal etc(which has j bit), then it cotains 26 bits address.
If the instruction is branch and ALU output is 0, then the newPC = oldPC+4+imm*4
Otherwise, newPC = oldPC+4
*/

WORD getNextPC(InstructionFields *fields, CPUControl *controlIn, int aluZero, WORD rsVal, WORD rtVal, WORD oldPC) {
	WORD newPC = 0;
	if (controlIn->jump == 1) {
		WORD Drop = fields->address << 2;//Drop last two bis
		WORD temp1 = 0xf0000000; //0xf000000 = 1111 0000 0000 0000 0000 0000 0000 0000
		WORD temp2 = temp1 & oldPC; //temp2 has the first 4 bits of the oldPC
		newPC = temp2 | Drop;
	}
	//insterction is branch and ALUout == 0, means it's correct beq.
	else if (controlIn->branch == 1 && aluZero == 1) {
		newPC = oldPC + 4 + (fields->imm16 * 4);
	}
	else {
		newPC = oldPC + 4;
	}

	return newPC;
}
/*
Function: void execute_updateRegs(InstructionFields *fields, CPUControl *controlIn, ALUResult *aluResultIn, MemResult *memResultIn, WORD *regs)
This function represent the wire of WriteReg. It write back the value to the register.
If and only if regWrite == 1, we need to activate this function.
If memToReg == 1, we need to use the data from memory to write to the register (LW, LB ...)
*/
void execute_updateRegs(InstructionFields *fields, CPUControl *controlIn, ALUResult *aluResultIn, MemResult *memResultIn, WORD *regs) {
	if (controlIn->regWrite == 1) {
		//ALU -> Register
		if (controlIn->memToReg == 0) {
			//R-Format: RegDst == 1, write to rd
			if (controlIn->regDst == 1) {
				regs[fields->rd] = aluResultIn->result;
			}
			//I-Format: RegDst == 0, write to rt
			else if (controlIn->regDst == 0) {
				regs[fields->rt] = aluResultIn->result;
			}
		}
		//Memory -> Register
		else if (controlIn->memToReg == 1) {
			//R-Format: RegDst == 1, write to rd
			if (controlIn->regDst == 1) {
				regs[fields->rd] = memResultIn->readVal;
			}
			//I-Format: RegDst == 0, write to rt
			else if (controlIn->regDst == 0) {
				regs[fields->rt] = memResultIn->readVal;
			}

		}
	}
}




