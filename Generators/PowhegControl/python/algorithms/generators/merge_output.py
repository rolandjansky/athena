# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
from ...utility import FileParser, LHE
import glob
import subprocess

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("merge output")
def merge_output(cores, n_events_unscaled, powheg_LHE_output):
    """! Merge output events if running in multicore mode.

    @param cores              Number of cores in use.
    @param n_events_unscaled  Total number of events generated.
    @param powheg_LHE_output  Name of LHE file produced by PowhegBox.

    @author James Robinson <james.robinson@cern.ch>
    """
    logger.info("Concatenating output LHE files: expecting {}, found {}".format(cores, len(glob.glob("pwgevents*.lhe"))))
    LHE.merge("pwgevents*.lhe", powheg_LHE_output)
    subprocess.call("rm pwgevents-*.lhe 2> /dev/null", shell=True)
    # Unscale nEvents in case this is needed later on
    FileParser("powheg*.input").text_replace("numevts .*", "numevts {}".format(n_events_unscaled))
