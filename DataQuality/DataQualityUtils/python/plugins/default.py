# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------
# | Vassilis D. Kazazakis, default.py v1.1 |
# | Plugin for the QuerryDB.py application |
# ------------------------------------------

import sys
import getopt
import os

from PyCool import cool, coral

import DBInfo

userName = "ATLAS_COOL_READER"
password = "COOLRED4PRO"

connectString  = "oracle://" +  DBInfo.getWriteServer()
connectString += ";schema=" +   DBInfo.getSchema()
connectString += ";user=" +     userName
connectString += ";dbname=" +   DBInfo.getDbName()
connectString += ";password=" + password

folderName = DBInfo.getFolder()

# print connectString

### PUBLIC INTERFACE OF PLUGIN ###

PluginName = "DefaultPlugin"

def usage():
	print PluginName, "options:"
	print """
	  --about	print this help
	  -s/--start	Starting run:lumiBlock
	  -e/--end	Ending run:lumiBlock (defaults to the next lumiBlock)
	  -c/--channel	Name of channel
	"""

def QuerryDB(argDict): 
	startTime = argDict["runStart"][0] << 32
	startTime += argDict["runStart"][1]

	if argDict["runEnd"] == None:
		endTime = startTime + 1 # just till the next lumiBlock
	else:
		endTime = argDict["runEnd"][0] << 32
		endTime += argDict["runEnd"][1]

	if startTime >= endTime:
		print >>sys.stderr, "Starting timestamp must be LESS than ending timestamp."
		sys.exit(-1)

	#print argDict

	dbSvc = cool.DatabaseSvcFactory.databaseService()
	try:
		db = dbSvc.openDatabase(connectString)
	except:
		print >>sys.stderr, "Error opening database. Connection string:",connectString
		sys.exit(-1)
	folder = db.getFolder(folderName)

	try:
		channelNum = DBInfo.getChannelDict()[argDict["channelName"]]
	except KeyError:
		print >>sys.stderr, "Invalid channel name:", argDict["channelName"]
		sys.exit(-1)

	objIter = folder.browseObjects(startTime, endTime, cool.ChannelSelection(channelNum))

	resultList = []
	while(objIter.hasNext()):
		resultList.append(objIter.next().payload()["filename"])
	
	return resultList

def ParseArguments(arg):
	argDict = dict([])
	argDict["runStart"] = (0, 0)
	argDict["runEnd"] = None
	argDict["channelName"] = "default"

	try:
		optList, unparsed = getopt.getopt(arg, "s:e:c:", ["start=", "end=", "channel=", "about"])
	except getopt.GetoptError:
		usage()
		sys.exit(-1)

	for opt, arg in optList:
		if opt in ("-s", "--start"):
			startList = arg.split(":")
			if len(startList) == 1:
				startList.append("0")
			for i in range(2):
				if startList[i] == "":
					startList[i] = "0"
			argDict["runStart"] = ( int(startList[0]), int(startList[1]) )
		elif opt in ("-e", "--end"):
			endList = arg.split(":")
			if len(endList) == 1:
				endList.append("0")
			for i in range(2):
				if endList[i] == "":
					endList[i] = "0"
			argDict["runEnd"] = ( int(endList[0]), int(endList[1]) )
		elif opt in ("-c", "--channel"):
			argDict["channelName"] = arg
		elif opt in ("--about"):
			usage()
			sys.exit(0)

	return argDict
