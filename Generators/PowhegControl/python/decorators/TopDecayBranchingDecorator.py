# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl TopDecayLeptonDecorator
#  Powheg runcard decorator for top decay parameters
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class TopDecayBranchingDecorator(object):

    # Define decorator name string
    name = "top decay branching"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated, prefix=""):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        if prefix == "tdec":
            self.decorated.add_parameter("elbranching", ATLASCommonParameters.W_lepton_BR, desc="W electronic branching fraction", parameter="tdec/elbranching")
        else:
            self.decorated.add_parameter("elbranching", ATLASCommonParameters.W_lepton_BR, desc="W electronic branching fraction")
