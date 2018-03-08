# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl WIDDecorator
#  Powheg runcard decorator for W-boson ID
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class WIDDecorator(object):

    # Define decorator name string
    name = "W ID"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("idvecbos", 24, desc="PDG ID for vector boson to be produced")
