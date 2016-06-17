# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl RunningScalesDecorator
#  Powheg runcard decorator for running scales
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class RunningScalesDecorator(object):

    # Define decorator name string
    name = "running scales"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("runningscales", 0, desc="(0:use fixed factorisation/renormalisation scale; 1:use running scale)")
