#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# simple utility to set channel names for a detector status folder
# Richard Hawkings, 16/2/07
# should not normally be needed, unless have to restart the DB

import sys
from CoolConvUtilities import AtlCoolLib
from DetectorStatus import DetStatusLib

if len(sys.argv)<2:
    print "usage",sys.argv[0],"<dbname> {<foldername>}"
    print "Adds channel names to detector status folder (default /GLOBAL/DETSTATUS/LBSUMM)"
    sys.exit(-1)

dbname=sys.argv[1]
foldername="/GLOBAL/DETSTATUS/LBSUMM"
if len(sys.argv)>2: foldername=sys.argv[2]

mynames=DetStatusLib.DetStatusNames()
print "Opening database",dbname
db=AtlCoolLib.forceOpen(dbname)
print "Access folder",foldername
folder=db.getFolder(foldername)
chanlist=mynames.allNums()
for ichan in chanlist:
    channame=mynames.name(ichan)
    print "Channel ",ichan," name ",channame,
    if (folder.existsChannel(ichan)):
        folder.setChannelName(ichan,channame)
        print "... Name set for existing channel"
    else:
        folder.createChannel(ichan,channame)
        print "... Created"
# check we have <named/> in the metadata
namestr='<named/>'
desc=folder.description()
if (desc.find(namestr)==-1):
    print "Folder description does not have %s - will add it" % namestr
    desc=desc+namestr
    print "Setting folder description to",desc
    try:
        folder.setDescription(desc)
        print "Folder description modified"
    except Exception,e:
        print "Could not set folder description:",e
print "All done"
