# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("output file renaming")
def output_file_renamer(output_events_name, powheg_LHE_output):
    """! Move output to correctly named file.

    @param output_file_name   Name of output LHE file.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author James Robinson <james.robinson@cern.ch>
    """
    try:
        os.rename(powheg_LHE_output, output_events_name)
        logger.info("Moved {} to {}".format(powheg_LHE_output, output_events_name))
    except OSError:
        logger.warning("No output LHEF file found! Probably because the Powheg process was killed before finishing.")
        raise RuntimeError("No output LHEF file produced by Powheg. Terminating job.")
