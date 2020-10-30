# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/linkDummyTags.py
# Created: Aug 2010, sss
# Purpose: 
#

from __future__ import print_function


import CoolConvUtilities.AtlCoolLib as AtlCoolLib
import os


def get_htags (folder):
    connstring = 'COOLOFL_CALO/OFLP200'
    db = AtlCoolLib.indirectOpen (connstring, True, True, True)
    parent = os.path.dirname (folder)
    fs = db.getFolderSet (parent)
    tags = fs.listTags()
    subf = db.getFolder (folder)
    htags = []
    for t in tags:
        try:
            subf.resolveTag(t)
        except Exception:
            htags.append (t)

    tags = subf.listTags()
    dtag = None
    for t in tags:
        if t.endswith ('-dummy'):
            dtag = t
            break
    return (htags, dtag)


def link_tag (f, htag, dtag):
    try:
        f.findTagRelation(htag)
        f.deleteTagRelation(htag)
    except Exception:
        pass
    f.createTagRelation (htag, dtag)
    print ('Linked', htag)
    return


def linkDummy (dbfile, folder):
    htags, dtag = get_htags (folder)

    if dbfile.endswith ('.db'):
        connstring = "sqlite://;schema=%s;dbname=OFLP200" % dbfile
    else:
        connstring = dbfile
    db = AtlCoolLib.indirectOpen(connstring,False,False,True)
    f = db.getFolder (folder)
    for h in htags:
        link_tag (f, h, dtag)


linkDummy ('swcool.db', '/CALO/Ofl/CaloSwClusterCorrections/deadOTXps')
