# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Josiah Couch
# 2 July 2014 
#
# Updated 14 July

def QueryDB(runNum,versionTag):
	from PyCool import cool, coral
	dbSvc = cool.DatabaseSvcFactory.databaseService()

	dbFile = "/data/jcouch/testFTK.db"
	dbName = "MYCOOLDB"
	
	connectString  = "sqlite://;schema="+dbFile+";dbname="+dbName
	
	folderName = "/FTK"
	db = dbSvc.openDatabase(connectString)
	
	folder = db.getFolder(folderName)
	
	channelID = folder.channelId("FTKinputDir")
	channel = cool.ChannelSelection(channelID)
	
	RunNumber = 0

	ls = []
	objIt = folder.browseObjects(runNum,runNum+1,channel,versionTag)	
	for obj in objIt:
    		ls.append(obj.payload()["Dir"])
	return ls[0]

def main():
	import sys
	print QueryDB(sys.argv[1],sys.argv[2])
	return 0
	

if __name__ == '__main__':
	main()
	
