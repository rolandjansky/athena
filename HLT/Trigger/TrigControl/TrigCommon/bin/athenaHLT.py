#!/bin/sh
# -*- mode: python -*-
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# This is a script that is born as shell to setup the preloading and then
# resurrected as python script for the actual athenaHLT.py application.
#
"""date"

# defaults
export USETCMALLOC=1
export USEIMF=1

# parse command line arguments
for a in ${@}
do
    case "$a" in
	--leak-check*)   USETCMALLOC=0;;
	--stdcmalloc)    USETCMALLOC=0;;
	--tcmalloc)      USETCMALLOC=1;;
	--stdcmath)      USEIMF=0;;
	--imf)           USEIMF=1;;
	--preloadlib*)   export ATHENA_ADD_PRELOAD=${a#*=};;
        --no-ers-signal-handlers)  export TDAQ_ERS_NO_SIGNAL_HANDLERS=1;;
    esac
done

# Do the actual preloading via LD_PRELOAD
source `which athena_preload.sh `

# Now resurrect ourselves as python script
python_path=`which python`
"exec" "$python_path" "-tt" "$0" "$@";

"""

import sys
import os
import argparse
import ast
import collections
from datetime import datetime as dt

from TrigCommon import AthHLT
from AthenaCommon.Logging import logging
log = logging.getLogger('athenaHLT')

##
## The following arg_* methods are used as custom types in argparse
##
def arg_sor_time(s):
   """Convert possible SOR time arguments to an OWLTime compatible string"""
   fmt = '%d/%m/%y %H:%M:%S.%f'
   if s=='now':        return dt.now().strftime(fmt)
   elif s.isdigit():   return dt.fromtimestamp(float(s)/1e9).strftime(fmt)
   else:               return s

def arg_detector_mask(s):
   """Convert detector mask to format expected by eformat"""
   dmask = hex(int(s,16))                                    # Normalize input to hex-string
   dmask = dmask.lower().replace('0x', '').replace('l', '')  # remove markers
   return '0' * (32 - len(dmask)) + dmask                    # (pad with 0s)

def arg_ros2rob(s):
   """Handle an explicit dictionary or read it from a file"""
   try:
      with open(s) as f:
         s = f.read()  # If file exists, replace ros2rob with content of the file
         print("Reading ros2rob from a file")
   except IOError:
      pass

   try:
      ros2robdict = arg_eval(s)
      if type(ros2robdict) is not dict:
         raise(ValueError)
      return ros2robdict
   except Exception:
      sys.stderr.write("ERROR! ros2rob cannot be evaluated as it is not a proper python dictionary: {}\n".format(s))
      sys.stderr.write("Using empty ros2rob dictionary instead\n")
      return {}

def arg_log_level(s):
   """Argument handler for log levels"""
   lvls = s.split(',')
   if len(lvls)==1: lvls.append('ERROR')
   return lvls

def arg_eval(s):
   """Argument handler for python types (list, dict, ...)"""
   return ast.literal_eval(s)

def update_pcommands(args, cdict):
   """Apply modifications to pre/postcommands"""

   cdict['trigger']['precommand'].append('_run_number=%d' % args.run_number)

   if args.perfmon:
      cdict['trigger']['precommand'].insert(0, "include('TrigCommon/PerfMon.py')")

   if args.leak_check:
      doLeakCheck = [] if args.leak_check=='all' else [args.leak_check]

      #early import is needed for proper offloading later
      import Hephaestus.MemoryTracker as memtrack  # noqa
      cdict['trigger']['precommand'].insert(0, "memChkMode='leak-check';doLeakCheck=%s;"
                                            "include('TrigCommon/LeakTracer.py')" % doLeakCheck)

def update_run_params(args):
   """Update run parameters from file/COOL"""

   if args.run_number is None:
      from eformat import EventStorage
      args.run_number = EventStorage.pickDataReader(args.file[0]).runNumber()

   if args.sor_time is None:
      args.sort_time = arg_sor_time(str(AthHLT.get_sor_params(args.run_number)['SORTime']))

   if args.detector_mask is None:
      dmask = AthHLT.get_sor_params(args.run_number)['DetectorMask']
      if args.run_number < AthHLT.CondDB._run2:
         dmask = hex(dmask)
      args.detector_mask = arg_detector_mask(dmask)

def update_nested_dict(d, u):
   """Update nested dictionary (https://stackoverflow.com/q/3232943)"""
   for k, v in u.iteritems():
      if isinstance(v, collections.Mapping):
         d[k] = update_nested_dict(d.get(k, {}), v)
      else:
         d[k] = v
   return d

def set_athena_flags(args):
   """Set athena flags based on command line args"""

   from AthenaCommon.ConcurrencyFlags import jobproperties as jp
   jp.ConcurrencyFlags.NumThreads = args.threads
   jp.ConcurrencyFlags.NumConcurrentEvents = args.concurrent_events
   jp.ConcurrencyFlags.NumProcs = args.nprocs

def HLTMPPy_cfgdict(args):
   """Create the configuration dictionary as expected by HLTMPPy as defined in
   https://gitlab.cern.ch/atlas-tdaq-software/HLTMPPU/blob/master/python/HLTMPPy/runner.py"""

   cdict = {}
   cdict['HLTMPPU'] = {
      'application_name' : 'athenaHLT-%d' % os.getpid(),  # unique name required to avoid interference
      'extra_params' : None,
      'interactive' : args.interactive,
      'log_root' : os.getcwd(),
      'log_name' : ('' if args.unique_log_files else 'athenaHLT:'),
      'module' : 'HLTMPPU',
      'num_forks' : args.nprocs,
      'num_threads' : args.threads,
      'num_slots' : args.concurrent_events,
      'partition_name' : args.partition,
      'hard_timeout' : args.timeout,
      'soft_timeout_fraction' : 0.9
   }

   cdict['datasource'] = {
      'module': 'dffileds',
      'dslibrary': 'DFDcmEmuBackend',
      'compressionFormat': 'ZLIB',
      'compressionLevel': 2,
      'file': args.file,
      'loopFiles': False,
      'numEvents': args.number_of_events,
      'outFile': args.save_output,
      'preload': False,
      'extraL1Robs': args.extra_l1r_robs,
      'skipEvents': args.skip_events
   }

   cdict['global'] = {
      'date': args.sor_time,
      'detector_mask': args.detector_mask,
      'log_root': cdict['HLTMPPU']['log_root'],
      'options_file': None,
      'partition_name': args.partition,
      'ros2robs': args.ros2rob,
      'run_number': args.run_number,
      'save_options': None,
      'solenoid_current': 7730,
      'toroid_current': 20400,
      'with_infrastructure': args.oh_monitoring
   }

   cdict['monitoring'] = {
      'module': 'monsvcis',
      'library': 'MonSvcInfoService',
      'ISInterval': 10,
      'ISRegex': '.*',
      'ISServer': '${TDAQ_IS_SERVER=DF}',
      'ISSlots': 1,
      'OHInterval': args.oh_interval,
      'OHInclude': '.*',
      'OHExclude': '',
      'OHServerName': 'HLT-Histogramming',
      'OHSlots': 5
   }

   cdict['trigger'] = {
      'library': ['TrigPSC'],
      'joType' : args.joboptionsvc_type,
      'msgType' : args.msgsvc_type
   }
   if not args.use_database:      # job options
      cdict['trigger'].update({
         'module': 'joboptions',
         'pythonSetupFile' : args.python_setup,
         'joFile': args.jobOptions,
         'SMK': None,
         'l1PSK': None,
         'l1BG': 0,
         'l1MenuConfig': 'DB',
         'precommand' : args.precommand,
         'postcommand' : args.postcommand,
         'logLevels' : args.log_level
      })
   else:
      cdict['trigger'].update({
         'module': 'DBPython',
         'pythonSetupFile' : args.python_setup,
         'db_alias': args.db_server,
         'SMK': args.smk,
         'l1PSK': args.l1pks,
         'HLTPSK': args.hltpks,
         'l1BG': 0,
         'l1MenuConfig': 'DB',
         'precommand' : args.precommand,
         'postcommand' : args.postcommand,
         'logLevels' : args.log_level
      })
      if args.db_type == "Coral":           # DBPython (with CORAL)
         cdict['trigger'].update({
            'use_coral': True,
            'coral_server': args.db_server
         })
      else:                                 # DBPython (without CORAL)
         cdict['trigger'].update({
            'use_coral': False,
            'db_alias': args.db_server
         })

   return cdict


class MyHelp(argparse.Action):
   """Custom help to hide/show expert groups"""
   def __call__(self, parser, namespace, values, option_string=None):

      for g in parser.expert_groups:
         for a in g._group_actions:
            if values!='all':
               a.help = argparse.SUPPRESS

      parser.print_help()
      if values!='all':
         print('\nUse --help=all to show all (expert) options')
      sys.exit(0)


def main():
   parser = argparse.ArgumentParser(prog='athenaHLT.py', formatter_class=
                                    lambda prog : argparse.ArgumentDefaultsHelpFormatter(prog, max_help_position=32, width=100),
                                    usage = '%(prog)s [OPTION]... -f FILE jobOptions',
                                    add_help=False)
   parser.expert_groups = []   # Keep list of expert option groups

   ## Global options
   g = parser.add_argument_group('Options')
   g.add_argument('jobOptions', help='job options file')
   g.add_argument('--file', '-f', action='append', required=True, help='input RAW file')
   g.add_argument('--save-output', '-o', metavar='FILE', help='output file name')
   g.add_argument('--number-of-events', '-n', metavar='N', default=-1, help='processes N events (<=0 means all)')
   g.add_argument('--skip-events', '-k', metavar='N', default=0, help='skip N first events')
   g.add_argument('--threads', metavar='N', type=int, default=1, help='number of threads')
   g.add_argument('--nprocs', metavar='N', type=int, default=1, help='number of children to fork')
   g.add_argument('--concurrent-events', metavar='N', type=int, help='number of concurrent events if different from --threads')
   g.add_argument('--log-level', '-l', metavar='LVL', type=arg_log_level, default='INFO,ERROR', help='OutputLevel of athena,POOL')
   g.add_argument('--precommand', '-c', metavar='CMD', action='append', default=[],
                  help='Python commands executed before job options or database configuration')
   g.add_argument('--postcommand', '-C', metavar='CMD', action='append', default=[],
                  help='Python commands executed after job options or database configuration')
   g.add_argument('--interactive', '-i', action='store_true', help='interactive mode')
   g.add_argument('--help', '-h', nargs='?', choices=['all'], action=MyHelp, help='show help')

   ## Performance and debugging
   g = parser.add_argument_group('Performance and debugging')
   g.add_argument('--perfmon', action='store_true', help='enable PerfMon')
   g.add_argument('--leak-check', metavar='<stage>', nargs='?', const='execute',
                  choices=['all','initialize','start','beginrun','execute','finalize','endrun','stop'],
                  help='Perform leak checking during <stage>')
   g.add_argument('--tcmalloc', action='store_true', default=True, help='use tcmalloc')
   g.add_argument('--stdcmalloc', action='store_true', help='use stdcmalloc')
   g.add_argument('--stdcmath', action='store_true', help='use stdcmath library')
   g.add_argument('--imf', action='store_true', default=True, help='use Intel math library')
   g.add_argument('--show-includes', '-s', action='store_true', help='show printout of included files')
   g.add_argument('--timeout', metavar='SEC', default=3600*10, help='timeout in seconds')

   ## Database
   g = parser.add_argument_group('Database')
   g.add_argument('--use-database', '-b', action='store_true', help='configure from trigger database')
   g.add_argument('--db-type', default='Coral', choices=['MySQL','Oracle','SQLite','Coral'], help='database type')
   g.add_argument('--db-server', metavar='DB', default='TRIGGERDB', help='DB server name')
   g.add_argument('--smk', type=int, default=0, help='Super Master Key')
   g.add_argument('--l1psk', type=int, default=0, help='L1 prescale key')
   g.add_argument('--hltpsk', type=int, default=0, help='HLT prescale key')

   ## Online histogramming
   g = parser.add_argument_group('Online Histogramming')
   g.add_argument('--oh-monitoring', '-M', action='store_true',
                  help='enable OH monitoring')
   g.add_argument('--oh-interval', metavar='SEC', type=int, default=5,
                  help='seconds between histogram publications.')

   ## Conditions
   g = parser.add_argument_group('Conditions')
   g.add_argument('--run-number', '-R', metavar='RUN', type=int,
                  help='run number (if None, read from first event)')
   g.add_argument('--sor-time', type=arg_sor_time,
                  help='The Start Of Run time. Three formats are accepted: '
                  '1) the string "now", for current time; '
                  '2) the number of nanoseconds since epoch (e.g. 1386355338658000000 or int(time.time() * 1e9)); '
                  '3) human-readable "20/11/18 17:40:42.3043". If not specified the sor-time is read from COOL')
   g.add_argument('--detector-mask', metavar='MASK', type=arg_detector_mask,
                  help='detector mask (if None, read from COOL)')

   ## Expert options
   g = parser.add_argument_group('Expert')
   parser.expert_groups.append(g)
   g.add_argument('--joboptionsvc-type', metavar='TYPE', default='JobOptionsSvc', help='JobOptionsSvc type')
   g.add_argument('--msgsvc-type', metavar='TYPE', default='TrigMessageSvc', help='MessageSvc type')
   g.add_argument('--python-setup', default='TrigPSC/TrigPSCPythonSetup.py', help='Python bootstrap/setup file')
   g.add_argument('--partition', '-p', metavar='NAME', default='athenaHLT', help='partition name')
   g.add_argument('--no-ers-signal-handlers', action='store_true', help='disable ERS signal handlers')
   g.add_argument('--preloadlib', metavar='LIB', help='preload an arbitrary library')
   g.add_argument('--unique-log-files', '-ul', action='store_true', help='add pid/timestamp to worker log files')
   g.add_argument('--extra-l1r-robs', metavar='ROBS', type=arg_eval, default=[],
                  help='List of additional ROB IDs that are considered part of the L1 result and passed to the HLT')
   g.add_argument('--ros2rob', metavar='DICT', type=arg_ros2rob, default={},
                  help='Either a string in the form of python dictionary that contains ros-rob mappings '
                  'or a file path that contains such string. For example, /path/to/rosmap.txt or '
                  '{"ROS0":[0x11205,0x11206],"ROS1":[2120005,2120006]}')
   g.add_argument('--cfgdict', metavar='DICT', type=arg_eval, default={},
                  help='HLTMPPy config dictionary with additional options, e.g.: '
                  '--cfgdict \'{"global": {"log_root" : "/tmp"}}\'')

   args = parser.parse_args()

   # set default OutputLevels and file inclusion
   import AthenaCommon.Logging
   AthenaCommon.Logging.log.setLevel(getattr(logging, args.log_level[0]))
   from AthenaCommon.Include import include
   include.setShowIncludes( args.show_includes )

   # consistency checks for arguments
   if not args.concurrent_events:
      args.concurrent_events = args.threads

   # Update args and set athena flags
   update_run_params(args)
   set_athena_flags(args)

   # get HLTMPPY config dictionary
   cdict = HLTMPPy_cfgdict(args)

   # Apply any expert-level overrides
   update_nested_dict(cdict, args.cfgdict)

   # Modify pre/postcommands if necessary
   update_pcommands(args, cdict)

   # Tell the PSC if we are in interactive mode (relevant for state machine)
   import TrigPSC.PscConfig
   TrigPSC.PscConfig.interactive = args.interactive

   # Select the correct THistSvc
   from TrigServices.TriggerUnixStandardSetup import _Conf
   _Conf.useOnlineTHistSvc = args.oh_monitoring

   # Run HLTMPPU
   from HLTMPPy.runner import runHLTMPPy
   runHLTMPPy(cdict)


if "__main__" in __name__:
   sys.exit(main())
