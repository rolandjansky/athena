# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#########################################################################################################################
#
#   Run a single Powheg process
#
#   Authors: James Robinson  <james.robinson@cern.ch>
#
#########################################################################################################################

#! /usr/bin/env python
import subprocess
from OutputHandler import write_output

###############################################################################
#
#  Register single Powheg process
#
###############################################################################
def runPowhegSingleThread(configurator, stdin=None, output_prefix='', display_output=True) :
  configurator.running_processes.append( (subprocess.Popen( [configurator._powheg_executable,''], stdout=subprocess.PIPE, stdin=stdin, stderr=subprocess.STDOUT ), output_prefix) )
  if display_output :
    write_output( configurator.running_processes, configurator.logger )
