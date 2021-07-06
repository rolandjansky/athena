# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import os

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("output validation")
def output_validator(powheg_LHE_output):
    """! Check for required output file.

    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author James Robinson <james.robinson@cern.ch>
    """
    if not os.path.isfile(powheg_LHE_output):
        logger.warning("No output LHEF file found! Probably because the Powheg process was killed before finishing.")
        raise RuntimeError("No output LHEF file produced by Powheg. Terminating job.")
