#include "M68000Instruction.h"
#ifndef __M68000_SUBQ_H__
#define __M68000_SUBQ_H__
namespace M68000 {

class SUBQ :public M68000Instruction
{
public:
	virtual SUBQ* Clone() {return new SUBQ();}
	virtual SUBQ* ClonePlacement(void* buffer) {return new(buffer) SUBQ();}

	virtual bool RegisterOpcode(OpcodeTable<M68000Instruction>* table)
	{
		bool result = true;
		result &= table->AllocateRegionToOpcode(this, L"0101***1CCDDDDDD", L"CC=00 DDDDDD=000000-000111,010000-110111,111000,111001");
		result &= table->AllocateRegionToOpcode(this, L"0101***1CCDDDDDD", L"CC=01-10 DDDDDD=000000-110111,111000,111001");
		return result;
	}

	virtual Disassembly M68000Disassemble()
	{
		return Disassembly(L"SUBQ." + DisassembleSize(size), source.Disassemble() + L", " + target.Disassemble());
	}

	virtual void M68000Decode(M68000* cpu, const M68000Long& location, const M68000Word& data, bool transparent)
	{
//	-----------------------------------------------------------------
//	|15 |14 |13 |12 |11 |10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//	|---|---|---|---|-----------|---|-------|-----------|-----------|
//	| 0 | 1 | 0 | 1 |    DATA   | 1 | SIZE  |    MODE   |  REGISTER |
//	----------------------------------------=========================
//                              |--<opmode>-|----------<ea>---------|
		switch(data.GetDataSegment(6, 2))
		{
		case 0:	//00
			size = BITCOUNT_BYTE;
			break;
		case 1:	//01
			size = BITCOUNT_WORD;
			break;
		case 2:	//10
			size = BITCOUNT_LONG;
			break;
		}

		//SUBQ	#<data>,<ea>
		source.BuildQuickData(location + GetInstructionSize(), data.GetDataSegment(9, 3));
		target.Decode(data.GetDataSegment(0, 3), data.GetDataSegment(3, 3), size, location + GetInstructionSize(), cpu, transparent, GetInstructionRegister());
		AddInstructionSize(target.ExtensionSize());

		if(target.GetAddressMode() == EffectiveAddress::DATAREG_DIRECT)
		{
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(4, 1, 0));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 0));
			}
		}
		else if(target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT)
		{
			AddExecuteCycleCount(ExecuteTime(8, 1, 0));
		}
		else
		{
			if(size != BITCOUNT_LONG)
			{
				AddExecuteCycleCount(ExecuteTime(8, 1, 1));
			}
			else
			{
				AddExecuteCycleCount(ExecuteTime(12, 1, 2));
			}
			AddExecuteCycleCount(target.DecodeTime());
		}
	}

	virtual ExecuteTime M68000Execute(M68000* cpu, const M68000Long& location) const
	{
		double additionalTime = 0;
		Data op1(size);
		Data op2(size);
		Data result(size);

		//Change the operation size to long if the target is an address register. See the
		//description of the opcode in the 68000 Programmer's Manual for more info on
		//this behaviour. We do this test here instead of the decode function so that the
		//disassembly correctly shows the specified size of the operation, even if it has
		//no meaning on an address register.
		if(target.GetAddressMode() == EffectiveAddress::ADDREG_DIRECT)
		{
			op1.Resize(BITCOUNT_LONG);
			op2.Resize(BITCOUNT_LONG);
			result.Resize(BITCOUNT_LONG);
		}

		//Perform the operation
		additionalTime += source.Read(cpu, op1, GetInstructionRegister());
		additionalTime += target.ReadWithoutAdjustingAddress(cpu, op2, GetInstructionRegister());
		result = op2 - op1;
		additionalTime += target.Write(cpu, result, GetInstructionRegister());

		if(target.GetAddressMode() != EffectiveAddress::ADDREG_DIRECT)
		{
			//Set the flag results
			bool overflow = (op1.MSB() == result.MSB()) && (op2.MSB() != op1.MSB());
			bool borrow = (op1.MSB() && result.MSB()) || (!op2.MSB() && (op1.MSB() || result.MSB()));
			cpu->SetX(borrow);
			cpu->SetN(result.Negative());
			cpu->SetZ(result.Zero());
			cpu->SetV(overflow);
			cpu->SetC(borrow);
		}

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
	Bitcount size;
};

} //Close namespace M68000
#endif
