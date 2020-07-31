#**************************************************************
#
# jopOptions file for reading ByteStream 
#
#==============================================================
# Input 
include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

svcMgr.EventSelector.Input = [ "/afs/cern.ch/atlas/maxidisk/d108/cranshaw/nightlies/extractedEvents.data" ]

from AthenaMP.AthenaMPFlags import jobproperties as jps
jps.AthenaMPFlags.Strategy='SharedQueue'

#from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 3 

MessageSvc.OutputLevel = INFO
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.MessageSvc.debugLimit =5000
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
