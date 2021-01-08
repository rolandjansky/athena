# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file TruthD3PDMaker/python/TruthParticleChildAssociation.py
# @author Ryan Reece <ryan.reece@cern.ch>
# @date Mar, 2010
# @brief Helper for setting up an association for truth particle 
#   children by index.
#


import TruthD3PDMaker
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation


def TruthParticleChildAssociation (parent,
                   prefix = '',
                   target = '',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up an association for truth particle
children by index.
"""
    if blockname is None:
        blockname = prefix + 'TruthParticleChildAssociation'

    return IndexMultiAssociation (parent,
                                  TruthD3PDMaker.TruthParticleChildAssociationTool,
                                  target,
                                  prefix,
                                  level,
                                  blockname,
                                  nrowName = '')

