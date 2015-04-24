# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# +---------------------------------------------+
# | Vassilis D. Kazazakis - DBInfo.py v1.0	|
# | Returns COOL DB connection information	|
# +---------------------------------------------+

# user & password information should be handled by each individual program

def getReadServer():
	return "ATLAS_COOLPROD"

def getWriteServer():
	#return "ATLAS_COOLWRITE"
  return ""

def getFolder():
	#return "/GLOBAL/DQM/HIST"
  return "/GLOBAL/DETSTATUS/DQMFOFL" #/GLOBAL/DETSTATUS/DQMFOFLH"

def getDbName():
	return "CONDBR2"
#	return "STRMP200"

def getSchema():
	#return "ATLAS_COOLOFL_GLOBAL"
  return "MyCOOL.db"

def getChannelDict():
	import DBInfo_Histo
	#########################################

	return DBInfo_Histo.getChannelDict()
