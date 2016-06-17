# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl DileptonMassDecorator
#  Powheg runcard decorator for diboson interference
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class DileptonMassDecorator(object):

    # Define decorator name string
    name = "dilepton mass"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("mllmin", 4, desc="Minimum invariant mass of lepton pairs from decay")
