# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import Logging
from .external_base import ExternalBase
import collections

## Get handle to Athena logging
logger = Logging.logging.getLogger("PowhegControl")


class ExternalNNLOReweighter(ExternalBase):
    """! Class for running external NNLO reweighting process.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, *executable_path):
        """! Constructor.

        @param executable_path  path to appropriate PowhegBox executable.
        """
        super(ExternalNNLOReweighter, self).__init__("NNLO reweighter", *executable_path)

        # Add parameters used by MadSpin
        self.add_keyword("NNLO_reweighting_inputs")
        self.add_keyword("NNLO_output_weights")

    def needs_scheduling(self, process):
        """! Report whether the NNLO reweighting process should be scheduled.

        @param process PowhegBox process.
        """
        self.expose()
        # Check first whether any reweighting inputs were provided. DYNNLO does not require outputs no test for these.
        if len(self.NNLO_reweighting_inputs) == 0:
            return False
        # Check that NNLO reweighting arguments are dictionaries
        if not isinstance(self.NNLO_reweighting_inputs, collections.OrderedDict):
            if isinstance(self.NNLO_reweighting_inputs, dict):
                self.decorated.logger.warning("NNLO_reweighting_inputs has been provided as an old-style unordered dictionary.")
            else:
                self.decorated.logger.fatal("NNLO_reweighting_inputs does not appear to be a valid label => file dictionary!")
                raise ValueError("NNLO reweighting cannot be performed")
        # Check that NNLO output weight arguments are dictionaries
        if not isinstance(self.NNLO_output_weights, collections.OrderedDict):
            if isinstance(self.NNLO_output_weights, dict):
                self.decorated.logger.warning("NNLO_output_weights has been provided as an unordered dictionary! Weight numbering will be arbitrary")
            else:
                self.decorated.logger.fatal("NNLO_output_weights does not appear to be a valid ID string => calculation dictionary!")
                raise ValueError("NNLO reweighting cannot be performed")
        # Ensure that reweighting has been scheduled
        process.add_algorithm("reweighting")
        return True
