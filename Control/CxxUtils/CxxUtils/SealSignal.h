/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/SealSignal.h
 * @author Lassi Tuura (original author)
 * @author Wim Lavrijsen <WLavrijsen@lbl.gov> (responsible in ATLAS)
 * @date Oct, 2008
 * @brief This is the signal handler from SEAL, adapted to build in Atlas,
 *        after the drop of that project.
 *
 *        Search for `wlav' or `sss' to find changes from the SEAL version. I
 *        also dropped all ASSERT macro's in favor of assert.
 */

#ifndef CXXUTILS_SEAL_SIGNAL_H // wlav SEAL_BASE_SIGNAL_H
#define CXXUTILS_SEAL_SIGNAL_H // wlav SEAL_BASE_SIGNAL_H

#include "CxxUtils/checker_macros.h"
//# include "SealBase/sysapi/IOTypes.h"           wlav

// These should be hidden, but we can't do that for now: the clients
// must be able to operate on sigset_t and pid_t.  Note that we do not
// want to have <csignal> -- we need all the extra POSIX stuff.
# include <signal.h>
# include <sys/types.h>
# include <climits>


// Hacks for fields we might not have in siginfo_t.  Just print zero.
#ifdef si_utime // sss
# define HAVE_SIGINFO_SI_UTIME 1  // sss
#endif // sss 
#ifdef si_int // sss
# define HAVE_SIGINFO_SI_INT 1 // sss
#endif // sss
# if !HAVE_SIGINFO_SI_UTIME		// darwin
#  define si_utime si_signo ? 0 : 0
#  define si_stime si_signo ? 0 : 0
# endif
# if !HAVE_SIGINFO_SI_INT
#  if HAVE_SIGINFO_SI_VALUE		// darwin
#   define si_int   si_value.sigval_int
#   define si_ptr   si_value.sigval_ptr
#  else					// (none known)
#   define si_int   si_signo ? 0 : 0
#   define si_ptr   si_signo ? (void *) 0 : (void *) 0
#  endif
# endif


# if !HAVE_POSIX_SIGNALS
// Forward declare POSIX signal handling stuff for platforms that
// don't have them.  This allows them to be mentioned in the Signal
// interface and minimally used in the clients.  Special kludge for
// Windows.
#  ifdef _WIN32
typedef struct _EXCEPTION_RECORD siginfo_t;
#define SIGHUP          1       /* Hangup (POSIX).  */
#define SIGQUIT         3       /* Quit (POSIX).  */
#define SIGTRAP         5       /* Trace trap (POSIX).  */
#define SIGKILL         9       /* Kill, unblockable (POSIX).  */
#define SIGUSR1         10      /* User-defined signal 1 (POSIX).  */
#define SIGUSR2         12      /* User-defined signal 2 (POSIX).  */
#define SIGPIPE         13      /* Broken pipe (POSIX).  */
#define SIGALRM         14      /* Alarm clock (POSIX).  */
#define SIGCHLD         17      /* Child status has changed (POSIX).  */
#define SIGCONT         18      /* Continue (POSIX).  */
#define SIGSTOP         19      /* Stop, unblockable (POSIX).  */
#define SIGTSTP         20      /* Keyboard stop (POSIX).  */
#define SIGTTIN         21      /* Background read from tty (POSIX).  */
#define SIGTTOU         22      /* Background write to tty (POSIX).  */
#  else
struct siginfo_t {};
#  endif

typedef int sigset_t;

#  define sigemptyset(x) 	 (0)
#  define sigfillset(x)  	 (0)
#  define sigaddset(x,y) 	 (0)
#  define sigdelset(x,y) 	 (0)
#  define sigismember(x,y) (0)
# endif

//namespace seal {                                wlav
namespace Athena {                             // wlav

// wlav copied from SealBase/BitTraits.h
/** Describe the bit features of an integral type @c T. */
template <class T>
struct BitTraits
{
    /// Number of bits in @c T.
    enum { Bits		= sizeof (T) * CHAR_BIT };

    /// Number of 8-bit bytes in @c T.
    enum { Bytes	= Bits / 8 + ((Bits % 8) > 0) };

    /// Number of base-10 digits in @c T (without leading sign).
    enum { Digits	= (Bits * 30103) / 100000 + 1 };
    // 30103 =~ M_LN2 / M_LN10 * 100000

    /// Number of base-16 digits in @c T (without leading sign).
    enum { HexDigits	= Bits / 4 + ((Bits % 4) > 0) };
};


/** Utilities for handling signals and fatal errors.

    FIXME: POSIX single-threaded vs. multi-threaded signals?
         - all threads should block all the signals
         - one thread should do sigwait.

    The fatal error handling is largely inspired by code in DDD, the
    Data Display Debugger, and by the examples in GNU libc manual. */
class Signal
{
public:
    /** Option that instructs #fatal() to call #coredump() on SIGUSR1.
	This is merely a request to drop a @c core; no attempt is made
	to guarantee success.  Failure may result for example for lack
	of permissions, for lack of disk space, or due to low resource
	limits.  Please note that @c core files can only be created on
	unixen.  Note also that dropping a core is a security risk and
	should never be enabled in setuid or setgid programs or for
	production applications.  */
    static const int USR1_DUMP_CORE	= 1;

    /** Option to make SIGHUP, SIGTERM and SIGQUIT fatal instead of
	just #quit() signals.  */
    static const int FATAL_ON_QUIT	= 2;

    /** Option to make SIGINT fatal.  It will still just quit, not
        crash.  */
    static const int FATAL_ON_INT	= 4;

    /** Option to make #fatal() dump a core file before crashing.  */
    static const int FATAL_DUMP_CORE	= 8;

    /** Option to make #fataldump() (invoked by #fatal()) to dump the
	signal name (as reported by #name()).  */
    static const int FATAL_DUMP_SIG	= 16;

    /** Option to make #fataldump() (invoked by #fatal()) to dump
	stack backtrace for the offending code location.  */
    static const int FATAL_DUMP_STACK	= 32;

    /** Option to make #fataldump() (invoked by #fatal()) to dump the
	list of currently loaded shared libraries.  */
    static const int FATAL_DUMP_LIBS	= 64;

    /** Option to make #fataldump() (invoked by #fatal()) to dump the
	machine context (registers etc.) from the fault position.  */
    static const int FATAL_DUMP_CONTEXT	= 128;

    /** Option to make #fatal() exit via #quit().  This will cause all
	the application clean-up hook to run.  */
    static const int FATAL_AUTO_EXIT	= 256;

    /** Default options to #handleFatal().  */
    static const int FATAL_DEFAULT  = (USR1_DUMP_CORE
				       | FATAL_ON_INT
				       | FATAL_DUMP_CORE
				       | FATAL_DUMP_SIG
				       | FATAL_DUMP_STACK
				       | FATAL_DUMP_LIBS
				       | FATAL_DUMP_CONTEXT
				       | FATAL_AUTO_EXIT);

    /** Application clean-up hook invoked before #quit() exits from
	program termination signals (SIGHUP, SIGTERM or SIGQUIT).

	The handler should return @c true if the signal handler should
	proceed to exit the application.  Note that certain options to
	#handlFatal() cause this hook to be invoked for fatal signals.
	If such behaviour is enabled, be sure to check the #crashed()
	status before deciding to let the application to continue.

	The quit hook should take care of resetting terminal modes,
	killing child processes, removing lock files, and so forth.  */
    typedef bool		(*QuitHook) (int sig, siginfo_t *info, void *x);

    /** Application hook to run in fatal().  The hook should return @c
	true if the signal handler should proceed to die.  @a sig is
	the signal number, or its negative if core was dumped and, as
	far as can determined, successfully produced.

	The fatal hooks should, if possible, perform clean-ups similar
	to #QuitHook.  The application may achieve this by actually
	using the quit by setting #FATAL_AUTO_EXIT for #handleFatal(),
	or it could reuse an internal function in both handlers.  */
    typedef bool		(*FatalHook) (int sig, siginfo_t *info, void *x);

    /** Application hook to jump back to the main program from a fatal
	signal, for example using #siglongjmp.  It must never return.
	@a sig is the signal number, or its negative if core was
	dumped and, as far as can determined, successfully produced.  */
    typedef void		(*FatalReturn) (int sig, siginfo_t *info, void *x);

    /** Signal handler type.  This is defined explicitly and does not
	necessarily match the system's concept of signal handler type.
	If necessary, suitable trampolines are used internally to make
	sure the arguments make sense.

	@param sig	The signal number.
	@param info	Pointer to signal info.  This pointer will
			be null on platforms that do not support
			POSIX signals.
	@param extra	Extra argument, e.g. the fault address.
			This pointer will be null on platforms
			that do not support POSIX signals.  */
    typedef void (*HandlerType) (int sig, siginfo_t *info, void *extra);


    // Generic signal operations
    // - Signal names
    static const char *		name (int sig);

    // - Signal handlers and masks
    static HandlerType		handler (int sig, sigset_t *mask = 0);
    static HandlerType		handle (int sig, HandlerType handler,
					const sigset_t *blockMask = 0);
    static void			revert (int sig);
    static void			ignore (int sig);

    static void			block (int sig, bool sense);
    static void			block (const sigset_t *mask, bool sense);
    static void			mask (const sigset_t *mask, sigset_t *old = 0);

    // - Sending and receiving signals
    static int			raise (int sig);
    static int			kill (pid_t process, int sig);
    static int			queue (int sig, int value = 0);
    static int			queue (int sig, void *value);
    static int			queue (pid_t process, int sig, int value = 0);
    static int			queue (pid_t process, int sig, void *value);

    static bool			pending (int sig);
    static void			pending (sigset_t *mask);
    static void			suspend (const sigset_t *mask);
    static bool			wait (int		sig,
				      siginfo_t		*info = 0,
				      long		msecs = -1);
    static int			wait (const sigset_t	*mask,
				      siginfo_t		*info = 0,
				      long		msecs = -1);

    // Assisted handling of program termination signals
    static void			handleQuit ATLAS_NOT_THREAD_SAFE (QuitHook hook = 0);
    static QuitHook		handleQuitHook ATLAS_NOT_THREAD_SAFE (void);

    static void			quit ATLAS_NOT_THREAD_SAFE (int sig, siginfo_t *info, void *x);

    // Assisted handling of fatal signals
    static void			handleFatal ATLAS_NOT_THREAD_SAFE (const char *applicationName = 0,
                                                                   IOFD fd = IOFD_INVALID,
                                                                   FatalHook hook = 0,
                                                                   FatalReturn mainreturn = 0,
                                                                   unsigned options = FATAL_DEFAULT);
    static IOFD			handleFatalFd ATLAS_NOT_THREAD_SAFE (void);
    static FatalHook		handleFatalHook ATLAS_NOT_THREAD_SAFE (void);
    static FatalReturn		handleFatalReturn ATLAS_NOT_THREAD_SAFE (void);
    static unsigned		handleFatalOptions ATLAS_NOT_THREAD_SAFE (void);

    static void			fatal ATLAS_NOT_THREAD_SAFE (int sig, siginfo_t *info, void *x);
    static bool			fatalDump ATLAS_NOT_THREAD_SAFE (int sig, siginfo_t *info, void *x);
    static int			fatalLevel ATLAS_NOT_THREAD_SAFE (void);
    static bool			crashed ATLAS_NOT_THREAD_SAFE (void);

    static void			dumpInfo    (IOFD fd, char *buf, int sig,
					     const siginfo_t *info);
    static void			dumpMemory  (IOFD fd, char *buf,
					     const void *data, size_t n);
    static unsigned long        dumpContext (IOFD fd, char *buf,
					     const void *context);

    static const char *		describe (int sig, int code);
  
private:
    static void			trampoline (int sig);

    static bool			s_crashed;
    static int			s_inFatal;
    static unsigned long        s_lastSP;
    static const char		*s_applicationName;
    static IOFD			s_fatalFd;
    static FatalHook		s_fatalHook;
    static FatalReturn		s_fatalReturn;
    static unsigned		s_fatalOptions;
    static QuitHook		s_quitHook;
#if !HAVE_POSIX_SIGNALS
    static HandlerType		s_trampolines [NSIG];
#endif
};


//} // namespace seal                             wlav
} // namespace Athena                             wlav


extern "C" {
  /// Install fatal handler with default options.
  /// This is meant to be easy to call from pyton via ctypes.
  void CxxUtils_installFatalHandler ATLAS_NOT_THREAD_SAFE ();
}


#endif // CXXUTILS_SEAL_SIGNAL_H wlav SEAL_BASE_SIGNAL_H
