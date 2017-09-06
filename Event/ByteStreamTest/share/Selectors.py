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

theApp.EvtMax = 500
#svcMgr.ByteStreamInputSvc.FullFileName += [
#    "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data",
#    ]

svcMgr.EventSelector.Input += ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1"]
#ByteStreamInputSvc.DumpFlag = True

from ByteStreamTest.ByteStreamTestConf import EvenEventsSelectorTool
selector = EvenEventsSelectorTool("EvenEventsSelectorTool")

svcMgr.EventSelector.HelperTools += [selector]
svcMgr.EventSelector.OutputLevel = DEBUG

#MessageSvc.OutputLevel = DEBUG
svcMgr.EventSelector.OutputLevel = DEBUG
#svcMgr.ByteStreamInputSvc.OutputLevel = DEBUG
svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = DEBUG
