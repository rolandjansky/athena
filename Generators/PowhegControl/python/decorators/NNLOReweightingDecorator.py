# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl NNLOReweightingDecorator
#  Powheg runcard decorator for NNLO reweighting
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from collections import OrderedDict
from AthenaCommon import Logging

logger = Logging.logging.getLogger("PowhegControl")


class NNLOReweightingDecorator(object):

    # Define decorator name string
    name = "NNLO reweighting"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_phantom("NNLO_reweighting_inputs", OrderedDict(), desc="dictionary of labels => (pre-existing) HNNLO output")
        self.decorated.add_phantom("NNLO_output_weights", OrderedDict(),     desc="dictionary of weight ID string => weight calculation in NNLOPS format")

    # Check that NNLO reweighting arguments are dictionaries
    def finalise(self):
        # Check NNLO reweighting inputs
        if not isinstance(self.decorated.NNLO_reweighting_inputs, OrderedDict):
            if isinstance(self.decorated.NNLO_reweighting_inputs, dict):
                logger.warning("NNLO_reweighting_inputs has been provided as an old-style unordered dictionary.")
            else:
                logger.fatal("NNLO_reweighting_inputs does not appear to be a valid label => file dictionary!")

        # Check NNLO weight outputs
        if not isinstance(self.decorated.NNLO_output_weights, OrderedDict):
            if isinstance(self.decorated.NNLO_reweighting_inputs, dict):
                logger.warning("NNLO_output_weights has been provided as an unordered dictionary! Weight numbering will be arbitrary")
            else:
                logger.fatal("NNLO_output_weights does not appear to be a valid ID string => calculation dictionary!")
