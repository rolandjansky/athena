# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#

import D3PDMakerCoreComps
import PhotonD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def PAUTruthPhotonAssociation (parent,
                         type_name,
                         default_sgkey,
                         prefix = '',
                         matched = '',
                         level = 0,
                         blockname = None,
                         *args, **kw):

    if blockname == None:
        blockname = prefix + 'PAUTruthPhotonAssoc'

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
            assoc = PhotonD3PDMaker.PAUTruthPhotonAssociationTool (name + 'Assoc',
                                                                   Getter = getter)
            
        return D3PDMakerCoreComps.ContainedAssociationFillerTool (name,
                                                                  Prefix = prefix,
                                                                  Associator = assoc,
                                                                  Matched = matched)

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj
