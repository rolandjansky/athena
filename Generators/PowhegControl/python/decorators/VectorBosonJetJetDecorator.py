# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl VectorBosonJetJetDecorator
#  Powheg runcard decorator for vector boson plus two jets processes
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class VectorBosonJetJetDecorator(object):

    # Define decorator name string
    name = "Vector boson + two jets"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("ptborncut", -1,                       desc="(-1:Powheg default)")
        self.decorated.add_parameter("zfixedwidth", -1,                     desc="(-1:Powheg default)")
        self.decorated.add_parameter("zmass", ATLASCommonParameters.mass_Z, desc="Z mass in GeV")
