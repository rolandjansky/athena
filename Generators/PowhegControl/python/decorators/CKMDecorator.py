# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl CKMDecorator
#  Powheg runcard decorator for CKM matrix
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters


class CKMDecorator(object):

    # Define decorator name string
    name = "CKM"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.add_parameter("CKM_Vud", ATLASCommonParameters.CKM_Vud, desc="CKM element: Vud")
        self.decorated.add_parameter("CKM_Vus", ATLASCommonParameters.CKM_Vus, desc="CKM element: Vus")
        self.decorated.add_parameter("CKM_Vub", ATLASCommonParameters.CKM_Vub, desc="CKM element: Vub")
        self.decorated.add_parameter("CKM_Vcd", ATLASCommonParameters.CKM_Vcd, desc="CKM element: Vcd")
        self.decorated.add_parameter("CKM_Vcs", ATLASCommonParameters.CKM_Vcs, desc="CKM element: Vcs")
        self.decorated.add_parameter("CKM_Vcb", ATLASCommonParameters.CKM_Vcb, desc="CKM element: Vcb")
        self.decorated.add_parameter("CKM_Vtd", ATLASCommonParameters.CKM_Vtd, desc="CKM element: Vtd")
        self.decorated.add_parameter("CKM_Vts", ATLASCommonParameters.CKM_Vts, desc="CKM element: Vts")
        self.decorated.add_parameter("CKM_Vtb", ATLASCommonParameters.CKM_Vtb, desc="CKM element: Vtb")
