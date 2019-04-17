#! /usr/bin/env python

## \file Herwig7Utils.py
## \brief Module with helper functions and classes
## \author Daniel Rauch (daniel.rauch@desy.de)

import math, os, subprocess, time

from AthenaCommon import Logging
athMsgLog = Logging.logging.getLogger('Herwig7Utils')

integration_grids_precision_threshold = 0.05 # warn if integration xsec below


## Class for handling commands to modify the generator configuration
##
## The specific purpose of this class is to provide a mechanism for the locking
## of the generator configuration after the creation of the Herwig7 input file.
## The input file gets written upon execution of onoe of the run or build
## functions of the specific generator configuration object. Beyond this point
## no further modification of the configuration in the job options can be
## propagated and used in the Herwig7 run.
class ConfigurationCommands:

  def __init__(self):
    self.commands = ""
    self.locked = False
  
  def lock(self):
    self.locked = True
  
  def add(self, commands):
    if self.locked == False:
      self.commands += commands
    else:
      raise RuntimeError("The commands associated to this configuration object can't be modified anymore because the Herwig7 infile has already been written to disk. Therefore, any subsequent additional modifications can't be adopted in the Herwig7 run.")
    return(self)
  
  __iadd__ = add


## Class for handling parallel local multi-processing
##
## Provides management functionality for waiting for a set of subprocesses
## to terminate and catch success/failure
class ProcessHandler:

  def __init__(self, process_list, logger):
    self.processes = process_list
    self.n_initial = len(process_list)
    self.logger = logger

  def success(self):
    result = True
    while len(self.processes) > 0:
      for ID, process in enumerate(self.processes):
        returncode = process.poll()
        if not returncode is None:

          ## print exit status and number of pending process
          self.processes.remove(process)
          if returncode == 0:
            athMsgLog.info(ansi_format_info('Process #{} finished sucessfully, {}/{} still running'.format(process.ID, len(self.processes), self.n_initial)))
          elif returncode > 0:
            athMsgLog.error(ansi_format_error("Process #{} finished with error code {} (please check logfile '{}'), {}/{} still running".format(process.ID, returncode, process.logfile_title, len(self.processes), self.n_initial)))
            result = False
          elif returncode < 0:
            athMsgLog.error(ansi_format_error("Process #{} was terminated by signal {} (please check logfile '{}'), {}/{} still running".format(process.ID, -returncode, process.logfile_title, len(self.processes), self.n_initial)))
            result = False

          ## attach integration log to main log file
          # self.logger.info('================================================================================')
          # self.logger.info("Including content of integration log file '{}'".format(process.logfile_title))
          # self.logger.info('================================================================================')
          athMsgLog.info("Content of integration log file '{}':".format(process.logfile_title))
          athMsgLog.info("")
          with open(process.logfile_title, 'r') as logfile:
            for line in logfile:
              athMsgLog.info('  {}'.format(line.rstrip('\n')))
          athMsgLog.info("")
          # self.logger.info('================================================================================')
          # self.logger.info("End of integration log file '{}'".format(process.logfile_title))
          # self.logger.info('================================================================================')

    return(result)


## Class for handling a single process
##
## This class provides the infrastructure to spawn a new subprocess from python,
## pipe the output to a logfile and check the status of the subprocess
class Process:

  def __init__(self, ID, command, logfile_title):

    athMsgLog.info(ansi_format_info("Starting subprocess #{} with command '{}' and logfile '{}'".format(ID, ' '.join(command), logfile_title)))
    self.ID = ID
    self.logfile_title = logfile_title
    self.logfile = open(logfile_title, 'w')
    self.process = subprocess.Popen(command, stdout=self.logfile, stderr=self.logfile)

  def poll(self):
    return(self.process.poll())


## Calculate the total cross section from the integration log files
def get_cross_section(run_name, integration_jobs=1):

  athMsgLog.info("Calculating cross section after integration")
  logfiles = [run_name+'.integrate'+str(integration_job)+'.log' for integration_job in range(integration_jobs)]

  xsec = 0.0
  err  = 0.0

  for logfile in logfiles:

    athMsgLog.info("- {}".format(logfile))

    ## entire file content
    with open(logfile, 'r') as log: data = log.read().strip()

    with open(logfile, 'r') as log:

      ## get number of subprocesses that are integrated
      for line in log:
        if 'Integrate ' in line:
          n_subprocs = int(line.split('of')[1].replace(':',''))
          athMsgLog.info("  found {} subprocesses".format(n_subprocs))
          break

    data = data.split("Integrate ")[1:]
  
    for s, subproc in enumerate(data, start=1):
      _xsec = 0.0
      _err  = 0.0
      for line in subproc.split("\n"):
        if 'integrated ( ' in line:
          _xsec = float(line.split()[2])
          _err = float(line.split()[4])
      athMsgLog.info("  - subprocess {}: xsec = {} +/- {} nb".format(s, _xsec, _err))
      xsec += _xsec
      err  += _err*_err

  err = math.sqrt(err)

  if err / xsec > integration_grids_precision_threshold:
    threshold = '{}%'.format(integration_grids_precision_threshold*100.0)
    athMsgLog.warn(ansi_format_warning('! The integration grids only have a low precision (worse than {}): xsec = {} +/- {} nb (accuracy: {:.3f}%)'.format(threshold, xsec, err, err/xsec*100.0)))
    athMsgLog.warn(ansi_format_warning('! In order to speed up the event generation you should consider improving the statistics of the integration / phase space sampling stage (see the sampler_commands() function).'))
  else:
    athMsgLog.info(ansi_format_info('After integration the estimated cross section was found to be: xsec = {} +/- {} nb (accuracy: {:.3f}%)'.format(xsec, err, err/xsec*100.0)))

  return(xsec, err)


## Return a string made up of a certain number of repetitions of the same pattern
def get_repeated_pattern(pattern, repetitions):

  return(pattern.join(['' for i in range(repetitions+1)]))


## Get size of file or folder
def get_size(path):

  size = 0
  
  if os.path.isfile(path):
    return(os.path.getsize(path))

  elif os.path.isdir(path):
    for dirpath, dirs, files in os.walk(path):
      for file in files:
        file_path = os.path.join(dirpath, file)
        # if not os.path.islink(file_path): # exclude symlinks because MadGraph has lots of broken symlinks
        if os.path.isfile(file_path):
          size += os.path.getsize(file_path)

  return(size)


## Convert file/folder size from bytes to units with appropriate prefixes (multiples of 1000, not 1024!)
def humanize_bytes(bytes, precision=2):
  # shamelessly stolen from
  # http://code.activestate.com/recipes/577081-humanized-representation-of-a-number-of-bytes/

  abbrevs = (
      (1<<50L, 'PiB'),
      (1<<40L, 'TiB'),
      (1<<30L, 'GiB'),
      (1<<20L, 'MiB'),
      (1<<10L, 'KiB'),
      (1, 'byte(s)')
  )
  if bytes == 1:
    return '1 byte'
  for factor, suffix in abbrevs:
    if bytes >= factor:
      break
  return '%.*f %s' % (precision, bytes / factor, suffix)


## Format and colorize terminal output.
def ansi_format(text, colour='None', background_colour='None',
           bold=False, underline=False, italics=False, marked=False, strikethrough=False):

  format_none          = '\033[0m'
  format_bold          = '\033[1m'
  format_underline     = '\033[4m'
  format_italics       = '\033[3m'
  format_marked        = '\033[7m'
  format_strikethrough = '\033[9m'

  colours = {
    'None': '',
    'Black':     '\033[30m',
    'Grey':      '\033[90m',
    'Red':       '\033[91m', 'DarkRed':   '\033[31m',
    'Green':     '\033[92m', 'DarkGreen': '\033[32m',
    'Yellow':    '\033[93m', 'Orange':    '\033[33m',
    'Blue':      '\033[94m', 'DarkBlue':  '\033[34m',
    'Pink':      '\033[95m', 'DarkPink':  '\033[35m',
    'Cyan':      '\033[96m', 'DarkCyan':  '\033[36m',
    'White':     '\033[97m', 'LightGrey': '\033[37m',
  }

  background_colours = {
    'None': '',
    'Black':  '\033[40m',
    'Red':    '\033[41m',
    'Green':  '\033[42m',
    'Orange': '\033[43m',
    'Blue':   '\033[44m',
    'Punk':   '\033[45m',
    'Cyan':   '\033[46m',
    'Grey':   '\033[47m',
  }

  if colour in colours:
    if background_colour in background_colours:
      return (format_none
        + colours[colour]
        + background_colours[background_colour]
        + (format_bold          if bold          else '')
        + (format_underline     if underline     else '')
        + (format_italics       if italics       else '')
        + (format_marked        if marked        else '')
        + (format_strikethrough if strikethrough else '')
        + text + format_none)
    else:
      raise Exception("Could not find background colour '{}'.".format(background_colour))
  else:
    raise Exception("Could not find colour '{}'.".format(colour))

## Green colouring
def ansi_format_ok(text):
  return (ansi_format(text, "Green"))

## Blue colouring
def ansi_format_info(text):
  return (ansi_format(text, "Blue"))

## Yellow colouring
def ansi_format_warning(text):
  return (ansi_format(text, "Yellow"))

## Red colouring
def ansi_format_error(text):
  return (ansi_format(text, "Red"))
