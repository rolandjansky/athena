# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/DRAssociation.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Helper for setting up a nearest-DR association to a contained object.
#


import D3PDMakerCoreComps
import EventCommonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def DRAssociation (parent,
                   type_name,
                   default_sgkey,
                   default_drcut,
                   prefix = '',
                   matched = 'matched',
                   level = 0,
                   blockname = None,
                   *args, **kw):
    """Helper for setting up a nearest-DR association to a contained object.

    We get an source object and a collection of target objects.
    We pick the target object that's closest in DR to the source object.
    By default, the target collection is a DataVector from StoreGate.

    parent: The parent D3PDobject or block.
    type_name: The C++ type of the target object collection.
    default_sgkey: Default StoreGate key for the target collection.
    default_drcut: Default DR cut.  If no target objects are within
                   this DR, the association will fail.
    prefix: Prefix to add to the contained variables, if any.
    matched: If not null, a flag variable with this name will
             be created, set to true when the association succeeds.
    level: Level of detail for the block.
    blockname: Name for the block.

    Extra arguments are passed to the association tool.

    Here's an example.  Suppose we have an electron object, and we
    want to record the 4-vector of a matching jet.

    Usage is something like this:

       ElectronObject = ...

       EleJetAssoc = DRAssociation (ElectronObject,
                                   'JetCollection',
                                   'Cone4H1TowerJets',
                                   0.2,
                                   'jet_',
                                   level = 2,
                                   blockname = 'JetMatch')
       EleJetAssoc.defineBlock (2, 'JetKinematics',
                                EventCommonD3PDMaker.FourMomFillerTool)

    The DRAssociation call makes the association.  We'll look in SG
    for a JetCollection called `Cone4H1TowerJets' and find the jet from
    there that's closest in DR to the electron (but not farther away
    than 0.2).  If the electron prefix is `el_', then we'll automatically
    create the variables `el_jet_matched' and `el_jet_dr'.  The second
    call then adds the jet kinematic variables: `el_jet_pt', etc.
    Additional blocks can be added if desired.
"""
    if blockname == None:
        blockname = prefix + 'DRAssoc'

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

        return D3PDMakerCoreComps.ContainedAssociationFillerTool (name,
                                                         Prefix = prefix,
                                                         Associator = assoc,
                                                         Matched = matched)

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj
