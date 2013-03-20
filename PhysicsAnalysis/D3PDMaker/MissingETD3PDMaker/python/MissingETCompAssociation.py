# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file MissingETD3PDMaker/python/MissingETAssociation.py
# @author Jet Goodson <jgoodson@cern.ch>
# @date September, 2010
# @brief Helper for setting up a MissingET association to a contained object.
#

import D3PDMakerCoreComps
import MissingETD3PDMaker
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def MissingETCompAssociation (parent,
                              type_name,
                              default_sgkey,
                              assocTool = MissingETD3PDMaker.MissingETCompAssociationTool,
                              prefix = '',
                              matched = '',
                              level = 0,
                              objectType = 'egamma',
                              blockname = None,
                              allowMissing = False,
                              *args, **kw):
   
    if blockname == None:
        blockname = prefix + 'METCompAssoc'

    def maker (name, prefix, object_name,
               sgkey = default_sgkey,
               getter = None,
               assoc = None):

        if not getter:
            getter = D3PDMakerCoreComps.SGObjGetterTool \
                     (name + '_Getter',
                      TypeName = type_name,
                      SGKey = sgkey)
        
        assoc = assocTool (name + 'Assoc',
                           Getter = getter,
                           ObjectType = objectType,
                           AllowMissing = allowMissing)
        
        return D3PDMakerCoreComps.ContainedAssociationFillerTool (name,
                                                                  Prefix = prefix,
                                                                  Associator = assoc,
                                                                  Matched = matched)

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj
