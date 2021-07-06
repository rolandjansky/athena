# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from ...decorators import timed
from ...utility import  ProcessManager, SingleProcessThread
import os


@timed("single-core generation")
def singlecore(process):
    """! Run a single Powheg process in its own timed thread.

    @param process  PowhegBox process.

    @author James Robinson  <james.robinson@cern.ch>
    """
    singlecore_untimed(process)


def singlecore_untimed(process):
    """! Run a single Powheg process in its own thread.

    @param process  PowhegBox process.

    @author James Robinson  <james.robinson@cern.ch>
    """
    if not os.path.isfile(process.executable):
        raise OSError("Powheg executable {} not found!".format(process.executable))
    threads = [SingleProcessThread(process.executable)]
    manager = ProcessManager(threads)
    while manager.monitor():
        pass
