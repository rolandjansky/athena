# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id$
#
# File: CaloClusterCorrection/python/scrubMagicTags.py
# Created: Aug 2010, sss
# Purpose: Remove magic tags from a cool database.
#
# Temporary(?) measure since the number of tags needed to do magic
# is getting out of control.
#


import CoolConvUtilities.AtlCoolLib as AtlCoolLib
import sys


def scrubFolder (db, folder):
    f = db.getFolder (folder)
    tags = f.listTags()
    for t in tags:
        if t.find('-ATLAS-') >= 0 or t.find('TagInfo/') >= 0:
            print 'nuke', t
            f.deleteTag(t)
        else:
            print 'keep', t
    return


def scrub (dbfile, folder):
    if dbfile.endswith ('.db'):
        connstring = "sqlite://;schema=%s;dbname=OFLP200" % dbfile
    else:
        connstring = dbfile

    db = AtlCoolLib.indirectOpen(connstring,False,False,True)
    if not db.existsFolderSet (folder):
        print "Can't find folder", folder
        sys.exit(1)
    fs = db.getFolderSet (folder)
    for f in fs.listFolders():
        scrubFolder (db, f)
    return


scrub (sys.argv[1], sys.argv[2])
