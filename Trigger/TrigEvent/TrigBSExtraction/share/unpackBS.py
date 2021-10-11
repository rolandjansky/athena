################################################################################
##  unpackBS.py - simple jO to dump HLT result part of the BS,
##   put it in SG and optionally run EDM checker to dump it on screen
##
##  2009/09/09       Jiri.Masik@hep.manchester.ac.uk
################################################################################

run = 2    # set the run period to test
edmCheck = False
doStream = False
theApp.EvtMax = 10

BSFileList = {
  1 : ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1"],
  2 : ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1"]
}

################################################################################
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr as svcMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AlgSequence import AlgSequence

globalflags.DataSource = 'data'
globalflags.InputFormat = 'bytestream'
athenaCommonFlags.FilesInput = BSFileList[run]
job = AlgSequence()

from ByteStreamCnvSvc import ReadByteStream  # noqa: F401
svcMgr.EventSelector.Input = athenaCommonFlags.FilesInput()

if run==1:
  svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2",
                                                     "HLT::HLTResult/HLTResult_EF" ]
elif run==2:
  svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_HLT" ]

# Address examples for DataScouting HLT results
#svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/DataScouting_03" ]
#svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/DataScouting_05" ]

from TrigNavigation.TrigNavigationConfig import HLTNavigationOffline
from TrigBSExtraction.TrigBSExtractionConf import TrigBSExtraction
extr = TrigBSExtraction(OutputLevel=DEBUG)
job += extr
if run==1:
  extr.L2ResultKey = "HLTResult_L2"
  extr.HLTResultKey = "HLTResult_EF"
  extr.NavigationForL2 = HLTNavigationOffline("NavigationForL2")
  extr.NavigationForL2.ClassesFromPayloadIgnore = ["TrigPassBits#passbits"]  # ATR-23411
elif run==2:
  extr.L2ResultKey = ""
  extr.HLTResultKey = "HLTResult_HLT"

extr.Navigation = HLTNavigationOffline()

from TrigEDMConfig.TriggerEDM import getEDMLibraries, getPreregistrationList, getTPList
extr.Navigation.Dlls = getEDMLibraries()
extr.Navigation.ClassesToPreregister = getPreregistrationList(run)

# example for adding DataScouting results to decode
#extr.DSResultKeys += [ "DataScouting_03" ]
#extr.DSResultKeys += [ "DataScouting_05" ]

from TrigSerializeTP.TrigSerializeTPConf import TrigSerTPTool
ToolSvc += TrigSerTPTool('TrigSerTPTool')
ToolSvc.TrigSerTPTool.TPMap = getTPList(run)

from TrigSerializeCnvSvc.TrigSerializeCnvSvcConf import TrigSerializeConvHelper
TrigSerializeConvHelper = TrigSerializeConvHelper(doTP = True)
ToolSvc += TrigSerializeConvHelper

if run==1:  # for Run-1 we need the detector geometry and conditions
  globalflags.DatabaseInstance = 'COMP200'
  globalflags.ConditionsTag = "COMCOND-BLKPA-RUN1-07"
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())
  from AthenaCommon.DetFlags import DetFlags
  DetFlags.detdescr.all_setOn()
  DetFlags.detdescr.ALFA_setOff()
  include("RecExCond/AllDet_detDescr.py")

svcMgr.MessageSvc.verboseLimit = 0
svcMgr.MessageSvc.debugLimit = 0
svcMgr.MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
svcMgr.StoreGateSvc.Dump = False

if edmCheck:
  from TrigValAlgs.TrigValAlgsConfig import TrigEDMChecker
  job += TrigEDMChecker()

if doStream:
  from OutputStreamAthenaPool.CreateOutputStreams import createOutputStream
  StreamESD = createOutputStream('StreamESD', 'ESD.pool.root', True)
  # populate as needed for testing:
  StreamESD.ItemList += ['TrigRoiDescriptorCollection#HLT_TrigRoiDescriptorCollection']
  job += StreamESD

print(job)
