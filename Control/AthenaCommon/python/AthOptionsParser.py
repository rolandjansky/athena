# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# @file AthenaCommon.AthOptionsParser
# @purpose the central module to parse command line options of athena.py
# @date December 2009

from __future__ import print_function

__version__ = "$Revision: 721711 $"
__doc__ = "a module to parse command line options for athena.py"
__author__ = "Sebastien Binet"

# TODO: needs major cleanup to a) manage options, b) be lazy, and c) allow
# cleanup like options used to be before this module was created.

import getopt
import sys
import os

from .Debugging import DbgStage

### globals -------------------------------------------------------------------
_useropts = "bidc:hl:svp:r:t:"
_userlongopts = [
    "batch", "interactive", "no-display", "debug=", "command=", "help",
    "loglevel=", "showincludes", "trace=", "check-properties",
    "version",
    "leak-check=", "leak-check-execute", "delete-check=",
    "perfmon", "pmon=", "repeat-evts=", "profile-python=",
    "enable-ers-hdlr=",
    "keep-configuration","drop-configuration", "drop-and-reload", "config-only=",
    "dump-configuration=",
    "tcmalloc", "stdcmalloc", "stdcmath", "imf", "preloadlib=",
    "nprocs=",
    "debugWorker",
    "pycintex_minvmem=", "cppyy_minvmem",
    "minimal",                     # private, undocumented
    "threads=", "concurrent-events=", "mtes", "mtes-channel=",
    "evtMax=",    #will set theApp.EvtMax just before theApp.run() in runbatch.py
    "skipEvents=",#will set svcMgr.EventSelector.SkipEvents just before theApp.run() in runbatch.py 
    "filesInput=" #will set the AthenaCommonFlags.FilesInput job option and lock it
    ]

_allowed_values = {
    'leak-check': [ 'initialize', 'start', 'beginrun',
                    'execute', 'finalize', 'endrun', 'stop',
                    'full', 'full-athena' ],

    'pmon': ('perfmon', 'fastmon', 'fullmon', 'sdmon', 'sdmonfp', 'perfmonmt', 'fastmonmt', 'fullmonmt',
             'malloc-mon', 'heph-mon', 'dso-mon', 'pers-mon', 'dp-mon',
             'post-proc',),
    }
"""a list of allowed values for options which require them"""

_error_msg = """\
Accepted command line options:
     --evtMax=<numEvents>             ...  Max number of events to process (only used in batch mode)
     --skipEvents=<numEvents>         ...  Number of events to skip (only used in batch mode)
     --filesInput=<files>             ...  Set the FilesInput job property 
                                            (comma-separated list, which can include wild characters)
 -b, --batch                          ...  batch mode [DEFAULT]
 -i, --interactive                    ...  interactive mode
     --no-display                           prompt, but no graphics display
 -d, --debug <stage>                  ...  attach debugger (gdb) before run,
                                            w/ <stage>: conf, init, exec, fini
                                           -d is a shorthand for --debug init and takes no argument
     --leak-check=<stage>             ...  perform basic memory leak checking.
                                           This disables the use of tcmalloc.
     --delete-check=<stage>           ...  perform double delete checking.
                                           This disables the use of tcmalloc.
     --profile-python=<file>.pkl|txt  ...  profile python code, dump in <file>.pkl|txt
 -c, --command                        ...  one-liner, runs before any scripts
 -h, --help                           ...  print this help message
 -l, --loglevel <level>               ...  logging level (ALL, VERBOSE, DEBUG,
                                              INFO, WARNING, ERROR, or FATAL)
 -s, --showincludes,                  ...  show printout of included files
     --trace <pattern>                     also show files that match <pattern>
 -t, --check-properties <level>       ...  check properties based on setting history,
                                              report details depend <level>
 -v, --version                        ...  print version number
     --perfmon                        ...  enable performance monitoring toolkit
                                           (same as --pmon=perfmon)
     --pmon=<level-or-name>           ...  enable performance monitoring toolkit
 -r, --repeat-evts=n                  ...  number of times to repeat each event
                                           from a given input file
     --enable-ers-hdlr=y/n            ...  enable or not the ERS handler. [DEFAULT]
                                           is to disable it so we get nice
                                           tracebacks.
     --keep-configuration             ...  do not offload Configurables after use [DEFAULT]
     --drop-configuration             ...  offload Configurables after use
     --drop-and-reload                ...  offload configuration and start new process
     --config-only=<file>             ...  run configuration and store in <file>
     --dump-configuration=<file>      ...  dump an ASCII version of the post-setup() configuration to <file>
     --tcmalloc                       ...  use tcmalloc.so for memory allocation [DEFAULT]. This disables leak-check
     --stdcmalloc                     ...  use libc malloc for memory allocation
     --stdcmath                       ...  use glibc math functions [DEFAULT]
     --imf                            ...  use Intel Math Function library: faster, but may give different numerical results from --stdcmath
     --preloadlib=<lib>               ...  localized preload of library <lib>
     --nprocs=n                       ...  enable AthenaMP if n>=1 or n==-1
     --threads=n                      ...  number of threads for AthenaMT
                                           With AthenaMP, number of threads per worker
     --concurrent-events              ...  number of concurrent events for AthenaMT
     --mtes                           ...  activate multithreaded event service
     --mtes-channel                   ...  the name of the yampl channel between pilot and AthenaMT
                                           when running in the event service mode
     --debugWorker                    ...  pause AthenaMP workers at bootstrap until SIGUSR1 signal received
 [<file1>.py [<file2>.py [...]]]      ...  scripts to run
 """

### classes -------------------------------------------------------------------
class Options(object):
    """place holder for command line options values"""
    pass

class AthOptionsError(SystemExit):
    def __init__(self, reason=None):
        import AthenaCommon.ExitCodes as ath_codes
        if reason is None:
            reason = ath_codes.OPTIONS_UNKNOWN
        try:
            message = ath_codes.codes[reason]
        except KeyError:
            message = ath_codes.codes[ath_codes.OPTIONS_UNKNOWN]

        print (_error_msg)
        SystemExit.__init__(self, reason, message)
    pass

### functions -----------------------------------------------------------------
def _help_and_exit(reason=None):
    raise AthOptionsError(reason)

def parse(chk_tcmalloc=True):
    """parses command line arguments and returns an ``Options`` instance"""

    scripts, fromdb, _opts = [], None, []
    opts = Options()
    opts.default_jobopt = "jobOptions.py" # name of the default joboption (if not provided)
    opts.msg_lvl = "INFO"        # default level of (gaudi) logging
    opts.dbg_stage = None        # no debugging by default
    opts.run_batch = 1           # batch mode is the default
    opts.display = None          # only useful in interactive mode: no display
    opts.command = ""            # optional one-line command
    opts.showincludes = 0        # don't show include files
    opts.trace_pattern = ""      # defaults decided in Include module
    opts.check_properties = 0    # no checking by default
    opts.do_pmon = False         # default is to NOT enable perfmon
    opts.nbr_repeat_evts = 0     # default is to NOT repeat events
    opts.enable_ers_hdlr = False # enable/disable TDAQ ERS signal handlers
    opts.drop_cfg = False        # default is to keep the configurables at initialize
    opts.drop_reload = False     # default is to run a single process
    opts.config_only = ''        # default is to run full process
    opts.config_dump_file = None # not used by default
    opts.do_leak_chk = False     # default is not to do any leak checking
    opts.memchk_mode = ''        # no mode selected by default 
    opts.profile_python = None   # set to file name to collect and dump python profile
    opts.nprocs = 0              # enable AthenaMP if >= 1 or == -1
    opts.threads = 0             # enable AthenaMT if >= 1
    opts.concurrent_events = 0   # enable AthenaMT if >= 1
    opts.mtes = False            # activate multithreaded event service
    opts.mtes_channel="EventService_EventRanges"    # yampl channel between pilot and athenaMT
    opts.debug_worker = False    # pause AthenaMP worker after bootstrap until SIGUSR1 received
    opts.cppyy_minvmem = None    # artificial vmem bump around cppyy's import
    opts.minimal = False         # private, undocumented
    opts.user_opts = []          # left-over opts after '-'
    opts.evtMax = None           # evtMax on command line (or None if unspecified)
    opts.skipEvents = None # skipEvents on command line (or None if unspecified)

    ldpreload = os.getenv('LD_PRELOAD') or ''

    ### print out warning if tcmalloc requested, but not loaded, 
    warn_tcmalloc = False
    libname='libtcmalloc.so'
    using_minimaltcmalloc=False
    if os.getenv('USETCMALLOCMINIMAL') == '1':
        libname='libtcmalloc_minimal.so'
        using_minimaltcmalloc=True

    if ldpreload.find(libname) == -1:
        using_tcmalloc = False
        if (os.getenv('USETCMALLOC') == '1' or
            os.getenv('USETCMALLOC') is None):
            warn_tcmalloc = True
        for arg in sys.argv[1:]:
            if arg == '--tcmalloc':
                warn_tcmalloc = True
            elif arg in ("--stdcmalloc",
                         "--leak-check",
                         "--leak-check-execute"):
                warn_tcmalloc = False
    else:
        using_tcmalloc = True

    opts.use_tcmalloc = using_tcmalloc
    opts.use_minimaltcmalloc = using_minimaltcmalloc
    
    # emulated GNU getopt for p2.2:
    # collect scripts and options (special case for '-p')
    args = sys.argv[1:]
    for arg in args:
        if (arg[-3:] == '.py' and 
            (arg[:7] != '--trace' )):
            scripts.append(arg)
        elif arg[-4:] == '.pkl' and '=' not in arg:
            fromdb = arg
            opts.default_jobopt = ''
        elif arg == '-':     # rest are user opts, save and done
            opts.user_opts += args[ args.index(arg)+1: ]
            break
        else:
            _opts.append(arg)

    # process user options
    try:
        optlist, args = getopt.getopt(_opts, _useropts, _userlongopts)
    except getopt.error:
        print (sys.exc_info()[1])
        _help_and_exit()

    if warn_tcmalloc and chk_tcmalloc:
        print ('*******************************************************************************')
        print ('WARNING: option --tcmalloc used or implied, but libtcmalloc.so not loaded.')
        print ('         This is probably because you\'re using athena.py in a non standard way')
        print ('         such as "python athena.py ..." or "nohup athena.py"')
        print ('         If you wish to use tcmalloc, you will have to manually LD_PRELOAD it')
        print ('*******************************************************************************')
        print ('')

    if args:
        print ("Unhandled arguments:", args)
        _help_and_exit()
        pass

    for opt, arg in optlist:

        if opt in ("-b", "--batch"):
            opts.run_batch = 1

        elif opt in ("-i", "--interactive"):
            opts.run_batch = 0
            opts.default_jobopt = ""
            if opts.display is None:
                opts.display = 1

        elif opt in ("--no-display",):
            opts.display = 0

        elif opt in ("-d", "--debug"):
            if not arg:
                arg = "init"
            elif arg not in DbgStage.allowed_values:
                _help_and_exit()
            opts.dbg_stage = arg

        elif opt in ("-c", "--command"):
            opts.command = arg.strip()

        elif opt in ("-h", "--help"):
            if len(scripts) > 0:
                # If we have user scripts, we should also check if they have
                # helptext to print
                opts.msg_lvl = "WARNING"
                opts.user_opts += ["-h"]
            else:
                print (_error_msg)
                sys.exit()

        elif opt in ("-l", "--loglevel"):
            opts.msg_lvl = arg.upper()
            
        elif opt in ("-s", "--showincludes"):
            opts.showincludes = 1

        elif opt == "--trace":
            opts.trace_pattern = arg

        elif opt in ("-t", "--check-properties" ):
            try:
                iarg = int(arg)
                if iarg == 1 or iarg < 0:
                    raise ValueError
                opts.check_properties = iarg
            except ValueError:
                print ('properties check expects a numerical argument of 2 or greater')
                _help_and_exit()
            
        elif opt in ("-v", "--version"):
            print (__version__)
            sys.exit()
            
        elif opt in ("--leak-check", "--leak-check-execute", "--delete-check"):
            if not using_tcmalloc:
                # early import is needed for proper offloading later
                import Hephaestus.MemoryTracker as memtrack    # noqa: F401
                if opt == "--delete-check":
                    import Hephaestus.DeleteChecker            # noqa: F401
                    opts.memchk_mode = 'delete-check'
                else:
                    opts.memchk_mode = 'leak-check'
                allowed = _allowed_values['leak-check']
                if not arg:
                    # meaning --leak-check-execute which does not
                    # require an argument
                    opts.do_leak_chk = [ 'execute' ]
                elif arg == 'all':
                    opts.do_leak_chk = []
                elif arg in allowed:
                    opts.do_leak_chk = [ arg ]
                else:
                    print ("invalid argument to hephaestus leak check [%s]" % arg)
                    print ("allowed values are: %r" % allowed)
                    _help_and_exit()
            else:
                print ("disabling Hephaestus leak check as it is incompatible with tcmalloc")

        elif opt in ("--profile-python",):
            pos = arg.rfind('.')
            if pos < 0 or (arg[pos+1:] != "txt" and arg[pos+1:] != "pkl"):
               print ("no recognizable profile output requested (%s): assuming txt" % arg)
               opts.profile_python = arg + ".txt"
            else:
               opts.profile_python = arg

        elif opt in ("--perfmon", "--pmon"):
            allowed = _allowed_values['pmon']
            if not arg:
                # meaning old switch (ie: normal-pmon)
                opts.do_pmon = ['perfmon']
            else:
                pmon_args = arg.split(',')[:]
                for a in pmon_args:
                    if a.startswith(('+','-')):
                        a = a[1:]
                    if a not in allowed:
                        print ("invalid argument to perfmon [%s]" % (a,))
                        print ("allowed values are: %r" % (allowed,))
                        _help_and_exit()
                        
                opts.do_pmon = arg.split(',')[:]
                

        elif opt in ("--repeat-evts",):
            if not arg:
                arg = 0
            try: arg = int(arg)
            except Exception as err:
                print ("ERROR:",err)
                _help_and_exit()
            opts.nbr_repeat_evts = arg
            
        elif opt in ("--enable-ers-hdlr",):
            opts.enable_ers_hdlr = arg=='y'
            pass

        elif opt in ("--drop-configuration",):
            opts.drop_cfg = True
        elif opt in ("--keep-configuration",):
            opts.drop_cfg = False

        elif opt in ("--drop-and-reload",):
            opts.drop_reload = True
        elif opt in ("--config-only",):
            opts.config_only = arg
            if opts.config_only[-4:] != '.pkl':
               opts.config_only += '.pkl'

            opts.default_jobopt = ''

        elif opt in ("--dump-configuration",):
            opts.config_dump_file = arg

        elif opt in ("--pycintex_minvmem", "--cppyy_minvmem"):
            try: opts.cppyy_minvmem = float(arg)
            except Exception as err:
                print ("ERROR:",err)
                _help_and_exit()

        elif opt in ("--minimal",):
            opts.minimal = True

        elif opt in ("--nprocs",):
            if not arg:
                arg = 0
            try: arg = int(arg)
            except Exception as err:
                print ("ERROR:",err)
                _help_and_exit()
            opts.nprocs = arg

        elif opt in ("--threads",):
            if not arg:
                arg = 0
            try: arg = int(arg)
            except Exception as err:
                print ("ERROR:",err)
                _help_and_exit()
            opts.threads = arg

        elif opt in ("--concurrent-events",):
            if not arg:
                arg = 0
            try: arg = int(arg)
            except Exception as err:
                print ("ERROR:",err)
                _help_and_exit()
            opts.concurrent_events = arg

        elif opt in ("--mtes-channel",):
            if not arg:
                arg = "EventService_EventRanges"
            opts.mtes_channel = arg

        elif opt in ("--mtes",):
            opts.mtes = True

        elif opt in ("--debugWorker",):
            opts.debug_worker = True

        elif opt in("--filesInput",):
            #set the jps.AthenaCommonFlags.FilesInput property
            from AthenaCommon.AthenaCommonFlags import jobproperties as jps
            from glob import glob
            #split string by , character
            #and for each use glob to expand path
            files = []
            for fe in arg.split(","):
                #glob, if it fails to find any file, then add string explicitly
                _files = glob(fe)
                if len(_files)==0: files += [fe]
                else: files += _files
            jps.AthenaCommonFlags.FilesInput.set_Value_and_Lock(files)

        elif opt in("--evtMax",):
            from AthenaCommon.AthenaCommonFlags import jobproperties as jps
            try: arg = int(arg)
            except Exception as err:
                print ("ERROR: --evtMax option - ",err)
                _help_and_exit()
            opts.evtMax = arg
            jps.AthenaCommonFlags.EvtMax.set_Value_and_Lock(arg)
        elif opt in("--skipEvents",):
            from AthenaCommon.AthenaCommonFlags import jobproperties as jps
            try: arg = int(arg)
            except Exception as err:
                print ("ERROR: --skipEvents option - ",err)
                _help_and_exit()
            opts.skipEvents = arg
            jps.AthenaCommonFlags.SkipEvents.set_Value_and_Lock(arg)

    # Unconditionally set this environment (see JIRA ATEAM-241)
    # This behavior can be controlled by a flag, if needed
    os.environ['LIBC_FATAL_STDERR_']='1'

    # user decision about TDAQ ERS signal handlers
    if opts.enable_ers_hdlr:
        if 'TDAQ_ERS_NO_SIGNAL_HANDLERS' in os.environ:
            del os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS']
    else:
        os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS']='1'

    opts.scripts = scripts
    opts.fromdb  = fromdb

    return opts
