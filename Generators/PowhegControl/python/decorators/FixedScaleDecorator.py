# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl FixedScaleDecorator
#  Powheg runcard decorator for fixed scale
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class FixedScaleDecorator(object):

    # Define decorator name string
    name = "fixed scale"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("fixedscale", -1, desc="(-1:use Powheg default) use reference renormalisation and factorisation scales")
