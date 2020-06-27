#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# CheckTagAssociation.py
# Andrei Artamonov 2012
#==================================================

from __future__ import print_function

import sys, getopt
tagusefile='/afs/cern.ch/user/a/atlcond/notify/AtlCoolMerge.taguse'


options, remainder = getopt.getopt(sys.argv[1:], 'h', ['help','folder=','globaltag=','instance=','localtag=','schema='])

# defaults
folder=''
#globaltag='CURRENT'
globaltag=''
localtag=''
instance = 'CONDBR2'
schema = "COOLOFL_TILE"
help = 0
for opt, arg in options:
    if opt in ('-h'):
        help = 1
    elif opt in ('--help'):
        help = 1
    elif opt in ('--folder'):
        folder = arg
    elif opt in ('--globaltag'):
        globaltag = arg
    elif opt in ('--localtag'):
        localtag = arg
    elif opt in ('--instance'):
        instance = arg
    elif opt in ('--schema'):
        schema = arg

if 'ONL01' in folder:
    print (' it does not work with singleversion folders')
    sys.exit(0)

if help:
    print (' This script prints aliases for CURRENT and NEXT global tags')
    print (' if no options are provided and also resolves global tag ')
    print (' to leaf tag if')
    print (' folder is provided. By default, if global tag is not specified,')
    print ('  globaltag=CURRENT')
    print ('   if local tag is specified, it makes inverse operation - ')
    print ('   shows all global tags linked to local tag')
    print ('  localtag=')
    print ('  default instance=CONDBR2')
    print (' usage:')
    print (' CheckTagAssociation.py --folder=foldername --globaltag=tagname --instance=instancename --localtag=leaftagname')
    print ("options globaltag and localtag are mutually exclusive")
    print ("    if both are specified, globaltag is ignored and ")
    print ("    association of localtag to all global tags is printed")
    print (" EXAMPLES: ")
    print ("CheckTagAssociation.py --folder=/TILE/OFL02/CALIB/CES --localtag=TileOfl02CalibCes-RUN2-HLT-UPD1-01")
    print ("CheckTagAssociation.py --folder=/TILE/OFL02/CALIB/CES")
    sys.exit()


from TileCalibBlobPython import TileCalibTools
from AtlCoolBKLib import resolveAlias
current = resolveAlias.getCurrent()
nexttag = resolveAlias.getNext()
#--------------------------------
from TileCalibBlobPython.TileCalibLogger import getLogger
log = getLogger("TileCalibTools")
import logging
log.setLevel(logging.WARNING)


#if instance == 'CONDBR2' :
print ("alias CURRENT = %s alias NEXT = %s" % (current, nexttag))

if folder == '':
    sys.exit()

#=================================================
connStr=schema+'/'+instance

#=== open the database
db = TileCalibTools.openDbConn(connStr, 'READONLY')

if localtag == "" :
    #=== resolve folder tag from global tag
    if globaltag != "":
        foldertag = TileCalibTools.getFolderTag(db, folder, globaltag)
        print ("global tag %s associated to leaf TAG %s" % (globaltag,foldertag))
    else:
        foldertag = TileCalibTools.getFolderTag(db, folder, current)
        print ("global tag %s associated to leaf TAG %s" % (current,foldertag))
        foldertag = TileCalibTools.getFolderTag(db, folder, next)
        print ("global tag %s associated to leaf TAG %s" % (next,foldertag))

else:
    rfolder=db.getFolderSet('/')
    taglist=rfolder.listTags()
    #print (taglist)
    for tag in taglist:
        try:
            foldertag = TileCalibTools.getFolderTag(db, folder, tag)
            if localtag == foldertag:
                print (" leaf tag %s linked to global tag %s " % (localtag, tag))
        except Exception:
            print (" ")
            #print (" WARNING !, existing global tag %s is not linked to local tag %s " % (tag,localtag))
