# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl ProcessHandling
#  Utility classes to handle Powheg processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
import subprocess
import time
from NonBlockingStreamReader import NonBlockingStreamReader

## A wrapper to handle multiple Powheg subprocesses
class ProcessManager :
  def __init__(self, process_list) :
    self.__process_list = process_list
    self.__n_initial = len(process_list)


  ## Monitor each of the managed processes and log when they are finished
  def monitor(self, logger) :
    for idx, process in enumerate(self.__process_list) : process.id_number = idx+1
    while len( self.__process_list ) > 0 :
      for process in list(self.__process_list) :
        if not process.has_output() :
          self.__process_list.remove( process )
          logger.info( 'Finished process #{0}: there are now {1}/{2} running'.format( process.id_number, len(self.__process_list), self.__n_initial ) )



## A single Powheg subprocess
class SingleProcessThread :
  log_level = { 'stdout':'info', 'stderr':'error' }

  ## Setup underlying process together with non-blocking readers for stdout and stderr
  def __init__(self, command_list, logger, seed_index=None) :
    self.__logger = logger
    if not isinstance( command_list, list ) : command_list = [ command_list ]
    self.__process = subprocess.Popen( [str(x) for x in command_list], stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE )
    self.__output_prefix = [ 'Process #{0}: '.format( seed_index ), '' ][ seed_index is None ]
    # Write seed to stdin
    if seed_index is not None :
      self.__process.stdin.write( str(seed_index) )
      self.__process.stdin.close()
      with open('pwgseeds.dat','rb') as seed_file : random_seed_list = seed_file.read().splitlines()
      self.log( 'Providing random seed: {0}'.format( random_seed_list[seed_index-1] ) )
    # Setup non-blocking stream readers for stdout and stderr
    self.__stdout = NonBlockingStreamReader(self.__process.stdout)
    self.__stderr = NonBlockingStreamReader(self.__process.stderr)


  ## Write queued output and return process status
  def has_output( self ) :
    status = self.is_running()
    self.write_queued_output()
    return status


  ## Check if the underlying process is running and finalise stream readers if not
  def is_running( self ) :
    if self.__process.poll() is not None : # process has ended
      [ getattr(self, nbsr).finalise() for nbsr in ['stdout','stderr'] ]
      return False
    return True


  ## Write to the logger with appropriate log-level
  def log( self, message, log_level='info' ) :
    getattr( self.__logger, log_level )( '{0}{1}'.format( self.__output_prefix, message ) )


  ## Pass queued output to the logger
  def write_queued_output( self ) :
    for stream in [ 'stdout', 'stderr' ] :
      while True :
        output = getattr( self, stream ).readline(timeout=0.1)
        if output is None or len(output) == 0 : break
        self.log( output, self.log_level[stream] )


  ## stdout stream from underlying process
  @property
  def stdout(self) :
    return self.__stdout


  ## stderr stream from underlying process
  @property
  def stderr(self) :
    return self.__stderr
