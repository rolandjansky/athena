#**************************************************************
#
# jopOptions file for reading ByteStream 
#
#==============================================================
# Input 
from ByteStreamCnvSvc import ReadByteStream
svcMgr.EventSelector.Input = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1" ]

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 2 

from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")

MessageSvc.OutputLevel = INFO
svcMgr.EventSelector.OutputLevel = DEBUG
svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
