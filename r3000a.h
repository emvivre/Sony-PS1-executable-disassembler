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
#ifdef OPCODE
OPCODE(specl, Special,              000)
OPCODE(bcond, BcondOperation,       001)
OPCODE(j,     JType,                002)
OPCODE(jal,   JType,                003)
OPCODE(beq,   BranchEqualFmt,       004)
OPCODE(bne,   BranchEqualFmt,       005)
OPCODE(blez,  BranchZeroFmt,        006)
OPCODE(bgtz,  BranchZeroFmt,        007)
OPCODE(addi,  ImmediateALU,         010)
OPCODE(addiu, ImmediateALU,         011)
OPCODE(slti,  ImmediateALU,         012)
OPCODE(sltiu, ImmediateALU,         013)
OPCODE(andi,  ImmediateALU,         014)
OPCODE(ori,   ImmediateALU,         015)
OPCODE(xori,  ImmediateALU,         016)
OPCODE(lui,   LoadStoreInstruction, 017)
OPCODE(ch0,   CoprocessorHeader,    020)
OPCODE(ch1,   CoprocessorHeader,    021)
OPCODE(ch2,   CoprocessorHeader,    022)
OPCODE(ch3,   CoprocessorHeader,    023)
OPCODE(lb,    LoadStoreInstruction, 040)
OPCODE(lh,    LoadStoreInstruction, 041)
OPCODE(lwl,   LoadStoreInstruction, 042)
OPCODE(lw,    LoadStoreInstruction, 043)
OPCODE(lbu,   LoadStoreInstruction, 044)
OPCODE(lhu,   LoadStoreInstruction, 045)
OPCODE(lwr,   LoadStoreInstruction, 046)
OPCODE(sb,    LoadStoreInstruction, 050)
OPCODE(sh,    LoadStoreInstruction, 051)
OPCODE(swl,   LoadStoreInstruction, 052)
OPCODE(sw,    LoadStoreInstruction, 053)
OPCODE(swr,   LoadStoreInstruction, 056)
OPCODE(lwc0,  LoadStoreInstruction, 060)
OPCODE(lwc1,  LoadStoreInstruction, 061)
OPCODE(lwc2,  LoadStoreInstruction, 062)
OPCODE(lwc3,  LoadStoreInstruction, 063)
OPCODE(swc0,  LoadStoreInstruction, 070)
OPCODE(swc1,  LoadStoreInstruction, 071)
OPCODE(swc2,  LoadStoreInstruction, 072)
OPCODE(swc3,  LoadStoreInstruction, 073)
#endif

#ifdef BCOND
BCOND(bltz,   bcond, 000)
BCOND(bgez,   bcond, 001)
BCOND(bltzal, bcond, 020)
BCOND(bgezal, bcond, 021)
#endif

#ifdef SPECIAL
SPECIAL(sll,     ShiftAmount,   000)
SPECIAL(srl,     ShiftAmount,   002)
SPECIAL(sra,     ShiftAmount,   003)
SPECIAL(sllv,    ShiftRegister, 004)
SPECIAL(srlv,    ShiftRegister, 006)
SPECIAL(srav,    ShiftRegister, 007)
SPECIAL(jr,      JumpTarget,    010)
SPECIAL(jalr,    JumpRegister,  011)
SPECIAL(syscall, specl,         014)
SPECIAL(break,   specl,         015)
SPECIAL(mfhi,    MoveHILO,      020)
SPECIAL(mthi,    MoveHILO,      021)
SPECIAL(mflo,    MoveHILO,      022)
SPECIAL(mtlo,    MoveHILO,      023)
SPECIAL(mult,    DivMult,       030)
SPECIAL(multu,   DivMult,       031)
SPECIAL(div,     DivMult,       032)
SPECIAL(divu,    DivMult,       033)
SPECIAL(add,     RegisterALU,   040)
SPECIAL(addu,    RegisterALU,   041)
SPECIAL(sub,     RegisterALU,   042)
SPECIAL(subu,    RegisterALU,   043)
SPECIAL(and,     RegisterALU,   044)
SPECIAL(or,      RegisterALU,   045)
SPECIAL(xor,     RegisterALU,   046)
SPECIAL(nor,     RegisterALU,   047)
SPECIAL(slt,     RegisterALU,   052)
SPECIAL(sltu,    RegisterALU,   053)
#endif

#ifdef COP0
COP0(cop0, CoprocessorOperation, 1)
COP0(cm0,  CoprocessorMove,      0)
#endif

#ifdef COP1
COP1(cop1, CoprocessorOperation, 1)
COP1(cm1,  CoprocessorMove,      0)
#endif

#ifdef COP2
COP2(cop2, CoprocessorOperation, 1)
COP2(cm2 , CoprocessorMove,      0)
#endif

#ifdef COP3
COP3(cop3, CoprocessorOperation, 1)
COP3(cm3 , CoprocessorMove,      0)
#endif

#ifdef CM0
CM0(mfc0, CoprocessorMove, 000)
CM0(mtc0, CoprocessorMove, 004)
#endif

#ifdef CM1
CM1(mfc1, CoprocessorMove, 000)
CM1(mtc1, CoprocessorMove, 004)
CM1(cfc1, CoprocessorMove, 002)
CM1(ctc1, CoprocessorMove, 006)
#endif

#ifdef CM2
CM2(mfc2, CoprocessorMove, 000)
CM2(mtc2, CoprocessorMove, 004)
CM2(cfc2, CoprocessorMove, 002)
CM2(ctc2, CoprocessorMove, 006)
#endif

#ifdef CM3
CM3(mfc3, CoprocessorMove, 000)
CM3(mtc3, CoprocessorMove, 004)
CM3(cfc3, CoprocessorMove, 002)
CM3(ctc3, CoprocessorMove, 006)
#endif

#ifdef COP0TL
COP0TL(tlbr, Coprocessor0TLB, 001)
COP0TL(tlwi, Coprocessor0TLB, 002)
COP0TL(tlwr, Coprocessor0TLB, 006)
COP0TL(tlbp, Coprocessor0TLB, 010)
COP0TL(rfe,  Coprocessor0TLB, 020)
#endif

