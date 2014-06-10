#**************************************************************
#
# jopOptions file for selecting  ByteStream events
#
#==============================================================
# Input 
#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
from ByteStreamCnvSvc import ReadByteStream

svcMgr = theApp.serviceMgr()

#ByteStreamInputSvc = svcMgr.ByteStreamInputSvc

theApp.EvtMax = 5000
#svcMgr.ByteStreamInputSvc.FullFileName += [
#    "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data",
#    ]

svcMgr.EventSelector.Input += ["/afs/cern.ch/atlas/maxidisk/d108/cranshaw/nightlies/extractedEvents.data"]
#svcMgr.ByteStreamInputSvc.FullFileName += ["/afs/cern.ch/atlas/maxidisk/d108/cranshaw/nightlies/extractedEvents.data"]

svcMgr.EventSelector.SkipEvents=-1

#MessageSvc.OutputLevel = DEBUG
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
