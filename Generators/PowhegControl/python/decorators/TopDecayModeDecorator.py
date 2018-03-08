# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl TopDecayModeDecorator
#  Powheg runcard decorator for top decay parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class TopDecayModeDecorator(object):

    # Define decorator name string
    name = "top decay mode"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("topdecaymode", 0, desc="5 digits indicating the maximum number of (e,mu,tau,u,c) allowed in decays")
