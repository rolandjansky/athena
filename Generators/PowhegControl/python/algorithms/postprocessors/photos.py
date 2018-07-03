# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
from AthenaCommon import Logging
from ...decorators import timed
from ...utility import ProcessManager, SingleProcessThread

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("PHOTOS post-processing")
def PHOTOS(process):
    """! Run PHOTOS over pre-generated Powheg events.

    @param process            External PHOTOS process.

    @author James Robinson <james.robinson@cern.ch>
    """
    logger.info("Running PHOTOS afterburner")
    if not os.path.isfile(process.executable):
        raise OSError("PHOTOS executable {} not found!".format(process.executable))
    processes = [SingleProcessThread(process.executable)]
    manager = ProcessManager(processes)
    while manager.monitor():
        pass
