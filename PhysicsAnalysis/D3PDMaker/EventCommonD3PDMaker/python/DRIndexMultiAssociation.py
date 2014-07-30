# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/DRIndexMultiAssociation.py
# @author scott snyder <snyder@bnl.gov>
# @author Ryan Reece <ryan.reece@cern.ch>
# @date Dec, 2009
# @brief Helper for setting up an association to objects within a DR cone,
#        represented by indices.
#


import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def DRIndexMultiAssociation (parent,
                   type_name,
                   default_sgkey,
                   default_drcut,
                   prefix = '',
                   target = '',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up an association to objects within a DR cone,
    represented by indices.
"""


    if blockname == None:
        blockname = prefix + 'DRIndexMultiAssoc'

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
            assoc = EventCommonD3PDMaker.DRConeAssociationTool (name + 'Assoc',
                                                            Getter = getter,
                                                            DRCut = drcut)

        filler = D3PDMakerCoreComps.ContainedVectorMultiAssociationFillerTool \
                 (name,
                  Prefix = prefix,
                  Associator = assoc,
                  NrowName = '')
        indexer = D3PDMakerCoreComps.IndexFillerTool \
                  (name + 'Index', 
                   Target = target)
        filler.BlockFillers += [indexer]
        filler += [indexer]
        return filler
    

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj
