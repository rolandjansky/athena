# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

'''
Created on Jun 13, 2013

@author: ricab
'''

import logging
from datetime import datetime as dt

class BadOptionSet(Exception): pass
class CurrentlyNotSupported(BadOptionSet): pass

# list of options that may be automatically added by the parser
auto_options = ["help", 'expert-help', 'option-file', 'dump-options', 
                'dump-expert-options']

# list of supported option keys, or (key, value) pairs
# put options here as they become supported
supported =  ['file', 'number-of-events', 'perfmon', 'verbosity', 
              'event-modifier', 'precommand', 'postcommand', 'log-level', 
              'appmgrdll', 'rewind', 'run-number', 
              'save-output', 'save-output-conventional', 
              'ers-debug-level', 'tcmalloc', 'stdcmalloc', 'imf', 'stdcmath',
              'preloadlib', 'msgsvc-type', 'joboptionsvc-type', 'interactive',
              'show-includes', 'use-database', 'db-type', 'db-server', 
              'db-smkey', 'db-hltpskey', 'db-extra', 'sor-time', 
              'detector-mask', 'ros2rob', 'dcm-prefetch-strategy', 'leak-check-execute', 'leak-check', 
              'delete-check', 'no-ers-signal-handlers', 'oh-monitoring', 
              'oh-display', 'user-ipc', 'info-service', 'histogram-include',
              'histogram-exclude', 'histogram-publishing-interval', 
              'appmgrfactory', 'python-setup', 'timeout', 
              'use-compression', 'trace', 'extra-l1r-robs', 'skip-events',
              'muoncal-buffername', 'muoncal-buffersize', 'max-result-size', 
              'debug']

def sor_as_nanos(sorv):
  sor = sor_as_datetime(sorv)
  return int((sor - dt.fromtimestamp(0)).total_seconds() * 1e9)

def sor_as_datetime(sorv):
  sorf = common['sor-time']['format']
  if not sorv:
    return dt.fromtimestamp(0) # epoch
  try: # try interpreting it as now
    return now_sor_as_datetime(sorv)
  except ValueError:
    pass
  try: # try interpreting it as nanos
    return int_sor_as_datetime(sorv)
  except ValueError:
    pass
  try: # try interpreting it as the human readable format
    return hr_sor_as_datetime(sorv, sorf)
  except (ValueError, TypeError):
    raise BadOptionSet("Bad format for option 'sor-time': neither 'now', "
                       "nor int, nor '%s'" % sorf)
    
def now_sor_as_datetime(sorv):
  if sorv == 'now':
    return dt.now()
  raise ValueError

def int_sor_as_datetime(sorv):
  # notice negative numbers are allowed - they represent times before epoch
  nanos = int(sorv) # raises ValueError if not int
  return dt.fromtimestamp(nanos / 1e9)

def hr_sor_as_datetime(sorv, sorf):
  return dt.strptime(sorv, sorf)

####
common = {}
common['extra-l1r-robs'] = \
  {'short': '',
   'arg': True,
   'default': [],
   'group': 'Run mode',
   'description': "List of additional ROB IDs that should be considered part of the level 1 result. Any ROBs with matching IDs in input events will be included in the list of robs that is passed to the HLT when requesting it to process an event. Each element in the list must follow eformat's definition of ROB ID. In particular, it must be integers in the range [0..4294967295]"
  }
common['ros2rob'] = \
  {'short': 'm',
   'arg': True,
   'default': '{}',
   'group': 'Run mode',
   'description': "ROS to ROB map. This should be provided as a dictionary whose keys are ROS IDs - strings - and whose values are lists of ROB IDs - integers in the range [0..4294967295]. This dictionary can be provided either directly on the command line (e.g. --ros2rob '{\"foobar\": [0xABCD0001, 0xABCD0002]}'), or through a python module that contains it in the variable ros2rob (e.g. --ros2rob 'mymod', where mymod.py contains 'ros2rob={\"bla\": [0x123, 0x321]}'). In the latter case, the module should be accessible with a regular import (it should be in the PYTHONPATH). Such a module can be obtained from a partition with the script ros2rob_from_partition.py, which is included in this package. Any ROB that is not covered by the dictionary and that comes up in an event is considered as part of an artificial ROS corresponding to its subdetector. Notice, in particular, that is how all ROBs are treated when an empty dictionary is provided (the default)! In that case all ROBs are assumed to be in their SubDetector's artificial single ROS. If a non-empty dictionary is provided and, at runtime, a ROB is requested which is not in the provided map, a big WARNING is printed, as this indicates an incomplete mapping."
   }
common['dcm-prefetch-strategy'] = \
  {'short': '',
   'arg': True,
   'default': 0,
   'group': 'Run mode',
   'description': "ROB prefetching strategy in DCM: = 0: retrieve ROBs on prefetch list only when needed for decoding (as in Run 1), = 1: retrieve ROBs on prefetch list immediately (as begin on Run 2)"
  }
common['save-output'] = \
  {'short': 'o', 
   'arg': True, 
   'default': '',
   'group': 'Data',
   'description': 'Output events with the HLT result to the specified file. This parameter only sets the filename core (a sequence number and extension is still appended). If multiple input files are given, multiple output files are generated. This option cannot be used simultaneously with --save-output-conventional.'}
convallowed = {'dir': '.',
               'ProjectTag': 'UNKNOWN',
               'RunNumber': 0,
               'StreamType': 'UNKNOWN',
               'StreamName': 'UNKNOWN',
               'LumiBlockNumber': 0,
               'ProductionStep': 'UNKNOWN'}
common['save-output-conventional'] = \
  {'short': 'O',
   'arg': True,
   'default': {},
   'group': 'Data',
   'description': 'Output events with the HLT result to files whose full names are derived from the specified dictionary, following the Atlas RAW data file naming convention. The dictionary can have between 0 and 7 items (inclusive). The only keys allowed are: %s. The default values for these keys are, respectively: %s. The specified values must have the same type as these defaults, that is, respectively: %s. Properties that are not specified are derived, for each output file, from the corresponding input file if it follows Atlas RAW naming convention. Otherwise, they the default values are used.' % (convallowed.keys(), convallowed.values(), [type(v) for v in convallowed.values()]), 
   'allowed': convallowed}
common['use-compression'] = \
  {'short': 'z', 
   'arg': True, 
   'default': 0,
   'group': 'Data',
   'description': 'If set, written output data are compressed with the specified compression level. The compression level should be specified as an integer value between 0 and 5 (inclusive). Recommended value -z1. -z0 means no compression applied. This option requires either --save-output or --save-output-conventional.'}
common['verbosity'] = \
  {'short': 'V', 
   'arg': True, 
   'group': 'Run mode',
   'default': logging.INFO,
   'description': 'From which level to print system messages [%d, %d]. For details please consult the documentation of python\'s "logging" module' % (logging.NOTSET, logging.CRITICAL)}
common['timeout'] = \
  {'short': None, 
   'arg': True, 
   'default': {'timeout': 0, 'warn_fraction': 0.8},
   'group': 'Run mode',
   'description': 'This option determines if a watchdog will be used to monitor the event processing. If the dictionary entry "timeout" is set, it should be set to the maximum amount of time HLT should process every event, in milliseconds. The second parameter determines the fraction of that timeout in which the HLT framework will be notified of an eventual timeout.'}
common['precommand'] = \
  {'short': 'c', 
   'arg': True, 
   'default': [],
   'group': 'Run mode',
   'description': 'Optional python commands executed before jobOptions script or database configuration'}
common['postcommand'] = \
  {'short': 'C', 
   'arg': True, 
   'default': [],
   'group': 'Run mode',
   'description': 'Optional python commands executed after jobOptions script or database configuration'}
common['number-of-events'] = \
  {'short': 'n', 
   'arg': True, 
   'default': -1,
   'group': 'Data',
   'description': 'Processes this number of events (<=0 means all).'}
common['skip-events'] = \
  {'short': 'k',
   'arg': True,
   'default': 0,
   'group': 'Data',
   'description': 'Skip these many events. Not allowed with "--interactive" nor "--rewind". Must be smaller than the number of events in the input stream. '}
common['rewind'] = \
  {'short': 'r', 
   'arg': False, 
   'default': None,
   'group': 'Data',
   'description': 'Rewind to first event in prepareForRun. Useful in interactive mode with multiple runs and running over the same events.'}
common['ers-debug-level'] = \
  {'short': 'L', 
   'default': 0,
   'arg': True, 
   'group': 'Run mode',
   'description': 'An ERS debug level to be set dynamically, in the range [0,3].  This will overwrite your environmental settings for TDAQ_ERS_DEBUG_LEVEL.'}
oh_warn = "This option can only be used together with --oh-monitoring."
common['oh-monitoring'] = \
  {'short': 'M', 
   'arg': False, 
   'default': None,
   'group': 'Online Histogramming',
   'description': 'Run with OH monitoring'}
common['oh-display'] = \
  {'short': 'W', 
   'arg': False, 
   'default': None,
   'group': 'Online Histogramming',
   'description': 'Launch an histogram display. ' + oh_warn}
common['info-service'] = \
  {'short': 'x',
   'arg': True,
   'default': 'MonSvcInfoService',
   'group': 'Online Histogramming',
   'description': 'The IInfoRegister that should be used as TrigMonTHistSvc back-end. ' + oh_warn}  
common['user-ipc'] = \
  {'short': 'I', 
   'arg': False, 
   'default': None,
   'group': 'Online Histogramming', 
   'description': 'Use the IPC init file pointed to by the environment variable TDAQ_IPC_INIT_REF. ' + oh_warn}
common['histogram-publishing-interval'] = \
  {'short': 'U',
   'arg': True,
   'default': 5,
   'group': 'Online Histogramming',
   'description': 'Positive integer determining the number of seconds between each two consecutive online histogram publications. ' + oh_warn}
common['histogram-exclude'] = \
  {'short': '',
   'arg': True,
   'default': '',
   'group': 'Online Histogramming',
   'description': 'Regular expression describing the histograms that should be excluded from online publishing. ' + oh_warn}
common['histogram-include'] = \
  {'short': '',
   'arg': True,
   'default': '.*',
   'group': 'Online Histogramming',
   'description': 'Regular expression describing the histograms that should be included in online publishing. ' + oh_warn}
common['show-includes'] = \
  {'short': 's', 
   'arg': False, 
   'default': None,
   'group': 'Run mode',
   'description': 'Show include(s) resolution in python jobOptions files'}
common['log-level'] = \
  {'short': 'l', 
   'arg': True, 
   'default': 'INFO,ERROR',
   'group': 'Run mode',
   'description': 'Gaudi output level, Pool output level'}
common['msgsvc-type'] = \
  {'short': 'T', 
   'arg': True, 
   'default': 'TrigMessageSvc',
   'group': 'Gaudi configuration',
   'description': 'Message service type'}
common['python-setup'] = \
  {'short': 'K', 
   'arg': True, 
   'default': '',
   'group': 'Gaudi configuration',
   'description': 'Python bootstrap/setup file for Steering Controller'} 
common['joboptionsvc-type'] = \
  {'short': 'J', 
   'arg': True, 
   'default': 'JobOptionsSvc',
   'group': 'Gaudi configuration',
   'description': 'JobOptions service type'}
common['run-number'] = \
  {'short': 'R', 
   'arg': True,
   'default': 0, 
   'group': 'Run mode',
   'description': 'Defines the RunNumber to use for this run. If you do not set this value, it will be read from the first event of the first datafile you give me. If you run in interactive mode and supply a run number in the prepareForRun transition, it will overwrite this value permanently.'}
common['detector-mask'] = \
  {'short': '',
   'arg': True,
   'default': 0,
   'group': 'Run mode',
   'description': 'Defines the detector mask to use for this run. A value of 0 means that the detector mask should be picked up from COOL.'}
common['sor-time'] = \
  {'short': '',
   'arg': True,
   'default': None,
   'group': 'Run mode',
   'description': 'The Start Of Run time. Three formats are accepted: 1) the string "now", for current time; 2) the number of nanoseconds since epoch (e.g. 1386355338658000000 or int(time.time() * 1e9)); 3) the following human readable format (according to python\'s datetime directives): "%s". By default the sor-time is obtained from the Conditions DB. This happens also if the input evaluates to false or if it represents the date "1/1/70 1:0:0.0"',
   'format': '%d/%m/%y %H:%M:%S.%f'}
common['sor-time']['description'] %= common['sor-time']['format']
common['sor-time']['parse'] = sor_as_nanos
common['event-modifier'] = \
  {'short': 'Z', 
   'arg': True,
   'default': [], 
   'group': 'Data',
   'description': 'If set, this should be a list of python modules containing at least one of the following functions: "modify" or "modify_general". The former takes exactly one argument: the event to be modified. The latter takes keyword arguments: currently, the event to be modified and the current configuration, under the keywords "event" and "configuration", respectively. Both functions must return, either a valid event, or something evaluating to False, to indicate that the event should be skipped. For each provided module, if it contains the function "modify_general", that function is called ("modify" is not called in this case, even in present). Otherwise, the function "modify" is called. If several modifiers are provided, they are called in sequence, until either one of them returns something that evaluates to False or until all of them have been processed.'}
common['max-result-size'] = \
  {'short': '', 
   'arg': True, 
   'default': -1,
   'group': 'Run mode',
   'description': 'Changes the maximum size of the HLTResult. The size is given in multiples of 32-bit words. A size of 1 means actually 4 bytes in this case. Any number smaller than zero will trigger the usage of the default buffer size, as defined in the hltinterface package.'} 
common['leak-check-execute'] = \
  {'short': 'Q', 
   'arg': False,
   'default': None, 
   'group': 'Run mode',
   'description': 'Perform leak checking during execute. Equivalent to: --leak-check="execute".'}
checkallowed = ('all', 'initialize', 'start', 'beginrun', 'execute', 'finalize',
               'endrun', 'stop' )
common['leak-check'] = \
  {'short': '', 
   'arg': True,
   'default' : None, 
   'group': 'Run mode',
   'description': 'Perform leak checking during the stage you specify %s. Syntax: --leak-check="<stage>" Example: --leak-check="all"' % str(checkallowed),
   'allowed': checkallowed}
common['delete-check'] = \
  {'short': '', 
   'arg': True,
  'default' : None, 
  'group': 'Run mode',
  'description': 'Perform double delete checking at the stage you specify %s. Syntax: --delete-check="<stage>" Example: --delete-check="all"' % str(checkallowed),
  'allowed': checkallowed}
common['perfmon'] = \
  {'short': 'H', 
   'arg': False,
   'default': None, 
   'group': 'Run mode',
   'description': 'Enables performance monitoring toolkit'}
debugallowed = ('configure', 'connect', 'prepareForRun', 'run', 'stop', 
                'disconnect', 'unconfigure')
common['debug'] = \
  {'short': 'd', 
   'arg': True, 
   'default': '',
   'group': 'Run mode',
   'description': 'Attaches GDB just before the stage you specify %s.' % str(debugallowed),
   'allowed': debugallowed}
common['appmgrdll'] = \
  {'short': 'N', 
   'arg': True, 
   'default': '',
   'group': 'Gaudi configuration', 
   'description': 'Sets the Application Manager DLL'}
common['appmgrfactory'] = \
  {'short': 'P',
   'arg': True,
   'default': '',
   'group': 'Gaudi configuration', 
   'description': 'Sets the Application Manager factory'}
common['trace'] = \
  {'short': 'B', 
   'arg': True,
   'default': '',
   'group': 'Run mode',
   'description': 'Also shows include files that match the given pattern'}
common['tcmalloc'] = \
  {'short': '',
   'arg': False,
   'default': None, 
   'group': 'Run mode', 
   'description': 'Use tcmalloc instead of stdcmalloc [DEFAULT].'
   'This option is incompatible with --leak-check, --leak-check-execute and --delete-check. '}
common['stdcmalloc'] = \
  {'short': '', 
   'arg': False, 
   'default': None, 
   'group': 'Run mode', 
   'description': 'Use stdcmalloc intead of tcmalloc.'}
common['imf'] = \
  {'short': '', 
   'arg': False, 
   'default': None, 
   'group': 'Run mode', 
   'description': "Use Intel's imf library, instead of stdcmath [DEFAULT]."}
common['stdcmath'] = \
  {'short': '', 
   'arg': False, 
   'default': None, 
   'group': 'Run mode', 
   'description': "Use stdcmath, instead of Intel's imf library."}
common['preloadlib'] = \
  {'short': '', 
   'arg': True, 
   'default': None, 
   'group': 'Run mode', 
   'description': 'Preload an arbitrary library, to be specified with an equals sign (e.g. --preloadlib=foobar.so).'}
common['no-ers-signal-handlers'] = \
  {'short': '', 
   'arg': False,
   'default': None, 
   'group': 'Run mode', 
   'description': 'Do not use the ERS signal handlers.'}
common['muoncal-buffername'] = \
  {'short': '', 
   'arg': True, 
   'default': '',
   'group': 'Muon calibration',
   'description': 'Sets the name of the muon calibration buffer'}
common['muoncal-buffersize'] = \
  {'short': '', 
   'arg': True, 
   'default': '',
   'group': 'Muon calibration',
   'description': 'Sets the size of the muon calibration buffer'}
usedb_warn = 'This option can only be used together with --use-database.'
common['use-database'] = \
  {'short': 'b', 
   'arg': False, 
   'default': None,
   'group': 'Database',
   'description': 'Configure from the TriggerDB.'}
common['db-type'] = \
  {'short': 't', 
   'arg': True, 
   'default': None, 
   'group': 'Database', 
   'allowed': ['MySQL', 'Oracle', 'SQLite', 'Coral'],
   'description': 'Select the type of database to configure from. Allowed ' +
   'values are: ' # see below
   }
common['db-type']['description'] += (
  ', '.join(['"%s"' % x for x in common['db-type']['allowed']]) + ". " + 
  usedb_warn 
  )
common['db-server'] = \
  {'short': 'S',
   'arg': True,
   'default': None,
   'group': 'Database',
   'description': 'The meaning of this parameter depends on the value of db-type. When db-type is "Coral", db-server identifies an entry for DB lookup. Otherwise, it should contain the name of the server (e.g. "ATLAS_CONFIG"). ' + usedb_warn}
common['db-smkey'] = \
  {'short': 'X',
   'arg': True,
   'default': None,
   'group': 'Database',
   'description': 'The Super Master key to read from the trigger database. This should be a single positive integer. ' + usedb_warn}
common['db-hltpskey'] = \
  {'short': 'Y',
   'arg': True,
   'default': None,
   'group': 'Database',
   'description': 'The HLT PreScale key(s) to retrieve from the trigger database. This can be either a single positive integer, or a non-empty list of pairs of positive integers (e.g. [(1, 123), (100, 321)]). In the latter case, the list must be ordered by the first element of each pair, following a strict growing order. The first element of each pair represents a lumiblock number, while the second represents a prescale key. A prescale key specified by the second element of a pair is used for all the lumiblocks greater or equal to the lumiblock specified by the first element of the same pair and, if the pair is not the last on the list, less than lumiblock specified by the first element of the following pair. Therefore, in the example above the key 123 would be used for lumiblocks 1 to 99 (inclusive) and only for these ones; the key 321 would be used for all other lumiblocks (100 or greater). ' + usedb_warn}
common['db-extra'] = \
  {'short': 'E',
   'arg': True,
   'default': {},
   'group': 'Database',
   'description': 'Dictionary of free key-value combinations. Each element whose key is "recognized" is extracted and treated individualy to make sure it reaches the software below by the proper means (through the corresponding individual ptree node). Other elements are passed below as "additionalConnectionParameters" (in the ptree node with that name). The "recognized" parameters are: "lvl1key", "schema" (corresponding to TriggerDBConnection.Name (e.g. "ATLAS_CONF_TRIGGER_V2")), "user", "password". The remaining parameters include "retrialperiod", and "maxretrials". ' + usedb_warn}



# Options that deal with file input
fileinput = {}
fileinput['file'] = {'short': 'f', 'arg': True, 'default': [],
                     'group': 'Data',
                     'description': 'The input data-file, or a python list thereof. Multiple files can only be specified with --oh-monitoring, because the implicit stop/start transitions at file boundaries are not supported by the default (offline) histogramming service.'}
fileinput['interactive'] = {'short': 'i', 'arg': False, 'default': None,
                            'description': 'Switches on interactive mode, so you can control the state transitions manually. This option can only be used together with --oh-monitoring, because the default (offline) histogramming service does not support arbitrary state transitions.',
                            'group': 'Run mode'}

# Options that deal with emon input
emoninput = {} # empty for now

# constants (only common_constants for now)
common_constants = {'ohserver': 'HLT-Histogramming',
                    'rootprovider': "athenaHLT",
                    'oh_numslots': 5}

# helper procedure to get the available short options
def __short_avail():
  from string import lowercase as lw, uppercase as up
  
  all_short = set(lw + up)
  reserved_short = set(['h', 'D', 'F'])
  used_short = set([v['short'] for v in (fileinput.values() + 
                                         emoninput.values()) + 
                                         common.values()])
  return all_short - used_short - reserved_short

if __name__ == '__main__':
  print ("(No tests in this file. All option tests are in option.py. Running "
         "this file prints the available letters for athenaHLT options)")
  print
  print "Available letters for options are:",
  print ''.join(sorted(__short_avail()))
