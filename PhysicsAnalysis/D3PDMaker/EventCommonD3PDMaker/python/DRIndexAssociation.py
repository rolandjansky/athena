# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/DRIndexAssociation.py
# @author scott snyder <snyder@bnl.gov>
# @author Ryan Reece <ryan.reece@cern.ch>
# @date Dec, 2009
# @brief Helper for setting up a nearest-DR association, represented
#        by an index.
#


import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def DRIndexAssociation (parent,
                   type_name,
                   default_sgkey,
                   default_drcut,
                   prefix = '',
                   target = '',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up a nearest-DR association, represented by an index.
"""
    if blockname == None:
        blockname = prefix + 'DRIndexAssoc'

    def maker (name, prefix, object_name,
               sgkey = default_sgkey,
               getter = None,
               assoc = None,
               drcut = default_drcut):

        if not getter:
            getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                     (name + '_Getter',
                      TypeName = type_name,
                      SGKey = sgkey)
        if not assoc:
            assoc = EventCommonD3PDMaker.DRAssociationTool (name + 'Assoc',
                                                            Getter = getter,
                                                            DRCut = drcut)

        filler = D3PDMakerCoreComps.ContainedAssociationFillerTool \
                 (name,
                  Prefix = prefix,
                  Associator = assoc,
                  BlockName = blockname)
        indexer = D3PDMakerCoreComps.IndexFillerTool \
                  (name + 'Index',
                   Target = target)
        filler.BlockFillers += [indexer]
        filler += [indexer]
        return filler

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj


