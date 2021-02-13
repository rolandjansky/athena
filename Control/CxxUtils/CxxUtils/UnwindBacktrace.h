// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/UnwindBacktrace.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2021
 * @brief Hacked backtrace that can go past a bad stack frame.
 *
 * (Following is mostly specific to linux-x86_64 platforms.)
 *
 * backtrace() from glibc is implemented in terms of _Unwind_Backtrace
 * from libgcc, which in turn uses the same mechanism that unwinds
 * the stack during exception handling.
 *
 * Each function invocation is associated with a stack frame
 * (sometimes referred to as the Canonical Frame Address, or CFA).
 * From this, we can get the return address for the function and
 * the next CFA.
 *
 * On 32-bit x86 platforms, this is easy-peasy: the bp register points
 * at the current stack frame, and all the frames are maintained
 * in a linked list.
 *
 * However, keeping the current frame in bp is not strictly required.
 * At any point in the code, the compiler knows, statically, the offset
 * between sp and the current stack frame, so it can just as well use sp
 * rather than bp.  And that is in fact what is done for x86_64, allowing
 * rbp to be used as another general-purpose register.
 *
 * The problem is then that if we are given the program in some arbitrary
 * state and want to unwind the stack, we don't know how to find the
 * frame pointer without help from the compiler.  So the compiler provides it,
 * in the .eh_frame section.
 *
 * For each function, the compiler emits an object called a Frame Description
 * Entry (FDE).  This can be looked up by the instruction pointer.
 * FDEs are grouped together in sets described by a Common Information
 * Entry (CIE).  A FDE points at the corresponding CDE, and the CDE and FDE
 * together include a set of instructions telling how, at any point in the
 * function, to find the CFA and the return address (as well as any other
 * saved registers which must be restored during unwinding).
 *
 * Then the difficulty is what happens if we end up with an invalid
 * stack frame.  This could happen, for example, if we try to call
 * a virtual function on a deleted object.  When we get to such a frame,
 * we won't be able to find a FDE for it.  The backtrace()/_Unwind_Backtrace
 * code will just stop unwinding at that point, declaring it the end
 * of the stack.
 *
 * For exception handling, this is no problem.  The program is anyway
 * off in the weeds of undefined behavior at this point, so in some
 * sense it doesn't matter what the unwinder does, but shutting things
 * down expediously is probably a good strategy.  But from the point
 * of view of trying to collect diagnostics after a crash, this is plus ungood,
 * as it ends up hiding the actual location of the error.
 *
 * There is, however, an interface for registering new FDEs with the library,
 * __register_frame_info and friends.  This was used when loading a dynamic
 * library (nowadays a more efficient mechanism is used), but can also
 * be used by JITters to allow dynamically generated code to interoperate
 * with exception handling.  So: we we use _Unwind_Backtrace to get
 * a stack trace.  If it encounters a bad stack frame, we synthesize
 * a dummy FDE for it, register it, and try again.  (We only allow
 * doing this once, though.)
 *
 * A technical complication is that when libgcc tries to use a newly-registered
 * FDE for the first time, it will sort the entries, putting the result
 * in a vector obtained from malloc().  But we really want to avoid
 * doing memory allocation here (we may have crashed due to a corrupt
 * heap, and calling malloc will just crash again).  So after registering
 * the FDE, we diddle the correspoding data structures so that libgcc
 * things the sorting has been done.  This bit is the naughtiest
 * part of the whole enterprise, as it relies entirely on knowing the
 * private internals of libgcc.
 *
 * References:
 *  DWARF debugging information format version 5  <dwarfstd.org>,
 *  particularly section 6.4, `Call Frame Information'.
 *
 *  System V ABI, AMD64 Architecture Supplement 
 *  <https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-1.0.pdf>,
 *  particularly setion 6.2 `Unwind Libary Interface'.
 *
 *  The libgcc/ directory in the gcc distribution and unwind.h.
 */


#ifndef CXXUTILS_UNWINDBACKTRACE_H
#define CXXUTILS_UNWINDBACKTRACE_H


// Enable this only for gcc + linux + x86_64
#if defined(__GNUC__) && !defined(__clang__) && defined(__linux__) && defined(__x86_64__)
# define HAVE_LINUX_UNWIND_BACKTRACE 1
#endif


#ifdef HAVE_LINUX_UNWIND_BACKTRACE


#include "CxxUtils/SealCommon.h" // For IOFD


namespace CxxUtils {


/// Type of a function to be called for each line.
/// It gets the file descriptor for output and the return address.
typedef void backtraceLineFn (IOFD fd, unsigned long addr);


/**
 * @brief Generate a backtrace using _UnwindBacktrace,
 *        trying to handle a possible bad stack frame.
 * @param lineFn Function to call for each line.
 * @param fd File descriptor for output.
 */
void backtraceByUnwind (backtraceLineFn* lineFn, IOFD fd);

  
} // namespace CxxUtils


#endif


#endif // not CXXUTILS_UNWINDBACKTRACE_H
