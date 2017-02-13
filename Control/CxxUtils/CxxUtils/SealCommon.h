/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/SealCommon.h
 * @author Wim Lavrijsen <WLavrijsen@lbl.gov>
 * @date Oct, 2008
 * @brief Collecting a few shared bits and pieces from SEAL headers.
 */

#ifndef CXXUTILS_SEAL_COMMON_H
#define CXXUTILS_SEAL_COMMON_H

// start copy from "SealBase/sysapi/IOTypes.h"
/** Invalid channel descriptor constant.  */
#ifdef _WIN32
#define IOFD_INVALID (void *)-1
#else
#define IOFD_INVALID -1
#endif

/** Type the system uses for channel descriptors.  */
#ifdef _WIN32
typedef void *IOFD;
#else
typedef int   IOFD;
#endif
// end copy from IOTypes


// platform specific defines from SealPlatform/config.h
#ifdef __linux

/* C++ features --------------------------------------------------------- */

/* Define if you have standard C++ C headers like <cstdlib>. */
#define HAVE_CXX_STDC_HEADERS 1

/* Signals -------------------------------------------------------------- */

/* Define if you have POSIX signal stuff.  */
#define HAVE_POSIX_SIGNALS 1

/* Define if you have POSIX real-time signal stuff.  */
#define HAVE_POSIX_RT_SIGNALS 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1
#define HAVE_STRSIGNAL_DECL 1

/* Define if you have raise. */
#define HAVE_RAISE 1
#define HAVE_RAISE_DECL 1

/* Define if you have sys_siglist in <signal.h>.  */
#define HAVE_SYS_SIGLIST 1

/* Resource information ------------------------------------------------- */

/* Define if you have program_invocation_name.  */
#define HAVE_PROGRAM_INVOCATION_NAME 1

/* Stack tracing -------------------------------------------------------- */

/* Define if you have pstack (usually in /usr/proc/bin) to the full
   path of that program.  */
#define PROG_PSTACK "/usr/bin/pstack"

/* Define if you have c++filt to the full path of that program.  */
#define PROG_CXXFILT "/usr/bin/c++filt"

/* Define if you have backtrace and backtrace_symbols_fd (glibc 2.1).  */
#define HAVE_BACKTRACE_SYMBOLS_FD 1

/* Dynamic linker ------------------------------------------------------- */

/* Define if `dlopen' exists. */
#define HAVE_DLOPEN 1
#define HAVE_DLOPEN_DECL 1

/* Define if you have the <link.h> header file.  */
#define HAVE_LINK_H 1

/* Define if `dladdr' exists. */
#define HAVE_DLADDR 1
#define HAVE_DLADDR_DECL 1

/* Define if `_r_debug' exists. */
// note: leave undefined on SLC4 b/c of binutils problems
#define HAVE_R_DEBUG 1

/* Define if you have the <elf.h> header file.  */
#define HAVE_ELF_H 1

#endif // __linux


// platform specific defines from SealPlatform/config.h
#ifdef __APPLE__

/* C++ features --------------------------------------------------------- */
#define HAVE_R_DEBUG 1

/* Define if you have standard C++ C headers like <cstdlib>. */
#define HAVE_CXX_STDC_HEADERS 1

/* Signals -------------------------------------------------------------- */

/* Define if you have POSIX signal stuff.  */
#define HAVE_POSIX_SIGNALS 1

/* Define if you have the strsignal function.  */
#define HAVE_STRSIGNAL 1
#define HAVE_STRSIGNAL_DECL 1

/* Define if you have raise. */
#define HAVE_RAISE 1
#define HAVE_RAISE_DECL 1

/* Define if you have sys_siglist in <signal.h>.  */
#define HAVE_SYS_SIGLIST 1

/* Stack tracing -------------------------------------------------------- */

/* Define if you have c++filt to the full path of that program.  */
#define PROG_CXXFILT "/usr/bin/c++filt"

/* Define if you have backtrace and backtrace_symbols_fd (glibc 2.1).  */
#define HAVE_BACKTRACE_SYMBOLS_FD 1

/* Dynamic linker ------------------------------------------------------- */

/* Define if `dlopen' exists. */
#define HAVE_DLOPEN 1
#define HAVE_DLOPEN_DECL 1

/* Define if `dladdr' exists. */
#define HAVE_DLADDR 1
#define HAVE_DLADDR_DECL 1

/* Define if you have the <mach-o/dyld.h> header file.  */
#define HAVE_MACH_O_DYLD_H 1

#endif // __APPLE__


#if HAVE_LOAD
# define PATH		"LIBPATH"
#elif HAVE_DLOPEN
# if defined __APPLE__ && defined __MACH__
#  define PATH		"DYLD_LIBRARY_PATH"
# else
#  define PATH		"LD_LIBRARY_PATH"
# endif
#elif HAVE_SHL_LOAD
# define PATH		"SHLIB_PATH"
#elif defined _WIN32
# define PATH		"PATH"
#else
# define PATH		0
#endif
// end copy from SealBase/sysapi/SharedLibrary.h


// start copy from SealPlatform/system.h
#if HAVE_CXX_STDC_HEADERS
# define STDC std
#else
# define STDC
#endif
// end copy from SealPlatform/system.h


// start copy from SealBase/sysapi/ElfAbi.h
# ifndef _WIN32
#  if HAVE_LOADER_H
#   include <loader.h>
#  endif
#  if HAVE_LINK_H
#   include <link.h>
#   include <limits.h>
#   include <sys/stat.h>
#   include <unistd.h>
#  endif
#  if HAVE_SGIDEFS_H    // irix n32, 64
#   include <sgidefs.h>
#   include <objlist.h>
#   include <obj_list.h>
#   include <obj.h>
#  endif
#  if HAVE_ELF_H
#   include <elf.h>
#  endif
# endif // ! _WIN32

//<<<<<< PUBLIC DEFINES                                                 >>>>>>

#if /* irix */       (defined ABI64 || defined _ABI64 ||                \
                      (defined _MIPS_SIM && _MIPS_SIM == _MIPS_SIM_ABI64)) \
    /* solaris */ || (defined sparcv9 || defined _sparcv9               \
                      || defined __sparcv9 || defined __sparcv9__)      \
    /* tru64 */   || (defined arch64 || defined _arch64 ||              \
                      defined __arch64 || defined __arch64__)
# define ELF_ABI 64
#else
# define ELF_ABI 32
#endif
#ifndef ElfW
# define ElfW(type)   ElfW1(Elf,ELF_ABI,type)
# define ElfW1(e,w,t) ElfW2(Elf,w,_##t)
# define ElfW2(e,w,t) e ## w ## t
#endif
// end copy from SealBase/sysapi/ElfAbi.h

#endif // CXXUTILS_SEAL_COMMON_H
