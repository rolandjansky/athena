# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import glob
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("directory cleaner")
def directory_cleaner(*args):
    """! Clean up the directory before running.

    Remove any existing LHE files to avoid repeated events.

    @author James Robinson <james.robinson@cern.ch>
    """
    for LHE_file in glob.glob("*.lhe") + glob.glob("*.ev*ts"):
        logger.warning("Removing existing LHE file: {}".format(LHE_file))
        os.remove(LHE_file)
