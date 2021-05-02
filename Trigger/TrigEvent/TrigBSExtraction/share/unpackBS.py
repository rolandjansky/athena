################################################################################
##  unpackBS.py - simple jO to dump HLT result part of the BS,
##   put it in SG and optionally run EDM checker to dump it on screen
##
##  2009/09/09       Jiri.Masik@hep.manchester.ac.uk
################################################################################

run = 2    # set the run period to test
edmCheck = False
theApp.EvtMax = 10

BSFileList = {
  1 : ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1"],
  2 : ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1"]
}

################################################################################
from ByteStreamCnvSvc import ReadByteStream
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
svcMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")

svcMgr.EventSelector.Input = BSFileList[run]

from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
svcMgr += ROBDataProviderSvc()

from ByteStreamCnvSvc.ByteStreamCnvSvcConf import EventSelectorByteStream
svcMgr += EventSelectorByteStream("EventSelector")
theApp.EvtSel = "EventSelector"

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()

# Properties 
EventSelector = svcMgr.EventSelector
EventSelector.ByteStreamInputSvc     = "ByteStreamInputSvc"
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()

if run==1:
  svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2",
                                                     "HLT::HLTResult/HLTResult_EF" ]
elif run==2:
  svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_HLT" ]

# Address examples for DataScouting HLT results
#svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/DataScouting_03" ]
#svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/DataScouting_05" ]

from SGComps.SGCompsConf import ProxyProviderSvc
svcMgr += ProxyProviderSvc()
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]


# TrigBSExtraction
from TrigBSExtraction.TrigBSExtractionConf import TrigBSExtraction
extr = TrigBSExtraction(OutputLevel=DEBUG)
if run==1:
  extr.L2ResultKey = "HLTResult_L2"
  extr.EFResultKey = "HLTResult_EF"
  extr.HLTResultKey = ""
elif run==2:
  extr.L2ResultKey = ""
  extr.EFResultKey = ""
  extr.HLTResultKey = "HLTResult_HLT"

from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
extr.Navigation = HLTNavigationOffline()

from TrigEDMConfig.TriggerEDM import getEDMLibraries, getPreregistrationList, getTPList
extr.Navigation.Dlls = getEDMLibraries()
extr.Navigation.ClassesToPreregister = getPreregistrationList(run)

# example for adding DataScouting results to decode
#extr.DSResultKeys += [ "DataScouting_03" ]
#extr.DSResultKeys += [ "DataScouting_05" ]

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job += extr

from AthenaCommon.AppMgr import ToolSvc
from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
ToolSvc += TrigSerTPTool('TrigSerTPTool')
ToolSvc.TrigSerTPTool.TPMap = getTPList(run)

svcMgr.StoreGateSvc.Dump = False

if edmCheck:
  from TrigValAlgs.TrigValAlgsConfig import TrigEDMChecker
  job += TrigEDMChecker()

print(job)
