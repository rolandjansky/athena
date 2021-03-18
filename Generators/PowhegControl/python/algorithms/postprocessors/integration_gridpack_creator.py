# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from ...decorators import timed
import glob
import os
import subprocess

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


@timed("integration gridpack creator")
def integration_gridpack_creator(process):
    """! Create tarball containing integration grids for future use.

    Tar up integration grids

    @author James Robinson <james.robinson@cern.ch>
    """
    output_tarball_name = "integration_grids.tar.gz"
    file_names = sum([glob.glob(_f) for _f in process.integration_file_names], [])

    if file_names:
        logger.info("Tar-ing {} integration grids into {}".format(len(file_names), output_tarball_name))
        for line in subprocess.check_output(["tar", "cvzf", output_tarball_name] + file_names, stderr=subprocess.STDOUT).splitlines():
            logger.info(line)
        for file_name in file_names:
            try:
                os.remove(file_name)
            except OSError:
                logger.warning("... could not clean up {}".format(file_name))
