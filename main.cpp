/*
  ===========================================================================

  Copyright (C) 2014 Emvivre

  This file is part of PS1_REVERSE.

  PS1_REVERSE is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  PS1_REVERSE is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with PS1_REVERSE.  If not, see <http://www.gnu.org/licenses/>.

  ===========================================================================
*/

/**
# header de tekken3 PAL
$ hexdump -C sces_012.37
00000000  50 53 2d 58 20 45 58 45  00 00 00 00 00 00 00 00  |PS-X EXE........|
00000010  58 9c 07 80 00 00 00 00  00 00 01 80 00 68 12 00  |X............h..|
00000020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000030  f0 ff 1f 80 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000040  00 00 00 00 00 00 00 00  00 00 00 00 53 6f 6e 79  |............Sony|
00000050  20 43 6f 6d 70 75 74 65  72 20 45 6e 74 65 72 74  | Computer Entert|
00000060  61 69 6e 6d 65 6e 74 20  49 6e 63 2e 20 66 6f 72  |ainment Inc. for|
00000070  20 45 75 72 6f 70 65 20  61 72 65 61 00 00 00 00  | Europe area....|
00000080  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
 */

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "hexdump.h"
#include <map>
#include <sstream>
#include <iomanip>
#include <fstream>

/*
  ===========================================================================
  
  COMMON TYPE  
  
  ===========================================================================
*/


typedef long long int s64;
typedef unsigned long long int u64;
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef char s8;
typedef unsigned char u8;
typedef float f32;
typedef double f64;
typedef long double f96;

#define CONCAT2(A,B) A##B
#define CONCAT(A,B) CONCAT2(A,B)

unsigned int text_pc;

std::ofstream outfile;
std::ostream* outstream;

/*
  ===========================================================================
  
  FILE UTIL  
  
  ===========================================================================
*/

namespace FileUtil
{
	class UnableToOpenFile {};
	static const unsigned char* read_file(std::string f, unsigned int& sz) {
		int fd = open(f.c_str(), O_RDONLY);
		if ( fd < 0 ) throw UnableToOpenFile();
		sz = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		unsigned char* data = new unsigned char[sz];
		read(fd, data, sz);
		close(fd);
		return data;
	}
}

/*
  ===========================================================================
  
  PS1 EXE FORMAT  
  
  ===========================================================================
*/


struct PS1Header {
	u8 magic[8]; // PS-X EXE
	u32 text;                            // ?
	u32 data;                            // ?
	u32 pc0;
	u32 gp0;
	u32 t_addr;
	u32 t_size;
	u32 d_addr;
	u32 d_size;
	u32 b_addr;
	u32 b_size;
	u32 s_addr;
	u32 s_size;
	u32 sp,fp,gp,ret,base;
	u8 marker[1972];
	PS1Header(const unsigned char* b) {
		*this = *(const PS1Header*)b;
	}
	int pc0_offset() const { return pc0 - t_addr; }
	friend std::ostream& operator<<(std::ostream& os, const PS1Header& h)
	{
		os << "magic_number: " << h.magic << "\n";
		os << "pc0 : 0x" << std::hex << h.pc0 << std::dec << "\n";
		os << "t_addr : 0x" << std::hex << h.t_addr << std::dec << "\n";
		os << "t_size : " << h.t_size << "\n";
		os << "s_addr : 0x" << std::hex << h.s_addr << std::dec << "\n";
		os << "s_size : " << h.s_size << "\n";
		os << "marker : " << h.marker << "\n";		
		os << "pc0_offset : 0x" << std::hex << h.pc0_offset() << std::dec;
		return os;
	}
};

struct PS1File
{
	std::string filename;	
	unsigned int data_sz;
	union {
		const unsigned char* data;
		PS1Header* h;
	};
	PS1File(std::string filename) : filename(filename) {
		data = FileUtil::read_file(filename, data_sz);
	}
	~PS1File() {
		delete[] data;
	}
	const unsigned char* text_section() const { return data + 0x800; }	
};

namespace R3000AInstruction
{
	const std::string registers[] = { "$zr", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" };		
	static unsigned int shift_mask(unsigned int data, int shift, int len_bit) {
		return (data >> shift) & ((1<<len_bit)-1);
	}

	static std::string hex2str(unsigned long v, int l) 
	{
		std::stringstream ss; 
		ss << "0x" << std::hex << std::setfill('0') << std::setw(l) << v << std::dec; 
		return ss.str();
	}
	
	struct Generic {
		unsigned int op : 6;
		unsigned int other: 26;
		u64 pc;
		Generic(unsigned int data, u64 pc) : op(shift_mask(data, 26, 6)), other(shift_mask(data, 0, 26)), pc(pc) {}
		virtual std::string name() const { std::stringstream ss; ss << "UNKNOWN_INSTRUCTION --- " << std::hex << "op:" << op << " other:" << other << std::dec; return ss.str(); }
		virtual std::ostream& str(std::ostream& os) const { return os; }
		friend std::ostream& operator<<(std::ostream& os, const Generic& inst) 
		{
			os << hex2str(inst.pc, 8) << " " << inst.name() << " ";
			inst.str(os);
			return os;
		}
	};
	
	struct JType : public Generic {
		int target() const { return shift_mask(other, 26, 0); }
		JType(const Generic& g) : Generic(g) {}
		std::ostream& str(std::ostream& os) const { return os << target(); }		
	};

	struct RType : public Generic {
		int rs_i() const { return shift_mask(other, 21, 5); }
		int rt_i() const { return shift_mask(other, 16, 5); }
		std::string rs() const { return registers[rs_i()]; }
		std::string rt() const { return registers[rt_i()]; }
		int imm_i() const { return shift_mask(other, 0, 16); }
		std::string imm() const { return hex2str(imm_i(), 4); }
		RType(const Generic& g) : Generic(g) {}
	};

	struct BranchEqualFmt : public RType {
		BranchEqualFmt(const Generic& g) : RType(g) {}
		std::ostream& str(std::ostream& os) const { return os << rs() << ", " << rt() << ", " << imm(); }						
	};
	struct BranchZeroFmt : public RType {
		BranchZeroFmt(const Generic& g) : RType(g) {}
		std::ostream& str(std::ostream& os) const { return os << rs() << ", " << imm(); }
	};

	struct LoadStoreInstruction : public RType {
		LoadStoreInstruction(const Generic& g) : RType(g) {}
		std::ostream& str(std::ostream& os) const { return os << rt() << ", " << imm() << "(" << rs() << ")"; }		
	};
	struct ImmediateALU : public RType {
		ImmediateALU(const Generic& g) : RType(g) {}
		std::ostream& str(std::ostream& os) const { return os << rt() << ", " << rs() << ", " << imm(); };
	};
	struct CoprocessorHeader : public JType {		
		CoprocessorHeader(const Generic& g) : JType(g) {}
		int cp() const { return shift_mask(op, 0, 2); }
		int coflag() const { return shift_mask(target(), 25, 1); }
	};
	struct CoprocessorOperation : public CoprocessorHeader {
		CoprocessorOperation(const Generic& g) : CoprocessorHeader(g) {}
		int cofunc() const { return shift_mask(target(), 0, 25); }
		std::ostream& str(std::ostream& os) const { return os << cofunc(); }
	};
	struct CoprocessorMove : public CoprocessorHeader {
		CoprocessorMove(const Generic& g) : CoprocessorHeader(g) {}
		int mvfunc() const { return shift_mask(target(), 0, 25); }
		int rt_i() const { return shift_mask(other, 16, 5); }		
		int rd_i() const { return shift_mask(other, 11, 5); }
		std::string rt() const { return registers[rt_i()]; }
		std::string rd() const { return registers[rd_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rt() << ", " << rd(); }
	};
	struct Coprocessor0TLB : public CoprocessorOperation {
		Coprocessor0TLB(Generic& g) : CoprocessorOperation(g) {}
		int tlbfunc() const { return shift_mask(other, 0, 6); }
		std::ostream& str(std::ostream& os) const { return os << tlbfunc(); }
	};
	struct Special : public Generic
	{
		static const int opcode = 000;
		Special(const Generic& g) : Generic(g) {}
		int sid() const { return shift_mask(other, 0, 6); }
	};
	struct ShiftAmount : public Special
	{
		ShiftAmount(const Generic& g) : Special(g) {}
		int rt_i() const { return shift_mask(other, 16, 5); }		
		int rd_i() const { return shift_mask(other, 11, 5); }
		std::string rt() const { return registers[rt_i()]; }
		std::string rd() const { return registers[rd_i()]; }
		int sa() const { return shift_mask(other, 6, 5); }
		std::ostream& str(std::ostream& os) const { return os << rd() << ", " << rt() << ", " << sa(); }		
	};
	struct ShiftRegister : public Special
	{
		ShiftRegister(const Generic& g) : Special(g) {}
		int rs_i() const { return shift_mask(other, 21, 5); }		
		int rt_i() const { return shift_mask(other, 16, 5); }		
		int rd_i() const { return shift_mask(other, 11, 5); }
		std::string rs() const { return registers[rs_i()]; }
		std::string rt() const { return registers[rt_i()]; }
		std::string rd() const { return registers[rd_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rd() << ", " << rt() << ", " << rs(); }		
	};
	struct JumpTarget : public Special
	{
		JumpTarget(const Generic& g) : Special(g) {}
		int rs_i() const { return shift_mask(other, 21, 5); }		
		std::string rs() const { return registers[rs_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rs(); }		
	};
	struct JumpRegister : public Special
	{
		JumpRegister(const Generic& g) : Special(g) {}
		int rs_i() const { return shift_mask(other, 21, 5); }		
		int rd_i() const { return shift_mask(other, 11, 5); }
		std::string rs() const { return registers[rs_i()]; }
		std::string rd() const { return registers[rd_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rd() << ", " << rs(); }
	};
	struct MoveHILO : public Special
	{
		MoveHILO(const Generic& g) : Special(g) {}
		int rd_i() const { return shift_mask(other, 11, 5); }
		std::string rd() const { return registers[rd_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rd(); }
	};
	struct DivMult : public Special
	{
		DivMult(const Generic& g) : Special(g) {}
		int rs_i() const { return shift_mask(other, 21, 5); }
		int rt_i() const { return shift_mask(other, 16, 5); }
		std::string rs() const { return registers[rs_i()]; }
		std::string rt() const { return registers[rt_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rs() << ", " << rt(); }
	};
	struct RegisterALU : public Special
	{
		RegisterALU(const Generic& g) : Special(g) {}
		int rs_i() const { return shift_mask(other, 21, 5); }
		int rt_i() const { return shift_mask(other, 16, 5); }
		int rd_i() const { return shift_mask(other, 11, 5); }
		std::string rs() const { return registers[rs_i()]; }
		std::string rt() const { return registers[rt_i()]; }
		std::string rd() const { return registers[rd_i()]; }
		std::ostream& str(std::ostream& os) const { return os << rd() << ", " << rs() << ", " << rt(); }
	};
	struct BcondOperation : public Generic
	{
		BcondOperation(const Generic& g) : Generic(g) {}
		int bid() const { return shift_mask(other, 16, 5); }
		int rs_i() const { return shift_mask(other, 21, 5); }
		std::string rs() const { return registers[rs_i()]; }
		int offset_i() const { return shift_mask(other, 0, 16); }
		std::string offset() const { return hex2str(offset_i(), 4); }
		std::ostream& str(std::ostream& os) const { return os << rs() << ", " << offset(); }
	};

#define DEFINE_INSTRUCTION(INST, TYPE, OP, OPNAME)		     \
	struct INST : public TYPE {				     \
		static const int OPNAME = OP;			     \
		INST(const Generic& g) : TYPE(g) {}		     \
		std::string name() const { return #INST; }	     \
	};
#define OPCODE(INST, TYPE, OP) DEFINE_INSTRUCTION(INST, TYPE, OP, opcode)
#define SPECIAL(INST, TYPE, SID)				     \
	struct CONCAT(S,INST) : public TYPE {			     \
		static const int sid = SID;			     \
		CONCAT(S,INST)(const Generic& g) : TYPE(g) {}	     \
		std::string name() const { return #INST; }	     \
	};
#define BCOND(INST, TYPE, BID) DEFINE_INSTRUCTION(INST, TYPE, BID, bid)
#define COP0(INST, TYPE, CO) DEFINE_INSTRUCTION(INST, TYPE, CO, co)
#define COP1(INST, TYPE, CO) DEFINE_INSTRUCTION(INST, TYPE, CO, co)
#define COP2(INST, TYPE, CO) DEFINE_INSTRUCTION(INST, TYPE, CO, co)
#define COP3(INST, TYPE, CO) DEFINE_INSTRUCTION(INST, TYPE, CO, co)
#define CM0(INST, TYPE, MVFUNC) DEFINE_INSTRUCTION(INST, TYPE, MVFUNC, mvfunc)
#define CM1(INST, TYPE, MVFUNC)	DEFINE_INSTRUCTION(INST, TYPE, MVFUNC, mvfunc)
#define CM2(INST, TYPE, MVFUNC) DEFINE_INSTRUCTION(INST, TYPE, MVFUNC, mvfunc)
#define CM3(INST, TYPE, MVFUNC) DEFINE_INSTRUCTION(INST, TYPE, MVFUNC, mvfunc)
#define CP0TL(INST, TYPE, TLBFUNC) DEFINE_INSTRUCTION(INST, TYPE, TLBFUNC, tlbfunc)
#include "r3000a.h"
#undef OPCODE
#undef SPECIAL
#undef BCOND
#undef COP0
#undef COP1
#undef COP2
#undef COP3
#undef CM0
#undef CM1
#undef CM2
#undef CM3
#undef CP0TL
};

namespace PS1Disassemble
{
	template <class T> void process(const T& inst) { *outstream << inst << "\n"; }
	template <> void process(const R3000AInstruction::bcond& b);
	template <> void process(const R3000AInstruction::specl& s);
	template <> void process(const R3000AInstruction::ch0&);
	template <> void process(const R3000AInstruction::cm0&);
	template <> void process(const R3000AInstruction::ch1&);
	template <> void process(const R3000AInstruction::cm1&);
	template <> void process(const R3000AInstruction::cm2&);
	template <> void process(const R3000AInstruction::ch2&);
	template <> void process(const R3000AInstruction::ch3&);
	template <> void process(const R3000AInstruction::cm3&);

#define DEFINE_MAP_ENTRY(INST, TYPE_PARENT, TYPE, OP) { OP, [](const R3000AInstruction::TYPE_PARENT& p) { R3000AInstruction::INST i(p); process(i); } },

	std::map<int, void(*)(const R3000AInstruction::bcond&)> process_bcond_map = {
#define BCOND(INST, TYPE, BID) DEFINE_MAP_ENTRY(INST, bcond, TYPE, BID)
#include "r3000a.h"
#undef BCOND
	};

	std::map<int, void(*)(const R3000AInstruction::specl&)> process_specl_map = {
		// #define SPECIAL(INST, TYPE, SID) DEFINE_MAP_ENTRY(INST, specl, TYPE, SID)
#define SPECIAL(INST, TYPE, SID) { SID, [](const R3000AInstruction::specl& p) { R3000AInstruction::CONCAT(S,INST) i(p); process(i); } },
#include "r3000a.h"
#undef SPECIAL
	};

	std::map<int, void(*)(const R3000AInstruction::Generic&)> process_map = {
#define OPCODE(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, Generic, TYPE, OP)
#include "r3000a.h"
#undef OPCODE	
	};

	std::map<int, void(*)(const R3000AInstruction::ch0&)> process_ch0_map = {
#define COP0(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, ch0, TYPE, OP)
#include "r3000a.h"
#undef COP0
	};
	std::map<int, void(*)(const R3000AInstruction::cm0&)> process_cm0_map = {
#define CM0(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, cm0, TYPE, OP)
#include "r3000a.h"
#undef CM0
	};

	std::map<int, void(*)(const R3000AInstruction::ch1&)> process_ch1_map = {
#define COP1(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, ch1, TYPE, OP)
#include "r3000a.h"
#undef COP1
	};
	std::map<int, void(*)(const R3000AInstruction::cm1&)> process_cm1_map = {
#define CM1(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, cm1, TYPE, OP)
#include "r3000a.h"
#undef CM1
	};

	std::map<int, void(*)(const R3000AInstruction::ch2&)> process_ch2_map = {
#define COP2(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, ch2, TYPE, OP)
#include "r3000a.h"
#undef COP2
	};
	std::map<int, void(*)(const R3000AInstruction::cm2&)> process_cm2_map = {
#define CM2(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, cm2, TYPE, OP)
#include "r3000a.h"
#undef CM2
	};

	std::map<int, void(*)(const R3000AInstruction::ch3&)> process_ch3_map = {
#define COP3(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, ch3, TYPE, OP)
#include "r3000a.h"
#undef COP3
	};
	std::map<int, void(*)(const R3000AInstruction::cm3&)> process_cm3_map = {
#define CM3(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, cm3, TYPE, OP)
#include "r3000a.h"
#undef CM3
	};

	std::map<int, void(*)(const R3000AInstruction::CoprocessorOperation&)> process_c0tlb_map = {
#define COP0TLB(INST, TYPE, OP) DEFINE_MAP_ENTRY(INST, CoprocessorOperation, TYPE, OP)
#include "r3000a.h"
#undef COP0TLB
	};
		template <> void process(const R3000AInstruction::ch0& c) { 
			const R3000AInstruction::CoprocessorOperation co((c));
			if ( c.coflag() == 1 && process_c0tlb_map.find(co.cofunc()) != process_c0tlb_map.end() ) { 
				process_c0tlb_map.at(co.cofunc())(co);
			} else { 
				process_ch0_map.at(c.coflag())(c);
			}
		}

template <> void process(const R3000AInstruction::cm0& c) { process_cm0_map.at(c.mvfunc())(c); }
template <> void process(const R3000AInstruction::ch1& c) { process_ch1_map.at(c.coflag())(c); }
template <> void process(const R3000AInstruction::cm1& c) { process_cm1_map.at(c.mvfunc())(c); }
template <> void process(const R3000AInstruction::ch2& c) { process_ch2_map.at(c.coflag())(c); }
template <> void process(const R3000AInstruction::cm2& c) { process_cm2_map.at(c.mvfunc())(c); }
template <> void process(const R3000AInstruction::ch3& c) { process_ch3_map.at(c.coflag())(c); }
template <> void process(const R3000AInstruction::cm3& c) { process_cm3_map.at(c.mvfunc())(c); }
template <> void process(const R3000AInstruction::Generic& g) { process_map.at(g.op)(g); }
template <> void process(const R3000AInstruction::bcond& b) { process_bcond_map.at(b.bid())(b); }	
template <> void process(const R3000AInstruction::specl& s) { process_specl_map.at(s.sid())(s); }

	class UnalignedPCBeginException {};
	class UnalignedPCEndException {};
	static void disassemble(const u8* data, u32 pc_start, u32 pc_end) {
		if ( (unsigned long long)pc_start & 0x3 ) throw UnalignedPCBeginException();
		if ( (unsigned long long)pc_end & 0x3 ) throw UnalignedPCEndException();
		for ( u32 pc = pc_start ; pc < pc_end; pc += 4 ) {
			R3000AInstruction::Generic inst(*(u32*)(data + pc), pc);
			try {
				process(inst);
			} catch (std::out_of_range&) {
				*outstream << inst << "\n";
			}
		}
	}
};	

int main(int argc, char** argv)
{
	if ( argc < 2 ) {
		std::cout << "Usage: " << argv[0] << " <PS1_EXE> [OUTPUT FILE]\n";
		return 1;
	}
	
	if ( argc >= 3) {
		outfile  = std::ofstream(argv[2]);
		outstream = &outfile;
	}
	else outstream = &std::cout;
	
	int i = 0;
	std::string ps1_exe = argv[++i]; *outstream << "ps1_exe: " << ps1_exe << "\n";
	PS1File f(ps1_exe);
	*outstream << *f.h << "\n";
	*outstream << "------\n";

	u32 pc_start = f.text_section() - f.data;
	PS1Disassemble::disassemble(f.data, pc_start, f.data_sz);
		
	return 0;
}
