# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file TruthD3PDMaker/python/TruthTauDecayAssociation.py
# @author Zach Marshall <zach.marshall@cern.ch>
# @date June 2013
# @brief Helper for setting up an association to tau decay products in the truth record

#import D3PDMakerCoreComps
import TruthD3PDMaker
#from D3PDMakerCoreComps.D3PDObject import D3PDObject
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation

def TruthTauDecayAssociation (parent,
                   prefix = '',
                   target = '',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up an association to tau decay products in the truth record"""

    if blockname == None:
        blockname = prefix + 'TauDecayMultiAssoc'

    return IndexMultiAssociation (parent,
                                  TruthD3PDMaker.TruthTauDecayAssociationTool,
                                  target,
                                  prefix,
                                  level,
                                  blockname,
                                  nrowName = '')

