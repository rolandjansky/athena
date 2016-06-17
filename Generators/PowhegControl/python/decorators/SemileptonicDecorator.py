# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl SemileptonicDecorator
#  Powheg runcard decorator for semileptonic decays
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class SemileptonicDecorator(object):

    # Define decorator name string
    name = "semileptonic"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("semileptonic", -1, desc="(-1:use Powheg default) only accept semileptonic decays")
