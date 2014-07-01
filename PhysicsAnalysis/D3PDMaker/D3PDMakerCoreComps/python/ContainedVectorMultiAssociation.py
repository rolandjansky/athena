# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: ContainedVectorMultiAssociation.py 526858 2012-11-20 18:13:46Z ssnyder $
#
# @file D3PDMakerCoreComps/python/ContainedVectorMultiAssociation.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Helper for setting up an association to a set of contained objects,
#        represented as vectors.
#


import D3PDMakerCoreComps
from D3PDObject import D3PDObject


def ContainedVectorMultiAssociation (parent,
                                     assoctool,
                                     prefix = '',
                                     level = 0,
                                     blockname = None,
                                     nrowName = 'n',
                                     **kw):
    """Helper for setting up an association to a set of contained objects,
represented as vectors.

    parent: The parent D3PDobject or block.
    assoctool: The class for the (single) association tool.
    prefix: Prefix to add to the contained variables, if any.
    level: Level of detail for the block.
    blockname: Name for the block.
    nrowName: Name of the variable for the count of objects.
              Omitted if empty.

    Extra arguments are passed to the association tool.
"""
    if blockname == None:
        blockname = assoctool.__name__

    def maker (name, prefix, object_name, **kw2):
        assoc = assoctool (name + 'Assoc', **kw2)
        return D3PDMakerCoreComps.ContainedVectorMultiAssociationFillerTool \
               (name,
                NrowName = nrowName,
                Prefix = prefix,
                Associator = assoc)

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj, **kw)
    return obj
