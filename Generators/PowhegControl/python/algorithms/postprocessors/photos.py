# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import os
import shutil
from AthenaCommon import Logging
from ...decorators import timed
from ...utility import ProcessManager, SingleProcessThread

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("PHOTOS post-processing")
def PHOTOS(process, powheg_LHE_output):
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

    # Get file names
    input_LHE_events = powheg_LHE_output
    photos_output = "pwgevents_photos.lhe"

    # Rename output file
    if os.path.isfile(input_LHE_events):
        shutil.move(input_LHE_events, "{}.undecayed_ready_for_photos".format(input_LHE_events))
    shutil.move(photos_output, input_LHE_events)
    logger.info("Moved {} to {}".format(photos_output, input_LHE_events))
