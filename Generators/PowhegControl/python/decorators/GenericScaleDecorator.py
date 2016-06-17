# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl GenericScaleDecorator
#  Powheg runcard decorator for universal NLO functionality
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class GenericScaleDecorator(object):

    # Define decorator name string
    name = "generic scale"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("btlscalect", -1,   desc="(-1:Powheg default; 0:disabled; 1:enabled) use the scales of the underlying-Born configuration for the subtraction terms")
        self.decorated.add_parameter("btlscalereal", -1, desc="(-1:Powheg default; 0:disabled; 1:enabled) compute scales that depend on the real kinematics")
