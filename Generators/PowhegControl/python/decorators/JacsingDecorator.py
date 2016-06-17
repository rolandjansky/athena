# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl JacsingDecorator
#  Powheg runcard decorator for jacsing parameter
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class JacsingDecorator(object):

    # Define decorator name string
    name = "jacsing"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("jacsing", -1, desc="(-1:use Powheg default)")
