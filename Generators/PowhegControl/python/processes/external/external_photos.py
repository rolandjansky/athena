# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from .external_base import ExternalBase

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ExternalPHOTOS(ExternalBase):
    """! Class for running external PHOTOS process.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, *args):
        """! Constructor.

        @param process  PHOTOS process
        """
        super(ExternalPHOTOS, self).__init__("PHOTOS", *args)

        # Add parameters used by PHOTOS
        self.add_keyword("PHOTOS_enabled")

    def needs_scheduling(self, process):
        """! Report whether the PHOTOS process should be scheduled.

        @param process PowhegBox process.
        """
        # Check that PHOTOS is not disabled
        self.expose()
        if self.PHOTOS_enabled:
            logger.info("Running with PHOTOS enabled")
        else:
            logger.info("Running without PHOTOS")
            return False
        return True
