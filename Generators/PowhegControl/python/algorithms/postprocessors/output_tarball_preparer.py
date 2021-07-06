# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import subprocess

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("output tarball preparer")
def output_tarball_preparer(output_events_name, output_tarball_name):
    """! Tar events if LHE output is requested.

    @param output_events_name  LHE events file name.
    @param output_tarball_name Tar-ed LHE file name.

    @author James Robinson <james.robinson@cern.ch>
    """
    logger.info("Tar-ing output events into {}".format(output_tarball_name))
    for line in subprocess.check_output(["tar", "cvzf", output_tarball_name, output_events_name], stderr=subprocess.STDOUT).splitlines():
        logger.info(line)
