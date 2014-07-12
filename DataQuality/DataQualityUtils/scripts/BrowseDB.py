#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# +-----------------------------------------------------+
# | Vassilis D. Kazazakis - BrowseDB.py v1.0            |
# |  Browses the COOL database and prints all elements  |
# |  For testing purposes only                          |
# +-----------------------------------------------------+

import sys

from PyCool import cool, coral

import DataQualityUtils.DBInfo as DBInfo

# user & password information should be handled by each individual program
# if you have authentication file, this works

home = os.environ.get('HOME')
os.environ['CORAL_AUTH_PATH'] = home+"/private"

#userName = "ATLAS_COOL_READER"
#password = "*******" no need for a pwd here 

connectString  = "oracle://" +  DBInfo.getWriteServer()
connectString += ";schema=" +   DBInfo.getSchema()
#connectString += ";user=" +     userName
connectString += ";dbname=" +   DBInfo.getDbName()
#connectString += ";password=" + password

folderName = DBInfo.getFolder()

dbSvc = cool.DatabaseSvcFactory.databaseService()

try:
        db = dbSvc.openDatabase( connectString )
except:
        print >>sys.stderr, "Database does not exist"
        sys.exit(-1)

folder = db.getFolder( folderName)
metadata = DBInfo.getChannelDict()
for key in metadata:
	channel = metadata[key]
	objIter = folder.browseObjects(0, cool.ValidityKeyMax, cool.ChannelSelection(channel))

	print "Folder:", folderName
	print "Channel:", channel, "Stream:", key

	while(objIter.hasNext()):
		objPayload = objIter.next().payload()
		print " ", objPayload["filename"], "GUID:", objPayload["guid"]

	print
