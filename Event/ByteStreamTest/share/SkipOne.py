#**************************************************************
#
# jopOptions file for selecting  ByteStream events
#
#==============================================================
# Input 
from ByteStreamCnvSvc import ReadByteStream

theApp.EvtMax = 500

svcMgr.EventSelector.Input += ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

svcMgr.EventSelector.SkipEvents = 1
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
