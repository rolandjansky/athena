# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl RunningWidthDecorator
#  Powheg runcard decorator for running width
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class RunningWidthDecorator(object):

    # Define decorator name string
    name = "running width"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("runningwidth", 0,  desc="(0:disabled; 1:enabled) use running width")
        self.decorated.add_parameter("running_width", 0, desc="(0:disabled; 1:enabled) use running width")
