# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl TopPropertiesDecorator
#  Powheg runcard decorator for top properties
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class TopPropertiesDecorator(object):

    # Define decorator name string
    name = "top properties"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("mass_t", ATLASCommonParameters.mass_t,   desc="mass of top quark in GeV", parameter="tmass")
        self.decorated.add_parameter("mass_t", ATLASCommonParameters.mass_t,   desc="mass of top quark in GeV", parameter="topmass")
        self.decorated.add_parameter("width_t", ATLASCommonParameters.width_t, desc="top width",                parameter="topwidth")
        self.decorated.add_parameter("ttype", 1,                               desc="(1:t; -1:tbar)")
