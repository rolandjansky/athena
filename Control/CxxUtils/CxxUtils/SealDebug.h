/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/SealDebug.h
 * @author Lassi Tuura (original author)
 * @author Wim Lavrijsen <WLavrijsen@lbl.gov> (responsible in ATLAS)
 * @date Oct, 2008
 * @brief This are the SEAL debug aids, adapted to build in Atlas,
 *        after the drop of that project.
 *
 *        Search for `wlav' to find changes from the SEAL version. I
 *        also dropped all ASSERT macro's in favor of assert. Removed
 *        logstream references.
 *
 *        sss: Add stacktraceLine.
 */

#ifndef CXXUTILS_SEAL_DEBUG_H // wlav SEAL_BASE_DEBUG_H
#define CXXUTILS_SEAL_DEBUG_H // wlav SEAL_BASE_DEBUG_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "CxxUtils/checker_macros.h"
#include "CxxUtils/SealCommon.h"  // sss -- needed for IOFD
//# include "SealBase/Macros.h"                   wlav
//# include "SealBase/sysapi/IOTypes.h"           wlav
# include <cstddef>

// wlav copied from SealBase/sysapi/DebugAids.h
// Windows doesn't have this, so fake a suitable substitute
# ifdef _WIN32
#  define STDERR_HANDLE GetStdHandle (STD_ERROR_HANDLE)
# else
#  define STDERR_HANDLE STDERR_FILENO
# endif

// Define a suitable wrapper to write to system file descriptors.
// This is needed because on Windows we are using HANDLEs, not the
// compiler's crippled posixy interface.
# ifdef _WIN32
#  define MYWRITE(fd,data,n)    do { DWORD written; WriteFile(fd,data,n,\
                                        &written,0); } while (0)
# else
#  define MYWRITE(fd,data,n)    write(fd,data,n)
# endif


//namespace seal {                                wlav
namespace Athena {                             // wlav
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/** Utilities for debugging support.  */
class DebugAids
{
public:
    // Miscellaneous functions
    static IOFD			stacktraceFd ATLAS_NOT_THREAD_SAFE (IOFD fd = IOFD_INVALID);
    static void			stacktrace ATLAS_NOT_THREAD_SAFE (IOFD fd = IOFD_INVALID);
    static void			coredump (int sig, ...);
    // sss
    static void                 stacktraceLine ATLAS_NOT_THREAD_SAFE (IOFD fd,
                                                                      unsigned long addr);
    static void                 setStackTraceAddr2Line ATLAS_NOT_THREAD_SAFE (const char* path);

private:
    static IOFD			s_stackTraceFd;
};

//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

//} // namespace seal                             wlav
} // namespace Athena                             wlav
#endif // CXXUTILS_SEAL_DEBUG_H wlav SEAL_BASE_DEBUG_H
