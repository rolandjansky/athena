#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# simple utility to add NConfig and NWorking columns to given folder
# Richard Hawkings, 28/7/09
# Please do not run this script

import sys
from PyCool import cool
from CoolConvUtilities import AtlCoolLib

if len(sys.argv)<2:
    print "usage",sys.argv[0],"<dbname> {<foldername>}"
    print "Adds number columns to detector status folder (default /GLOBAL/DETSTATUS/LBSUMM)"
    sys.exit(-1)

dbname=sys.argv[1]
foldername="/GLOBAL/DETSTATUS/LBSUMM"
if len(sys.argv)>2: foldername=sys.argv[2]

print "Opening database",dbname
try:
    db=AtlCoolLib.forceOpen(dbname)
    print "Access folder",foldername
    folder=db.getFolder(foldername)
except Exception,e:
    print "Could not get access to folder",foldername
    sys.exit(-1)
oldspec=folder.payloadSpecification()
if ('NConfig' not in oldspec.keys()):
    newspec=cool.RecordSpecification()
    newspec.extend("NConfig",cool.StorageType.Int32)
    newspec.extend("NWorking",cool.StorageType.Int32)
    newrec=cool.Record(newspec)
    # set default values
    newrec["NConfig"]=-1
    newrec["NWorking"]=-1
    try:
        folder.extendPayloadSpecification(newrec)
        print "Folder updated OK"
    except Exception,e:
        print "Exception",e,"when trying to extend payload"
else:
    print "Folder already updated to contain NConfig"
