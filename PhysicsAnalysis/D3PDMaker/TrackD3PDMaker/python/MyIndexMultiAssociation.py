# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file D3PDMakerCoreComps/python/IndexMultiAssociation.py
# @author scott snyder <snyder@bnl.gov>
# @author Ryan Reece <ryan.reece@cern.ch>
# @date Dec 2009
# @brief Helper for setting up an association via an index.
#

import TrackD3PDMaker
import D3PDMakerCoreComps
from D3PDMakerCoreComps.D3PDObject import D3PDObject


def MyIndexMultiAssociation (parent,
                             assoctool,
                             target,
                             prefix = '',
                             level = 0,
                             blockname = None,
                             *args, **kw):

    if blockname == None:
        blockname = assoctool.__name__

    def maker (name, prefix, *args_inner, **kw_inner):
        kw2 = kw.copy()
        kw2.update (kw_inner)
        assoc = assoctool (name + 'Assoc', *(args + args_inner), **kw2)
        return TrackD3PDMaker.MyIndexMultiAssociationFillerTool \
               (name, Target = target, Prefix = prefix, Associator = assoc)

    obj = D3PDObject (maker, prefix)
    parent.defineBlock (level, blockname, obj)
    return obj
