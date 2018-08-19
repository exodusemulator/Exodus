#include "Z80Instruction.h"
#ifndef __Z80_SRL_H__
#define __Z80_SRL_H__
namespace Z80 {

class SRL :public Z80Instruction
{
public:
	virtual SRL* Clone() const {return new SRL();}
	virtual SRL* ClonePlacement(void* buffer) const {return new(buffer) SRL();}
	virtual size_t GetOpcodeClassByteSize() const {return sizeof(*this);}

	virtual bool RegisterOpcode(OpcodeTable<Z80Instruction>& table) const
	{
		return table.AllocateRegionToOpcode(this, L"00111***", L"");
	}

	virtual std::wstring GetOpcodeName() const
	{
		return L"SRL";
	}

	virtual Disassembly Z80Disassemble(const Z80::LabelSubstitutionSettings& labelSettings) const
	{
		return Disassembly(GetOpcodeName(), _target.Disassemble());
	}

	virtual void Z80Decode(const Z80* cpu, const Z80Word& location, const Z80Byte& data, bool transparent)
	{
		_target.SetIndexState(GetIndexState(), GetIndexOffset());
		_doubleOutput = false;

		if(_target.Decode8BitRegister(data.GetDataSegment(0, 3)))
		{
			//SRL r			11001011 00111rrr
			AddExecuteCycleCount(4);
		}
		else
		{
			//SRL (HL)		11001011 00111110
			//SRL (IX+d)	11011101 11001011 dddddddd 00111110
			//SRL (IY+d)	11111101 11001011 dddddddd 00111110
			_target.SetMode(EffectiveAddress::Mode::HLIndirect);
			AddExecuteCycleCount(11);

			if(GetIndexState() != EffectiveAddress::IndexState::None)
			{
				_doubleOutput = true;
				_targetHL.SetIndexState(GetIndexState(), GetIndexOffset());
				_targetHL.SetMode(EffectiveAddress::Mode::HLIndirect);
				AddExecuteCycleCount(4);
			}
		}

		AddInstructionSize(GetIndexOffsetSize(_target.UsesIndexOffset()));
		AddInstructionSize(_target.ExtensionSize());
	}

	virtual ExecuteTime Z80Execute(Z80* cpu, const Z80Word& location) const
	{
		double additionalTime = 0;
		Z80Byte op1;
		Z80Byte result;

		//Perform the operation
		if(_doubleOutput)
		{
			additionalTime += _targetHL.Read(cpu, location, op1);
		}
		else
		{
			additionalTime += _target.Read(cpu, location, op1);
		}
		result = (op1 >> 1);
		if(_doubleOutput)
		{
			additionalTime += _targetHL.Write(cpu, location, result);
		}
		additionalTime += _target.Write(cpu, location, result);

		//Set the flag results
		cpu->SetFlagS(false);
		cpu->SetFlagZ(result.Zero());
		cpu->SetFlagY(result.GetBit(5));
		cpu->SetFlagH(false);
		cpu->SetFlagX(result.GetBit(3));
		cpu->SetFlagPV(result.ParityEven());
		cpu->SetFlagN(false);
		cpu->SetFlagC(op1.GetBit(0));

		//Adjust the PC and return the execution time
		cpu->SetPC(location + GetInstructionSize());
		return GetExecuteCycleCount(additionalTime);
	}

private:
	EffectiveAddress _target;
	EffectiveAddress _targetHL;
	bool _doubleOutput;
};

} //Close namespace Z80
#endif
