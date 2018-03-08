# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl GenerateSingleCore
#  Run a single Powheg process in its own thread
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from ..utility import ProcessHandling

# Run single process
def generate_single_core(configurator):
    processes = [ProcessHandling.SingleProcessThread(configurator._powheg_executable)]
    manager = ProcessHandling.ProcessManager(processes)
    while manager.monitor():
        pass
