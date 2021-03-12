# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import glob
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("directory cleaner")
def directory_cleaner(process):
    """! Clean up the directory after running.

    Remove any leftover files to avoid clutter.

    @author James Robinson <james.robinson@cern.ch>
    """
    nRemoved, nFailures = 0, 0
    for file_name in sum([glob.glob(_f) for _f in process.files_for_cleanup], []):
        logger.debug("... removing unneeded file: {}".format(file_name))
        try:
            os.remove(file_name)
        except OSError:
            logger.debug("... could not remove {}".format(file_name))
            nFailures += 1
        nRemoved += 1
    logger.info("... removed {} unneeded files with a further {} failures".format(nRemoved, nFailures))
