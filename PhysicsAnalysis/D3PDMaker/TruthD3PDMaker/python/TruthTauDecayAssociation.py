# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/TruthTauDecayAssociation.py
# @author Zach Marshall <zach.marshall@cern.ch>
# @date June 2013
# @brief Helper for setting up an association to tau decay products in the truth record

import TruthD3PDMaker
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation

def TruthTauDecayAssociation (parent,
                   prefix = '',
                   target = '',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up an association to tau decay products in the truth record"""

    if blockname is None:
        blockname = prefix + 'TauDecayMultiAssoc'

    return IndexMultiAssociation (parent,
                                  TruthD3PDMaker.TruthTauDecayAssociationTool,
                                  target,
                                  prefix,
                                  level,
                                  blockname,
                                  nrowName = '')

