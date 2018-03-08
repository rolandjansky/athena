# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl TopDecayModeDecorator
#  Powheg runcard decorator for top decay parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from AthenaCommon import Logging

logger = Logging.logging.getLogger("PowhegControl")

class TopDecayModeDecorator(object):

    # Define decorator name string
    name = "top decay mode"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("topdecaymode", "00000", desc="string of 5 digits indicating the maximum number of (e,mu,tau,u,c) allowed in decays")

    # Check that parameters are correctly specified
    def finalise(self):
        # Set up parallelisation parameters if in multicore mode
        if not isinstance(self.decorated.topdecaymode, str):
            self.decorated.topdecaymode = str(self.decorated.topdecaymode)
            logger.warning("The 'topdecaymode' parameter was not provided as a string. Please check that its value of {} is what you were expecting!".format(self.decorated.topdecaymode))
