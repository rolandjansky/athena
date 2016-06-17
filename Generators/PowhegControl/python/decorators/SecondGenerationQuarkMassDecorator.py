# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl SecondGenerationQuarkMassDecorator
#  Powheg runcard decorator for second generation quark masses
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class SecondGenerationQuarkMassDecorator(object):

    # Define decorator name string
    name = "second generation quark mass"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated, prefix):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        if "lhfm" in prefix:
            self.decorated.add_parameter("mass_b", ATLASCommonParameters.mass_b, desc="b-quark mass in GeV",                              parameter="lhfm/bmass")
            self.decorated.add_parameter("mass_c", ATLASCommonParameters.mass_c, desc="c-quark mass in GeV",                              parameter="lhfm/cmass")
        if "lhe" in prefix:
            self.decorated.add_parameter("mass_b", ATLASCommonParameters.mass_b, desc="set b-quark mass in GeV for momentum reshuffling", parameter="bmass_lhe")
            self.decorated.add_parameter("mass_c", ATLASCommonParameters.mass_c, desc="set c-quark mass in GeV for momentum reshuffling", parameter="cmass_lhe")
