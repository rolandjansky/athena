# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl NLOEWDecorator
#  Powheg runcard decorator for NLO EW processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class NLOEWDecorator(object):

    # Define decorator name string
    name = "NLO EW"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("G_F", ATLASCommonParameters.G_F,       desc="Fermi constant",       parameter="gmu")
        self.decorated.add_parameter("lepaslight", -1,                       desc="(-1:Powheg default)")
        self.decorated.add_parameter("mass_H", ATLASCommonParameters.mass_H, desc="mass of Higgs in GeV", parameter="Hmass")
        self.decorated.add_parameter("mass_t", ATLASCommonParameters.mass_t, desc="t mass",               parameter="Tmass")
        self.decorated.add_parameter("new_damp", -1,                         desc="(-1:Powheg default)")
        self.decorated.add_parameter("no_ew", 0,                             desc="(0:with; 1:without) NLO EW corrections")
        self.decorated.add_parameter("no_strong", 0,                         desc="(0:with; 1:without) NLO QCD corrections")
        self.decorated.add_parameter("scheme", 1,                            desc="(0:Alpha(0); 1:G_mu) choice for EW NLO scheme")
        self.decorated.add_parameter("use_photos", 1,                        desc="(0:disabled; 1:enabled) use PHOTOS for photon radiation")
