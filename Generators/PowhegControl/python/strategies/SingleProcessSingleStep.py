# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Script to run a single Powheg powheg
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import subprocess

###############################################################################
#
#  Register single Powheg process
#
###############################################################################
def runSingleProcessSingleStep(configurator, stdin=None, output_prefix='', display_output=True) :
  configurator.running_processes.append( (subprocess.Popen( [configurator._powheg_executable,''], stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.STDOUT ), output_prefix) )
  if display_output :
    displayProcessOutput(configurator)

###############################################################################
#
#  Print Powheg output to screen
#
###############################################################################
def displayProcessOutput(configurator) :
  while configurator.running_processes :
    # Write stdout if any
    for process, output_prefix in configurator.running_processes :
      stdout = process.stdout.readline().rstrip()
      if len(stdout) > 0 : configurator._logger.info( '{0}{1}'.format(output_prefix,stdout) )
    # Check for finished processes and remove them from the list
    for process, output_prefix in configurator.running_processes :
      if process.poll() is not None : # process has ended
        stdout = process.stdout.readline().rstrip()
        if len(stdout) > 0 : configurator._logger.info( '{0}{1}'.format(output_prefix,stdout) ) # Print final output (if any) to screen
        process.stdout.close()
        configurator.running_processes.remove( (process,output_prefix) )
        configurator._logger.info( '{0}Process finished: there are now {1}/{2} running'.format(output_prefix,len(configurator.running_processes),configurator._n_cores) )
