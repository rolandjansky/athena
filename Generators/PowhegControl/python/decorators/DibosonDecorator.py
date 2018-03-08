# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl DibosonDecorator
#  Powheg runcard decorator for diboson settings
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class DibosonDecorator(object):

    # Define decorator name string
    name = "diboson"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.allowed_decay_modes = []

        self.decorated.add_phantom("decay_mode", None, desc="Diboson decay mode")
        self.decorated.add_parameter("dronly", 0,      desc="(0:disabled; 1:enabled) include only double resonant diagrams")

    # Convert decay mode into list of Powheg parameter options
    def finalise(self):
        # Check that decay mode is in the allowed list
        if self.decorated.decay_mode not in self.decorated.allowed_decay_modes:
            self.decorated.logger.warning("Decay mode {} not recognised!".format(self.decorated.decay_mode))
        # Add entry for each decay mode
        for decay_mode in self.decorated.allowed_decay_modes:
            self.decorated.fix_parameter(decay_mode, [-1, 1][decay_mode == self.decorated.decay_mode], default="user-configured", desc="(1:enabled; -1:disabled) Diboson decay mode, ")
