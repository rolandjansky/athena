# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl RunningScaleDecorator
#  Powheg runcard decorator for running scale
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class RunningScaleDecorator(object):

    # Define decorator name string
    name = "running scale"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated, description=None):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated
        if description is None:
            description = "Choice for mu_R and mu_F in Bbar integration."

        self.decorated.add_parameter("runningscale", 1, desc=description)
