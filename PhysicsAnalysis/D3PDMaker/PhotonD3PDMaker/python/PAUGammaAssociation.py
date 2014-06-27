# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file EventCommonD3PDMaker/python/DRAssociation.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Helper for setting up a nearest-DR association to a contained object.
#

import D3PDMakerCoreComps
import egammaD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def PAUGammaAssociation (parent,
                         type_name,
                         default_sgkey,
                         prefix = '',
                         matched = '',
                         level = 0,
                         blockname = None,
                         *args, **kw):
    """Helper for setting up an association from a photon to a PAU::gamma object.

    We get a source object (Analysis::Photon or Analysis::Electron)
    and a collection of target objects (PAU::gamma).
    We pick the target object that links to the source object.
    By default, the target collection is a DataVector from StoreGate.

    parent: The parent D3PDobject or block.
    type_name: The C++ type of the target object collection.
    default_sgkey: Default StoreGate key for the target collection.
    prefix: Prefix to add to the contained variables, if any.
    matched: If not null, a flag variable with this name will
             be created, set to true when the association succeeds.
    level: Level of detail for the block.
    blockname: Name for the block.

    Extra arguments are passed to the association tool.

    Here's an example.
    
    Usage is something like this:

       PhotonPAUAssoc = PAUGammaAssociation (PhotonD3PDObject,
                                             'PAU::gammaContainer',
                                             'PAUgammaCollection',
                                             prefix = 'PAU_',
                                             level = 2,
                                             blockname = 'PAU')
       PhotonPAUAssoc.defineBlock (2, 'PAUVariables',
                                   egammaD3PDMaker.PAUFillerTool)

    The PAUGammaAssociation call makes the association.  We'll look in SG
    for a PAU::gammaContainer called `PAUgammaCollection' and find the PAU::gamma
    object from there that links to the photon.  If the photon prefix is `ph_',
    then the PAUFillerTool will automatically create some variables, including a
    'ph_PAU_matched' variable which indicates whether or not a match was found.
"""
    if blockname == None:
        blockname = prefix + 'PAUGammaAssoc'

    def maker (name, prefix, object_name,
               sgkey = default_sgkey,
               getter = None,
               assoc = None):

        if not getter:
            getter = D3PDMakerCoreComps.SGDataVectorGetterTool \
                     (name + '_Getter',
                      TypeName = type_name,
                      SGKey = sgkey)
        if not assoc:
            assoc = egammaD3PDMaker.PAUGammaAssociationTool (name + 'Assoc',
                                                             Getter = getter)

        return D3PDMakerCoreComps.ContainedAssociationFillerTool (name,
                                                                  Prefix = prefix,
                                                                  Associator = assoc,
                                                                  Matched = matched)

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj
