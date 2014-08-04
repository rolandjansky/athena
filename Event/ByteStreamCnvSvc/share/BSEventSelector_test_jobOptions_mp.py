#**************************************************************
#
# jopOptions file for reading ByteStream 
#
#==============================================================
# Input 
import os 
os.environ['ATHENA_PROC_NUMBER']='2'

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

svcMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data" ]
svcMgr.ByteStreamInputSvc.FullFileName += [ "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data" ]
#svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data" ]
#svcMgr.ByteStreamInputSvc.MaxBadEvents = 1
#svcMgr.ByteStreamInputSvc.ValidateEvent = False

#from AthenaCommon.AppMgr import theApp
#theApp.EvtMax = 3 

MessageSvc.OutputLevel = INFO
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
