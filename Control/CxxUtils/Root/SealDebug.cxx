/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/src/SealDebug.cxx
 * @author Lassi Tuura (original author)
 * @author Wim Lavrijsen <WLavrijsen@lbl.gov> (responsible in ATLAS)
 * @date Oct, 2008
 *
 *        Search for `wlav' to find changes from the SEAL version. All
 *        includes were modified, all ASSERT macro's were dropped in
 *        favor of assert, only stack trace functionality was kept.
 *
 *        To comply with coverity rules, changed sprintf -> snprintf
 *        (wlav, 03/11/11)
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "CxxUtils/SealCommon.h"               // wlav
#include "CxxUtils/SealDebug.h"                // wlav
#include "CxxUtils/SealSignal.h"               // wlav
#include "CxxUtils/SealSharedLib.h"            // wlav
#include "CxxUtils/checker_macros.h"

// wlav copied from SealBase/sysapi/DebugAids.h
#include <cstring>
# include <cctype>
# include <cstdio>
# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <sstream>                            // wlav
# include <climits>                            // wlav

# ifdef _WIN32
#  include <windows.h>
#  include <winnt.h>
#  include <imagehlp.h>
//#  include <io.h>
# else
#  include <unistd.h>
#  include <sys/wait.h>
#  if HAVE_BACKTRACE_SYMBOLS_FD		// GNU
#   include <execinfo.h>
#   include <sys/uio.h>
#   include <cxxabi.h>
#  endif
#  if HAVE_DLADDR			// Linux, Solaris
#   include <dlfcn.h>
#  endif
# if HAVE_EXCEPTION_H
    // This is yucky.  KCC's <exception.h> that has nothing to do with the
    // header we are looking for (it redirect to <exception>).  This ugly
    // workaround allows us to find the (IRIX) header we are looking for.
#   if defined __KCC && defined __sgi
#     include </usr/include/exception.h>
#    elif defined __sgi
#     include <exception.h>
#    endif
#  endif
#  if HAVE_EXCPT_H			// IRIX
#   include <excpt.h>
#   undef try	  // Defined on SGI to structured exception handling goop
#   undef catch   // Defined on SGI to structured exception handling goop
#  endif
#  if HAVE_RLD_INTERFACE_H		// Tru64 (IRIX)
#   include <rld_interface.h>
#  endif
#  if HAVE_PDSC_H			// Tru64
#   include <pdsc.h>
#  endif
#  if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) // GCC 3.4+ C++ ABI
#   include <sys/uio.h>
#  endif
# endif

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
#  define MYWRITE(fd,data,n)	do { DWORD written; WriteFile(fd,data,n,\
					&written,0); } while (0)
# else
#  define MYWRITE(fd,data,n)	write(fd,data,n)
# endif

// Helper to write literals
# define MYWRITELIT(fd,str) MYWRITE(fd,str,sizeof(str)-1)

//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>

#if HAVE_BACKTRACE_SYMBOLS_FD
/** The maximum stack trace depth for systems where we request the
    stack depth separately (GNU libc-based systems).  */
static const int MAX_BACKTRACE_DEPTH = 128;
#endif


#if HAVE_BACKTRACE_SYMBOLS_FD && HAVE_DLADDR
// sss
#include <unistd.h>
#include <errno.h>
namespace {


std::string addr2LinePath = "/usr/bin/addr2line";


struct ATLAS_NOT_THREAD_SAFE BacktraceInit
{
  BacktraceInit()
  {
    // backtrace() has a one-time initialization that uses malloc().
    // so call it once now.
    void* trace[1];
    backtrace (trace, 1);

    // Also test for eu-addr2line.
    if (access ("/usr/bin/eu-addr2line", F_OK) == 0)
      addr2LinePath = "/usr/bin/eu-addr2line";
  }
};
BacktraceInit backtraceInit;


// This is like popen, except that it returns a fd rather
// than a FILE*.  The PID is returned in pid.
// This is to avoid memory allocation.
int stacktracePopenFD (const char* cmd, pid_t& child_pid)
{
  int stat;
  int fds[2];

  // The glibc popen() uses pipe2() here with O_CLOEXEC.
  // pipe2() is linux-specific, though, so avoid it here.
  stat = pipe (fds);
  if (stat < 0) return stat;

  int parent_end = fds[0];
  int child_end = fds[1];

  child_pid = fork();
  if (child_pid == 0) {
    int child_std_end = 1;
    close (parent_end);
    if (child_end != child_std_end) {
      dup2 (child_end, child_std_end);
      close (child_end);
    }

    /* POSIX.2:  "popen() shall ensure that any streams from previous
       popen() calls that remain open in the parent process are closed
       in the new child process."

       For our specific case here, we ignore this. */
    
    execl ("/bin/sh", "sh", "-c", cmd, (char *) 0);
    _exit (127);
  }

  close (child_end);
  if (child_pid < 0) {
    close (parent_end);
    return child_pid;
  }

  return parent_end;
}


int stacktracePcloseFD (int fd, pid_t child_pid)
{
  int stat = close (fd);
  if (stat < 0) return stat;

  /* POSIX.2 Rationale:  "Some historical implementations either block
     or ignore the signals SIGINT, SIGQUIT, and SIGHUP while waiting
     for the child process to terminate.  Since this behavior is not
     described in POSIX.2, such implementations are not conforming." */
  pid_t wait_pid;
  int wstatus;
  do {
    wait_pid = waitpid (child_pid, &wstatus, 0);
  } while (wait_pid == -1 && errno == EINTR);

  if (wait_pid == -1)
    return -1;
  return wstatus;
}


int stacktraceReadline (int fd, char* buf, int buflen)
{
  int len = 0;
  while (len < buflen-1) {
    int stat = read (fd, buf, 1);
    if (stat < 0) return stat;
    if (stat == 0) break;
    if (*buf == '\n') break;
    ++len;
    ++buf;
  }
  *buf = '\0';
  return len;
}


} // anonymous namespace
// sss
#endif


//namespace seal {                                wlav
namespace Athena {                             // wlav
//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

/** The default output file descriptor for #stacktrace().  */
IOFD			DebugAids::s_stackTraceFd = IOFD_INVALID;

//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>

#ifdef _WIN32
// /** WIN32 function to grab the current PC address from the SEH context.
//     We need this to grab the exception context so we can walk the stack
//     in #Debug::stacktrace().  We use SEH (as compiler-independently as
//     we can) as only XP 64-bit has RtlGetContext() function.  */
// static LONG CALLBACK
// GrabExceptionContext (PEXCEPTION_POINTERS info)
// {
//     *((CONTEXT *) info->ExceptionRecord->ExceptionInformation[0])
// 	= *info->ContextRecord;
//     return EXCEPTION_EXECUTE_HANDLER;
// }

/** Helper function to translate the virtual PC address @a addr into a
    logical address.  If the address translates to a known module (DLL
    or executable) memory mapping range, fills @a name (of maximum
    length @a length) with the name of the module, sets @a section to
    the index of the memory mapping section within the module, @a
    offset to a @a addr's relative offset within @a section, and
    returns @c true.  Otherwise returns @a false and @a name, @a
    section and @a offset will have undefined values.  Used to
    translate PC addresses to module addresses during the stack walk.  */
bool
GetLogicalAddress (PVOID addr, PTSTR name, DWORD length,
		   DWORD &section, DWORD &offset)
{
    MEMORY_BASIC_INFORMATION info;

    if (! VirtualQuery (addr, &info, sizeof (info)))
	return false;

    DWORD module = (DWORD) info.AllocationBase;
    if (! GetModuleFileName ((HMODULE) module, name, length))
	return false;

    PIMAGE_DOS_HEADER		dosheader = (PIMAGE_DOS_HEADER) module;
    PIMAGE_NT_HEADERS		ntheader
	= (PIMAGE_NT_HEADERS) (module + dosheader->e_lfanew);
    PIMAGE_SECTION_HEADER	sect = IMAGE_FIRST_SECTION (ntheader);
    DWORD			rva = (DWORD) addr - module;

    for (unsigned i = 0; i < ntheader->FileHeader.NumberOfSections; ++i,++sect)
    {
	DWORD sect_start = sect->VirtualAddress;
	DWORD sect_end = sect_start + std::max (sect->SizeOfRawData,
					        sect->Misc.VirtualSize);

	if ((rva >= sect_start) && (rva <= sect_end))
	{
	    section = i+1;
	    offset = rva - sect_start;
	    return true;
	}
    }

    assert (false);
    return false;
}
#endif

/**
 * Write out stack trace line to FD.
 * IP is the instruction pointer.
 * (sss)
 */
void DebugAids::stacktraceLine ATLAS_NOT_THREAD_SAFE (IOFD fd,
                                                      unsigned long addr)
{
  iovec		bufs [7];
  int		nbufs = 0;
  const int     addrbuf_size = 5 + BitTraits<unsigned long>::HexDigits;
  char		addrbuf [addrbuf_size];

#if HAVE_BACKTRACE_SYMBOLS_FD && HAVE_DLADDR
  const int     diffbuf_size = 15 + BitTraits<unsigned long>::HexDigits;
  char		diffbuf [diffbuf_size];
  static const char	trailer [] = "]\n";
  Dl_info		info;

  if (dladdr ((void*)addr, &info) && info.dli_fname && info.dli_fname[0])
  {
    const char *libname = info.dli_fname;
           
    unsigned long	symaddr = (unsigned long) info.dli_saddr;
    bool		gte = (addr >= symaddr);
    unsigned long	diff = (gte ? addr - symaddr : symaddr - addr);

    // RS start
    int length = 0;
    
    const int     relbuf_size = 7 + BitTraits<unsigned long>::HexDigits;
    char	  relbuf [relbuf_size];
    
    // difference of two pointers
    unsigned long libaddr = (unsigned long) info.dli_fbase;
    unsigned long relative_address = (addr >= libaddr) ? addr - libaddr : libaddr - addr;
    if (strstr (info.dli_fname, ".so") == 0)
      relative_address = addr;

    // need popen for addr2line ...
    int pfd;
    pid_t child_pid;
    char line[ LINE_MAX ];
    char dembuf[ LINE_MAX ];
    const char* symname = dembuf;
    size_t demlen = 0;

    // did we find valid entry ?
    size_t len = strlen(info.dli_fname);
    if ( len > 0 && len + 80 < LINE_MAX)
    {
      if (getenv ("LD_PRELOAD"))
        unsetenv ("LD_PRELOAD");

      if ( addr2LinePath == "/usr/bin/eu-addr2line" )
	{
	  snprintf (line, LINE_MAX, "%s -f -e %s %p | /usr/bin/c++filt | /usr/bin/tr \\\\012 \\\\040 ",
		    addr2LinePath.c_str(),
		    info.dli_fname,
		    (void*)relative_address); 
	}
      else
	{
	  snprintf (line, LINE_MAX, "%s -f -C -e %s %p",
		    addr2LinePath.c_str(),
		    info.dli_fname,
		    (void*)relative_address);
	}

      pfd = stacktracePopenFD( line, child_pid );

      length = 1;
      line[0] = ' ';

      // did we succeed to open the pipe?
      if ( pfd >= 0 )
      {
        demlen = stacktraceReadline (pfd, dembuf, sizeof(dembuf));

        length = stacktraceReadline (pfd, line+1, sizeof(line)-1);
        if (length >= 0) ++length;

        int stat = stacktracePcloseFD (pfd, child_pid);

        // don't print anything, if nothing is found
        if ( stat || line[1] == '?' || length < 0)
        {
          line[1] = '\0';
          length = 0;
        }

	if ( stat || demlen <= 0 || dembuf[0] == '?') {
          symname = info.dli_sname;
          if (!symname) symname = "???";
          demlen = strlen (symname);
        }

      }
    }
    // RS end

    bufs [nbufs].iov_base = addrbuf;
    bufs [nbufs].iov_len  = snprintf (addrbuf, addrbuf_size, " 0x%08lx ", addr);
    ++nbufs;

    bufs [nbufs].iov_base = (void *) symname; // discard const
    bufs [nbufs].iov_len  = demlen;
    ++nbufs;

    // RS start
    bufs [nbufs].iov_base = line;
    bufs [nbufs].iov_len  = length;
    ++nbufs;
    // RS end

    bufs [nbufs].iov_base = diffbuf;
    bufs [nbufs].iov_len  = snprintf (diffbuf, diffbuf_size, " %c 0x%lx [",
                                      gte ? '+' : '-', diff);
    ++nbufs;

    bufs [nbufs].iov_base = (void *) libname; // discard const
    bufs [nbufs].iov_len  = strlen (libname);
    ++nbufs;

    // RS start
    bufs [nbufs].iov_base = relbuf;
    bufs [nbufs].iov_len  = snprintf( relbuf, relbuf_size, " D[%p]", (void*)relative_address );
    ++nbufs;
    // RS end

    bufs [nbufs].iov_base = (void *) trailer; // discard const
    bufs [nbufs].iov_len  = 2;
    ++nbufs;

  }
  else
#endif
  {
    bufs [nbufs].iov_base = addrbuf;
    bufs [nbufs].iov_len  = snprintf (addrbuf, addrbuf_size, " 0x%08lx ", addr);
    ++nbufs;
    
    bufs [nbufs].iov_base = (void *) "<unknown function>\n"; //no const
    bufs [nbufs].iov_len  = 19;
    ++nbufs;
  }

  writev (fd, bufs, nbufs);
}


#if !(HAVE_BACKTRACE_SYMBOLS_FD && HAVE_DLADDR) && __GNUC__ >=4 
extern "C" {
    typedef unsigned _Unwind_Ptr __attribute__((__mode__(__pointer__)));
    struct _Unwind_Context;
    typedef enum
    {
      _URC_NO_REASON = 0,
      _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
      _URC_FATAL_PHASE2_ERROR = 2,
      _URC_FATAL_PHASE1_ERROR = 3,
      _URC_NORMAL_STOP = 4,
      _URC_END_OF_STACK = 5,
      _URC_HANDLER_FOUND = 6,
      _URC_INSTALL_CONTEXT = 7,
      _URC_CONTINUE_UNWIND = 8
    } _Unwind_Reason_Code;
    typedef _Unwind_Reason_Code (*_Unwind_Trace_Fn) (_Unwind_Context *, void *);
    extern _Unwind_Reason_Code _Unwind_Backtrace (_Unwind_Trace_Fn, void *);
    extern _Unwind_Ptr _Unwind_GetIP (_Unwind_Context *);
    extern _Unwind_Ptr _Unwind_GetRegionStart (_Unwind_Context *);
}

/** IA64-ABI specific stack walking routine.  This is mostly available
    on platforms with GCC 3.4+, but also with other compilers that have
    adopted the same ABI standard.  This walker seems to be limited to
    walking only through frames with DWARF2 exception data, so it may
    not be able to go through all C libraries.  It does seem to be
    able to signal frames on a number of platforms however.  */
_Unwind_Reason_Code
unwindWalkStack (_Unwind_Context *ctx, void *data)
{
    IOFD		fd = *(IOFD *) data;
    iovec		bufs [5];
    int			nbufs = 0;
    const int		addrbuf_size = 5 + BitTraits<unsigned long>::HexDigits;
    char		addrbuf [addrbuf_size];
    const int		diffbuf_size = 10 + 2 * BitTraits<unsigned long>::HexDigits;
    char		diffbuf [diffbuf_size];
    static const char	trailer [] = "]\n";
    unsigned long	ip = _Unwind_GetIP (ctx);
    unsigned long	ir = _Unwind_GetRegionStart (ctx);

# if HAVE_DLADDR
    Dl_info		info;
    if (dladdr ((void *) ir, &info) && info.dli_fname && info.dli_fname[0])
    {
	const char	*libname = info.dli_fname;
	const char	*symname = (info.dli_sname && info.dli_sname[0]
				    ? info.dli_sname : "?");
	unsigned long	symaddr = (unsigned long) info.dli_saddr;
	bool		gte = (ip >= symaddr);
	unsigned long	diff = (gte ? ip - symaddr : symaddr - ip);

	bufs [nbufs].iov_base = addrbuf;
	bufs [nbufs].iov_len  = snprintf (addrbuf, addrbuf_size, " 0x%08lx ", ip);
	++nbufs;

	bufs [nbufs].iov_base = (char *) symname; // discard const
	bufs [nbufs].iov_len  = strlen (symname);
	++nbufs;

	bufs [nbufs].iov_base = diffbuf;
	bufs [nbufs].iov_len  = snprintf (diffbuf, diffbuf_size, " %s 0x%lx [",
					  gte ? "+" : "-", diff);
	++nbufs;

	bufs [nbufs].iov_base = (char *) libname; // discard const
	bufs [nbufs].iov_len  = strlen (libname);
	++nbufs;

	bufs [nbufs].iov_base = (char *) trailer; // discard const
	bufs [nbufs].iov_len  = 2;
	++nbufs;
    }
    else
# endif // HAVE_DLADDR
    {
	bufs [nbufs].iov_base = addrbuf;
	bufs [nbufs].iov_len  = snprintf (addrbuf, addrbuf_size, " 0x%08lx ", ip);
	++nbufs;

	bufs [nbufs].iov_base = diffbuf;
	bufs [nbufs].iov_len  = snprintf (diffbuf, diffbuf_size, " <?%08lx> + 0x%lx\n",
					  ir, ip - ir);
        ++nbufs;
    }

    writev (fd, bufs, nbufs);
    return _URC_NO_REASON;
}
#endif // GCC 3.4+

//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>

// Change the path of the binary used for symbolization.
void DebugAids::setStackTraceAddr2Line ATLAS_NOT_THREAD_SAFE (const char* path)
{
  addr2LinePath = path;
}


#if HAVE_U_STACK_TRACE
// HP-UX stack walker (http://devresource.hp.com/STK/partner/unwind.pdf)
extern "C" void U_STACK_TRACE (void);
#endif

#if HAVE_XL_TRBK
// AIX stack walker (from xlf FORTRAN 90 runtime).
extern "C" void xl__trbk (void);
#endif

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

/** Set and return the file descriptor for stack trace output.

    If @a fd is the default invalid descriptor value, returns the
    current value without changing the setting.  This value is only
    effective for #stacktrace(), but can be overridden by the
    argument given to that function.  */
IOFD
DebugAids::stacktraceFd ATLAS_NOT_THREAD_SAFE (IOFD fd /* = IOFD_INVALID */)
{
    if (s_stackTraceFd == IOFD_INVALID)
	s_stackTraceFd = STDERR_HANDLE;

    IOFD old = s_stackTraceFd;
    if (fd != IOFD_INVALID)
	s_stackTraceFd = fd;
    return old;
}

/** Produce a stack trace.

    Prints the current stack trace to file descriptor @a fd or if the
    default invalid descriptor, the currently registered stack trace
    descriptor as registered with #stacktraceFd().  Avoids unnecessary
    memory allocation so it should be safe to call this function even
    in dire situations.  On some systems the implementation always
    outputs to the standard error and has no means for redirection.
    On these systems an attempt is made to redirect standard error
    momentarily elsewhere and then redirect standard error to the
    desired file descriptor, invoke the output, and redirect standard
    error back to its original location.  If the redirection fails or
    the system has no stack tracing support, no stack trace is
    produced.  */
void
DebugAids::stacktrace ATLAS_NOT_THREAD_SAFE (IOFD fd /* = IOFD_INVALID */)
{
    if (s_stackTraceFd == IOFD_INVALID)
	s_stackTraceFd = STDERR_HANDLE;

    if (fd == IOFD_INVALID)
	fd = s_stackTraceFd;

    std::cerr.flush ();
    fflush (stderr);

#ifdef _WIN32
    // FIXME: Autoload all these functions so users don't need to
    // link in imagehlp.dll.
    if (! SymInitialize (GetCurrentProcess (), NULL, TRUE))
    {
	MYWRITELIT (fd, ("failed to dump stack trace:"
			 " cannot get symbolic information\n"));
	return;
    }

    union SYMBUFFER {
	IMAGEHLP_SYMBOL	sym;
	BYTE		buffer [ sizeof (IMAGEHLP_SYMBOL) + 512 ];
    };

    unsigned		level = 0;
    CONTEXT		context;
    STACKFRAME		frame;
    SYMBUFFER		symbol;
    IMAGEHLP_MODULE	module;
    char		modulename [MAX_PATH];
    DWORD		section;
    DWORD		offset;
    const int		buf_size = 2*40+6;   // ample for two 128+ bit numbers
    char		buf [buf_size];
    // DWORD		exceptargs [] = { (DWORD) &context };

    // FIXME: XP 64-bit adds: RtlCaptureContext (&context);
    // This is documented to *not* work, but apparently it does.
    context.ContextFlags = CONTEXT_FULL;
    if (! GetThreadContext (GetCurrentThread (), &context))
	return;

    // LPTOP_LEVEL_EXCEPTION_FILTER oldseh
    //	= SetUnhandledExceptionFilter (&GrabExceptionContext);
    // RaiseException (0, 0, 1, exceptargs);
    // SetUnhandledExceptionFilter (oldseh);

    memset (&module, 0, sizeof (module));
    memset (&frame, 0, sizeof (frame));

    module.SizeOfStruct		= sizeof (module);

    frame.AddrPC.Offset		= context.Eip;
    frame.AddrPC.Mode		= AddrModeFlat;
    frame.AddrStack.Offset	= context.Esp;
    frame.AddrStack.Mode	= AddrModeFlat;
    frame.AddrFrame.Offset	= context.Ebp;
    frame.AddrFrame.Mode	= AddrModeFlat;

    while (true)
    {
	if (! StackWalk (IMAGE_FILE_MACHINE_I386,
			 GetCurrentProcess (),
			 GetCurrentThread (),
			 &frame,
			 &context,
			 NULL,
			 SymFunctionTableAccess,
			 SymGetModuleBase,
			 NULL)
	    || frame.AddrFrame.Offset == 0)
	    break;

	// FIXME: Throw away everything above stacktrace?  Keep looping
	// below until the name includes something we understand?

	// Print stack frame too?  If we know how many arguments there
	// are (from demangling function name -- see below, could count
	// commas), args are: *((ULONG *)frame.AddrFrame.Offset+2+ARG).
	MYWRITE (fd, buf, snprintf (buf, buf_size, "(%2u) 0x%08lx 0x%08lx   ",
				    level, frame.AddrPC.Offset,
				    frame.AddrFrame.Offset));

	memset (&symbol, 0, sizeof (symbol));
	symbol.sym.SizeOfStruct = sizeof (symbol);
	symbol.sym.MaxNameLength = sizeof (symbol) - sizeof (symbol.sym);

	offset = 0;
	if (SymGetSymFromAddr (GetCurrentProcess (), frame.AddrPC.Offset,
			       &offset, &symbol.sym))
	{
	    // FIXME: Demangle name with:
	    //   UnDecorateSymbolName (name, undecname, sizeof (undecname),
	    //				UNDNAME_COMPLETE
	    //				| UNDNAME_NO_THISTYPE
	    // 				| UNDNAME_NO_SPECIAL_SYMS
	    // 				| UNDNAME_NO_MEMBER_TYPE
	    // 				| UNDNAME_NO_MS_KEYWORDS
	    // 				| UNDNAME_NO_ACCESS_SPECIFIERS);
	    MYWRITE (fd, symbol.sym.Name, STDC::strlen (symbol.sym.Name));
	    MYWRITE (fd, buf, snprintf (buf, buf_size, " + %lx", offset));

	    if (SymGetModuleInfo (GetCurrentProcess(), frame.AddrPC.Offset,
				  &module))
	    {
		MYWRITELIT (fd, " [");
		MYWRITE (fd, module.ImageName,
			 STDC::strlen (module.ImageName));
		MYWRITELIT (fd, "]");
	    }
	}
	else
	{
	    GetLogicalAddress ((PVOID) frame.AddrPC.Offset,
			       modulename, sizeof (modulename),
			       section, offset);
	    MYWRITE (fd, buf, snprintf (buf, buf_size, "%04lx:%08lx [", section, offset));
	    MYWRITE (fd, modulename, STDC::strlen (modulename));
	    MYWRITELIT (fd, "]");
	}
	MYWRITELIT (fd, "\n");
	++level;
    }
    SymCleanup (GetCurrentProcess ());

#elif (HAVE_U_STACK_TRACE || HAVE_XL_TRBK)		// hp-ux, aix
    // FIXME: deal with inability to duplicate the file handle
    int stderrfd = dup (STDERR_FILENO);
    if (stderrfd == -1)
	return;

    int newfd    = dup2 (fd, STDERR_FILENO);
    if (newfd == -1)
    {
	close (stderrfd);
	return;
    }

# if HAVE_U_STACK_TRACE		// hp-ux
    U_STACK_TRACE ();
# elif HAVE_XL_TRBK		// aix
    xl__trbk ();
# else
#  error "oops, you shouldn't have gotten here!"
# endif

    fflush (stderr);
    dup2 (stderrfd, STDERR_FILENO);
    close (newfd);
#elif HAVE_BACKTRACE_SYMBOLS_FD && HAVE_DLADDR		// linux
    // we could have used backtrace_symbols_fd, except its output
    // format is pretty bad, so recode that here :-(
    void		*trace [MAX_BACKTRACE_DEPTH];
    int			depth = backtrace (trace, MAX_BACKTRACE_DEPTH);

    for (int n = 0; n < depth; ++n/*, nbufs = 0*/)
    {
	unsigned long	addr = (unsigned long) trace [n];
        stacktraceLine (fd, addr);
    }

#elif HAVE_EXCPT_H && HAVE_PDSC_H && HAVE_RLD_INTERFACE_H // tru64
    // Tru64 stack walk.  Uses the exception handling library and the
    // run-time linker's core functions (loader(5)).  FIXME: Tru64
    // should have _RLD_DLADDR like IRIX below.  Verify and update.

    const int	buffer_size = 100 + BitTraits<unsigned long>::HexDigits * 2 + 11;
    char	buffer [buffer_size];
    sigcontext	context;
    int		rc = 0;

    exc_capture_context (&context);
    while (!rc && context.sc_pc)
    {
	// FIXME: Elf32?
	pdsc_crd *func, *base, *crd
	  = exc_remote_lookup_function_entry(0, 0, context.sc_pc, 0, &func, &base);
	Elf32_Addr addr = PDSC_CRD_BEGIN_ADDRESS(base, func);
	// const char *name = _rld_address_to_name(addr);
	const char *name = "<unknown function>";
	snprintf (buffer, buffer_size, " 0x%012lx %.100s + 0x%lx\n",
		  context.sc_pc, name, context.sc_pc - addr);
	write (fd, buffer, STDC::strlen(buffer));
	rc = exc_virtual_unwind(0, &context);
    }

#elif HAVE_EXCEPTION_H && defined __sgi			// irix
    // IRIX stack walk -- like Tru64 but with a little different names.
    // NB: The guard above is to protect against unrelated <exception.h>
    //   provided by some compilers (e.g. KCC 4.0f).
    // NB: libexc.h has trace_back_stack and trace_back_stack_and_print
    //   but their output isn't pretty and nowhere as complete as ours.
    char       buffer [340];
    sigcontext context;

    exc_setjmp (&context);
    while (context.sc_pc >= 4)
    {
	// Do two lookups, one using exception handling tables and
	// another using _RLD_DLADDR, and use the one with a smaller
	// offset.  For signal handlers we seem to get things wrong:
	// _sigtramp's exception range is huge while based on Dl_info
	// the offset is small -- but both supposedly describe the
	// same thing.  Go figure.
	char            *name = 0;
	const char      *libname = 0;
	const char      *symname = 0;
	Elf32_Addr      offset = ~0L;

	// Do the exception/dwarf lookup
	Elf32_Addr      pc = context.sc_pc;
	Dwarf_Fde       fde = find_fde_name (&pc, &name);
	Dwarf_Addr      low_pc = context.sc_pc;
	Dwarf_Unsigned  udummy;
	Dwarf_Signed    sdummy;
	Dwarf_Ptr       pdummy;
	Dwarf_Off       odummy;
	Dwarf_Error     err;

	symname = name;

	// Determine offset using exception descriptor range information.
	if (dwarf_get_fde_range (fde, &low_pc, &udummy, &pdummy, &udummy,
				 &odummy, &sdummy, &odummy, &err) == DW_DLV_OK)
	    offset = context.sc_pc - low_pc;

       // Now do a dladdr() lookup.  If the found symbol has the same
       // address, trust the more accurate offset from dladdr();
       // ignore the looked up mangled symbol name and prefer the
       // demangled name produced by find_fde_name().  If we find a
       // smaller offset, trust the dynamic symbol as well.  Always
       // trust the library name even if we can't match it with an
       // exact symbol.
	Elf32_Addr      addr = context.sc_pc;
	Dl_info         info;

	if (_rld_new_interface (_RLD_DLADDR, addr, &info))
	{
	    if (info.dli_fname && info.dli_fname [0])
		libname = info.dli_fname;

	    Elf32_Addr symaddr = (Elf32_Addr) info.dli_saddr;
	    if (symaddr == low_pc)
		offset = addr - symaddr;
	    else if (info.dli_sname
		     && info.dli_sname [0]
		     && addr - symaddr < offset)
	    {
		offset = addr - symaddr;
		symname = info.dli_sname;
	    }
	}

	// Print out the result
	if (libname && symname)
            write (fd, buffer, snprintf
		   (buffer, buffer_size, " 0x%012lx %.100s + 0x%lx [%.200s]\n",
		    addr, symname, offset, libname));
	else if (symname)
	    write (fd, buffer, snprintf
		   (buffer, buffer_size, " 0x%012lx %.100s + 0x%lx\n",
		    addr, symname, offset));
	else
	    write (fd, buffer, snprintf
		   (buffer, buffer_size, " 0x%012lx <unknown function>\n", addr));

	// Free name from find_fde_name().
	free (name);

	// Check for termination.  exc_unwind() sets context.sc_pc to
	// 0 or an error (< 4).  However it seems we can't unwind
	// through signal stack frames though this is not mentioned in
	// the docs; it seems that for those we need to check for
	// changed pc after find_fde_name().  That seems to indicate
	// end of the post-signal stack frame.  (FIXME: Figure out how
	// to unwind through signal stack frame, e.g. perhaps using
	// sigcontext_t's old pc?  Or perhaps we can keep on going
	// down without doing the symbol lookup?)
	if (pc != context.sc_pc)
	    break;

	exc_unwind (&context, fde);
    }

#elif defined PROG_PSTACK				// solaris
# ifdef PROG_CXXFILT
#  define CXXFILTER " | " PROG_CXXFILT
# else
#  define CXXFILTER
# endif
    // 64 should more than plenty for a space and a pid.
    const int buffer_size = sizeof(PROG_PSTACK) + 1 + BitTraits<unsigned long>::Digits
                 + 3 + sizeof(PROG_CXXFILT) + BitTraits<int>::Digits + 1;
    char buffer [buffer_size];
    snprintf (buffer, buffer_size, "%s %lu%s 1>&%d", PROG_PSTACK, (unsigned long) getpid (),
	     "" CXXFILTER, fd);
    system (buffer);
# undef CXXFILTER

#elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
    // FIXME: Check for _Unwind*, compilers other than GCC support this API
    _Unwind_Backtrace (unwindWalkStack, &fd);
#endif

    // FIXME: mpatrol has some generic unix unwind code.
    // FIXME: from unix faq: ask debugger to dump stack trace
    // with something like:
    //   - gdb: echo "thread apply all where\nwhere\ndetach" | gdb $prog $pid
    //   - dbx: echo "where\ndetach" | dbx -a $program_path $pid
    //   - dbx (aix): echo "where\ndetach" | dbx -p $program_path $pid
}

/** Drop a core dump and continue.

    Creates a core file for the current program state and continues
    execution.  @a sig should be the number of the signal from which
    the program should appear to have died; this should a fatal signal
    that does cause a core file to be created (or @c SIGUSR1).

    This works by forking the process and then killing the child with
    the given signal; the signal is automatically unblocked in the
    child to make sure the sure the signal is delivered.  Thus the
    function returns only once, in the parent process.

    This function can be safely installed directly as a signal
    handler.  #Signal::handleFatal() will do so for @c SIGUSR1 with
    suitable options.

    Note that this function does not change core dump resource limits,
    not even for the forked child process.  If core files are disabled
    through resource limits, no core file will be created despite your
    explicit request to create one.

    This concept was taken from DDD, the Data Display Debugger. */
void
DebugAids::coredump (int sig, ...)
{
#ifndef _WIN32
    // FIXME: Forking vs. threads -- need to sort out what is safe.
    // FIXME: Provide a resource limits interface so that core
    // resource limits can be raised?

    pid_t	corepid;
    int		status;

    ::unlink ("core");
    if ((corepid = ::fork ()) == 0)
    {
	// In child: re-raise the signal, thus killing the process and
	// producing a core dump.  Make sure 1) the signal is not
	// blocked so that we won't return to the caller, 2) we have a
	// signal that is fatal, 3) the signal falls to its default
	// handler to produce the dump.

#ifdef SIGUSR1
	// SIGUSR1 does not cause a core dump; use abort() instead
	if (sig == SIGUSR1)
	    sig = SIGABRT;	// Could be SIGIOT if SIGABRT is not defined
#endif
	Signal::handle (sig, (Signal::HandlerType) SIG_DFL);
	Signal::block (sig, false);
	Signal::raise (sig);

	// Yikes, this shouldn't happen.  ASSERT isn't right here.  If
	// raise() failed to deliver the signal, abort() is unlikely
	// to work any better, but try it anyway.  Then make sure we
	// die so that we won't return to the caller from the child.
	abort ();
	_exit (255);
    }
    else if (corepid > 0) {
      pid_t wait_pid;
      do {
	wait_pid = ::waitpid (corepid, &status, 0);
      } while (wait_pid == -1 && errno == EINTR);
    }
#endif // !_WIN32
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//} // namespace seal                             wlav
} // namespace Athena                             wlav
