# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl LeptonMassDecorator
#  Powheg runcard decorator for lepton masses
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class LeptonMassDecorator(object):

    # Define decorator name string
    name = "lepton mass"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated, prefix):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        if "lhfm" in prefix:
            self.decorated.add_parameter("mass_e", ATLASCommonParameters.mass_e,     desc="mass of electron in GeV", parameter="lhfm/emass")
            self.decorated.add_parameter("mass_mu", ATLASCommonParameters.mass_mu,   desc="mass of mu in GeV",       parameter="lhfm/mumass")
            self.decorated.add_parameter("mass_tau", ATLASCommonParameters.mass_tau, desc="mass of tau in GeV",      parameter="lhfm/taumass")
        if "tdec" in prefix:
            self.decorated.add_parameter("mass_e", ATLASCommonParameters.mass_e,     desc="mass of electron in GeV", parameter="tdec/emass")
            self.decorated.add_parameter("mass_mu", ATLASCommonParameters.mass_mu,   desc="mass of mu in GeV",       parameter="tdec/mumass")
            self.decorated.add_parameter("mass_tau", ATLASCommonParameters.mass_tau, desc="mass of tau in GeV",      parameter="tdec/taumass")
