# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: IndexAssociation.py 526858 2012-11-20 18:13:46Z ssnyder $
#
# @file D3PDMakerCoreComps/python/IndexAssociation.py
# @author Nicolas Berger <nberger@mail.cern.ch>, scott snyder <snyder@bnl.gov>
# @date Nov, 2009
# @brief Helper for setting up an association via an index.
#


import D3PDMakerCoreComps
from D3PDObject import D3PDObject


def IndexAssociation (parent,
                      assoctool,
                      target,
                      prefix = '',
                      level = 0,
                      blockname = None,
                      allowMissing = False,
                      containerIndexName = '',
                      **kw):

    """Helper for setting up an association via an index.

    parent: The parent D3PDobject or block.
    assoctool: The class for the (single) association tool.
    target: The label for the getter within which we look up the index.
    prefix: Prefix to add to the contained variables, if any.
    level: Level of detail for the block.
    blockname: Name for the block.
    allowMissing: If true, then it is not considered an error for the
                  target getter to fail to find the input objects.

    Extra arguments are passed to the association tool.
"""

    if blockname == None:
        blockname = assoctool.__name__

    def maker (name, prefix, object_name, **kw2):
        this_target = target
        if kw2.has_key ('target'):
            this_target = kw2['target']
            del kw2['target']

        if this_target == '':
            this_target = []
        elif type(this_target) != type([]):
            this_target = [ this_target ]

        assoc = assoctool (name + 'Assoc', **kw2)
        filler = D3PDMakerCoreComps.ContainedAssociationFillerTool \
                 (name, Prefix = prefix, Associator = assoc, Matched = '')
        indexer = D3PDMakerCoreComps.IndexFillerTool \
                  (name + 'Index', 
                   Targets = this_target,
                   ContainerIndexName = containerIndexName,
                   AllowMissing = allowMissing)
        filler.BlockFillers += [indexer]
        filler += [indexer]
        return filler

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj, **kw)
    return obj
