#include "Z80Instruction.h"
#ifndef __Z80_PUSH_H__
#define __Z80_PUSH_H__
namespace Z80 {

class PUSH :public Z80Instruction
{
public:
	virtual PUSH* Clone() {return new PUSH();}
	virtual PUSH* ClonePlacement(void* buffer) {return new(buffer) PUSH();}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>* table)
	{
		return table->AllocateRegionToOpcode(this, L"11**0101", L"");
	}

	virtual Disassembly Z80Disassemble()
	{
		return Disassembly(L"PUSH", source.Disassemble());
	}

	virtual void Z80Decode(Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		source.SetIndexState(GetIndexState(), GetIndexOffset());
		target.SetIndexState(GetIndexState(), GetIndexOffset());

		//PUSH qq		11qq0101
		//PUSH IX		11011101 11100101
		//PUSH IY		11111101 11100101
		switch(data.GetDataSegment(4, 2))
		{
		case 0:
			source.SetMode(EffectiveAddress::MODE_BC);
			break;
		case 1:
			source.SetMode(EffectiveAddress::MODE_DE);
			break;
		case 2:
			source.SetMode(EffectiveAddress::MODE_HL);
			break;
		case 3:
			source.SetMode(EffectiveAddress::MODE_AF);
			break;
		}
		target.SetMode(EffectiveAddress::MODE_SP_PREDEC);

		AddInstructionSize(GetIndexOffsetSize(source.UsesIndexOffset() || target.UsesIndexOffset()));
		AddInstructionSize(source.ExtensionSize());
		AddInstructionSize(target.ExtensionSize());
		AddExecuteCycleCount(11);
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Word result;

		//Perform the operation
		additionalTime += source.Read(cpu, location, result);
		additionalTime += target.Write(cpu, location, result);

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress source;
	EffectiveAddress target;
};

} //Close namespace Z80
#endif
