#**************************************************************
#
# jopOptions file for selecting  ByteStream events
#
#==============================================================
# Input 
#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
from ByteStreamCnvSvc import ReadByteStream

svcMgr = theApp.serviceMgr()

theApp.EvtMax = 5000

#svcMgr.ByteStreamInputSvc.FullFileName += ["/afs/cern.ch/atlas/maxidisk/d108/cranshaw/nightlies/extractedEvents.data"]
svcMgr.EventSelector.Input += ["/afs/cern.ch/atlas/maxidisk/d108/cranshaw/nightlies/extractedEvents.data"]
svcMgr.ByteStreamInputSvc.MaxBadEvents = 10

#MessageSvc.OutputLevel = DEBUG
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
