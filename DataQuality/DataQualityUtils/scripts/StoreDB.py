#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# +-----------------------------------------------------+
# | Vassilis D. Kazazakis, StoreDB.py v1.1		|
# | Stores ROOT file information in the COOL database	|
# +-----------------------------------------------------+

import sys
import getopt
import DataQualityUtils.DBInfo as DBInfo

from PyCool import cool, coral

# user & password information should be handled by each individual program
# if you have authentication file, this works 

home = os.environ.get('HOME')
os.environ['CORAL_AUTH_PATH'] = home+"/private"

##userName = "ATLAS_COOLOFL_GLOBAL_W"
##password = "*******" ### 

connectString  = "oracle://" +  DBInfo.getWriteServer()
connectString += ";schema=" +   DBInfo.getSchema()
#connectString += ";user=" +     userName
connectString += ";dbname=" +   DBInfo.getDbName()
#connectString += ";password=" + password

folderName = DBInfo.getFolder()

def usage():
	print "Usage:", sys.argv[0], "[options]"
	print """
	-f/--filename 	ROOT filename to enter to the database
	-g/--guid	GUID of the file
	-s/--start	Starting run:lumiBlock number
	-e/--end	Ending run:lumiBlock (defaults to valid until next insertion) 
	-c/--channel	Channel name
	-l/--list	List all available channels
	"""

def addToDatabase(filename, guid, channelName, runStart, runEnd = None):
	dbSvc = cool.DatabaseSvcFactory.databaseService()

	try:
		channel_id = DBInfo.getChannelDict()[channelName]
	except KeyError:
		print >>sys.stderr, "Channel", channelName, "does not exist."
		sys.exit(-1)

	if runEnd == None:
		timeEnd = cool.ValidityKeyMax
	else:
		timeEnd = runEnd[0]  << 32
		timeEnd += runEnd[1]
	timeStart = runStart[0] << 32	## Since the 32 low bits are reserved for LumiBlocks...
	timeStart += runStart[1]	## add lumiblock
	
	if timeStart >= timeEnd:
		print >>sys.stderr, "Starting timestamp must be LESS than ending timestamp."
		sys.exit(-1)

	try:
		db = dbSvc.openDatabase( connectString, False )
	except:
		print >>sys.stderr, "Error opening database."
		sys.exit(-1)
	
	if db.existsFolder(folderName):
		folder = db.getFolder(folderName)
		spec = folder.payloadSpecification()
	else:
		print >>sys.stderr, "Error: Folder", folderName,"does not exist in database."
		sys.exit(-1)

	data = cool.Record(spec)
	data["filename"] = filename
	data["guid"] = guid
	folder.storeObject( timeStart, timeEnd, data, channel_id)

if __name__ == "__main__":
	
	try:
		optlist, unparsed = getopt.getopt(sys.argv[1:], 
							"f:s:e:c:lg:",
							["filename=", "start=", "end=", "channel=", "list", "guid="])
	except getopt.GetoptError:
		print >>sys.stderr, "Error parsing arguments."
		usage()
		sys.exit(-1)

	filename = None
	channelName = "default"
	guid = 0
	runStart = (0, 0)
	runEnd = None

	for opt, arg in optlist:
		if opt in ("-f", "--filename"):
			filename = arg
		elif opt in ("-g", "--guid"):
			guid = int(arg)
		elif opt in ("-s", "--start"):
			startList = arg.split(":")
			if len(startList) == 1:
				startList.append("0")
			for i in range(2):
				if startList[i] == "":
					startList[i] = "0"
			runStart = ( int(startList[0]), int(startList[1]) )
		elif opt in ("-e", "--end"):
			endList = arg.split(":")
			if len(endList) == 1:
				endList.append("0")
			for i in range(2):
				if endList[i] == "":
					endList[i] = "0"
			runEnd = ( int(endList[0]), int(endList[1]) )
		elif opt in ("-c", "--channel"):
			channelName = arg
		elif opt in ("-l", "--list"):
			print "Available channels:"
			for key in DBInfo.getChannelDict().keys():
				print " ",key
			sys.exit(0)

	if filename == None:
		print >>sys.stderr, "Must define ROOT file."
		usage()
		sys.exit(-1)

	#guid = generateGUID(filename)

	addToDatabase(filename, guid, channelName, runStart, runEnd)
