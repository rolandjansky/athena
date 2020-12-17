# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/TruthLeptonParentAssociation.py
# @author Zach Marshall <zach.marshall@cern.ch>
# @date June 2013
# @brief Helper for setting up an association to lepton parents (not direct) in the truth record

import TruthD3PDMaker
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation

def TruthLeptonParentAssociation (parent,
                   prefix = '',
                   target = '',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up an association to lepton parents in the truth record"""

    if blockname is None:
        blockname = prefix + 'LeptonParentMultiAssoc'

    return IndexMultiAssociation (parent,
                                  TruthD3PDMaker.TruthLeptonParentAssociationTool,
                                  target,
                                  prefix,
                                  level,
                                  blockname,
                                  nrowName = '')

