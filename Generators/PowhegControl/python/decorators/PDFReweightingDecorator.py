# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @PowhegControl PDFReweightingDecorator
#  Powheg runcard decorator for PDF reweighting
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python


class PDFReweightingDecorator(object):

    # Define decorator name string
    name = "PDF reweighting"

    # Attach decorations to Powheg configurable
    def __init__(self, decorated):
        decorated.run_card_decorators.append(self)
        self.decorated = decorated

        self.decorated.fix_parameter("compute_rwgt", 0,   desc="(0:nominal; 1:compute reweighting factor)")
        self.decorated.fix_parameter("storeinfo_rwgt", 0, desc="(0:disabled; 1:enabled) enable new-style PDF information")
