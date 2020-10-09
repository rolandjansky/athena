

## AthAlgSeq INFO Member list:
##    LVL1::TriggerTowerMaker/TriggerTowerMaker,
##    LVL1::CPMTowerMaker/CPMTowerMaker,
##    LVL1::JetElementMaker/JetElementMaker,
##    LVL1::EmTauTrigger/EmTauTrigger,
##    LVL1::JetTrigger/JetTrigger,
##    LVL1::EnergyTrigger/EnergyTrigger,
##    LVL1::ROD/ROD,
##    LVL1::CPCMMMaker/CPCMMMaker,
##    LVL1::JEPCMMMaker/JEPCMMMaker,
##    LVL1MUCTPI::L1Muctpi/L1Muctpi_on_RDO,
##    LVL1CTP::CTPSimulation/CTPSimulation,
##    ROIB::RoIBuilder/RoIBuilder,
##    JetGlobalEventSetup,
##    HLT::TrigSteer/TrigSteer_L2,
##    HLT::TrigSteer/TrigSteer_EF,
##    TrigDec::TrigDecisionMaker/TrigDecMaker,
##    TrigInDetTrackTruthMaker,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamAOD_FeaturesDrop,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamAOD_L2Drop,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamESD_FeaturesDrop,
##    HLT::StreamTrigNavSlimming/TrigSlimmerStreamESD_L2Drop


import logging
log = logging.getLogger("skeleton.BStoTRIGBS")
log.info("="*20 + " Start of BStoTRIGBS " + "="*20)

# import -------------------------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc
from AthenaCommon.Include import include
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.GlobalFlags import jobproperties,JobProperty
from InDetTrigRecExample.InDetTrigFlags import InDetTrigJobProperties
from InDetRecExample.InDetJobProperties import InDetJobProperties
from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
from InDetRecExample.InDetJobProperties import InDetFlags

from InDetTrigRecExample.InDetTrigJobProperties import *

from ByteStreamCnvSvc import ReadByteStream

from GaudiKernel.GaudiHandles import \
GaudiHandle,GaudiHandleArray,\
PublicToolHandle,PublicToolHandleArray,\
PrivateToolHandle,PrivateToolHandleArray,\
ServiceHandle

# runargs -------------------------------------------------------------------------------

globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)
globalflags.ConditionsTag.set_Value_and_Lock( runArgs.conditionsTag )

theApp.Dlls += [ "TrigT1RPCRecRoiSvc" ]
#theApp.ExtSvc += ["LVL1RPC::RPCRecRoiSvc"]


include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
topSequence = AlgSequence()
include( "LArTools/LArTools_jobOptions.py" )

from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_RDO
topSequence += L1Muctpi_on_RDO()
topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "/Run/L1MuCTPItoRoIBLocation"
topSequence.L1Muctpi_on_RDO.OverlapStrategyName = "NULL"

from TileRecAlgs.TileRecAlgsConf import TileRawChannelToHit                
theTileRawChannelToHit=TileRawChannelToHit()

# Take TileRawChannel and convert to TTL1
# Configure TileRawChannelToHit here
# Check TileRecAlgs_jobOptions.py for full configurability
theTileRawChannelToHit.TileRawChannelContainer="TileRawChannelCnt"
theTileRawChannelToHit.TileInfoName="TileInfo"
theTileRawChannelToHit.UseSamplFract = False

# sets output key  
theTileRawChannelToHit.TileHitContainer="TileHitVec"
topSequence += theTileRawChannelToHit

from TileSimAlgs.TileSimAlgsConf import TileHitVecToCnt
theTileHitVecToCnt=TileHitVecToCnt()
theTileHitVecToCnt.TileHitVectors=["TileHitVec"]
theTileHitVecToCnt.TileInfoName="TileInfo"
theTileHitVecToCnt.RndmEvtOverlay = True
#        
## sets output key  
theTileHitVecToCnt.TileHitContainer= "TileHitCnt"
from Digitization.DigitizationFlags import jobproperties
theTileHitVecToCnt.RndmSvc=jobproperties.Digitization.rndmSvc()
jobproperties.Digitization.rndmSeedList.addSeed("Tile_HitVecToCnt", 4789899, 989240512)

topSequence += theTileHitVecToCnt

from TileSimAlgs.TileSimAlgsConf import TileHitToTTL1
theTileHitToTTL1=TileHitToTTL1()
theTileHitToTTL1.TileHitContainer = "TileHitCnt"

from Digitization.DigitizationFlags import jobproperties
theTileHitToTTL1.RndmSvc=jobproperties.Digitization.rndmSvc()
jobproperties.Digitization.rndmSeedList.addSeed("theTile_HitToTTL1", 2789801, 189245514)

topSequence += theTileHitToTTL1
# end computation of TileRawChannel to TTL1

from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
muonByteStreamFlags.RpcDataType = 'atlas'
muonByteStreamFlags.MdtDataType = 'atlas'
muonByteStreamFlags.TgcDataType = 'atlas'

from MuonCnvExample.MuonCnvFlags import muonCnvFlags
if hasattr(muonCnvFlags,'RpcCablingMode'):
   muonCnvFlags.RpcCablingMode.set_Value_and_Lock('new')

from TrigmuFast.TrigmuFastConfig import TrigmuFastConfig
theTrigmuFast900GeV = TrigmuFastConfig('900GeV')
theTrigmuFast900GeV.DomuFastESD = False
theTrigmuFast900GeV.ROBmapperFile = "AtlasRoiROBmap.data"
theTrigmuFast900GeV.BackExtrapolator = PublicToolHandle('TrigMuonBackExtrapolator/MisalignedBackExtrapolator')

from TileRecAlgs.TileRecAlgsConf import TileRawChannelToTTL1
theTileRawChannelToTTL1 = TileRawChannelToTTL1()

theTileRawChannelToTTL1.TileRawChannelContainer = "TileRawChannelCnt"
theTileRawChannelToTTL1.TileTTL1Container = "TileTTL1Cnt"
theTileRawChannelToTTL1.maskBadChannels = True

# this causes "dupe ignored" error message
# from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
# topSequence += LVL1__TrigT1MBTS()

topSequence.DoBCM = True
topSequence.DoBPTX = True
topSequence.DoLUCID = True

from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys(rec.AutoConfiguration())


# setting input and output from runArgs
athenaCommonFlags.BSRDOInput.set_Value_and_Lock( runArgs.inputBS_OLDFile )
athenaCommonFlags.BSRDOOutput.set_Value_and_Lock( runArgs.outputBS_TRIG_OLDFile )

athenaCommonFlags.FilesInput =  runArgs.inputBS_OLDFile
globalflags.InputFormat = 'bytestream' 
globalflags.DataSource = 'geant4'
if hasattr(runArgs,"maxEvents"): athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
if hasattr(runArgs,"skipEvents"): athenaCommonFlags.SkipEvents.set_Value_and_Lock( runArgs.skipEvents )

print ('################################################')
print (runArgs.triggerConfig)

print ('########### DetFlags before ###############')
DetFlags.Print()
DetFlags.detdescr.all_setOn()
DetFlags.all_setOn()
include ("RecExCond/AllDet_detDescr.py")
print ('########### DetFlags after ###############')
DetFlags.Print()
include.block("RecExCond/RecExCommon_flags.py")
TriggerFlags.doID=True
TriggerFlags.doMuon=True
TriggerFlags.doCalo=True
TriggerFlags.doCalo=True
TriggerFlags.writeBS=True
#TriggerFlags.doID():
from InDetTrigRecExample.InDetTrigConfigConditions import \
         PixelConditionsSetup, SCT_ConditionsSetup, TRT_ConditionsSetup
#
PixelConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
PixelConditionsSetup.lock()
PixelConditionsSetup.createSvc()
SCT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
SCT_ConditionsSetup.lock()
SCT_ConditionsSetup.createTool()
TRT_ConditionsSetup.config(useDCS=False, onlineMode=True, prefix='')
TRT_ConditionsSetup.lock()
TRT_ConditionsSetup.createSvc()

include( "InDetRecExample/InDetRecCabling.py" )

DetFlags.detdescr.Calo_setOn()
from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.LoadElecCalib.set_Value_and_Lock(False)

from MuonCnvExample.MuonCnvFlags import muonCnvFlags
muonCnvFlags.RpcCablingMode.set_Value_and_Lock( "new" )
muonCnvFlags.MdtCablingMode.set_Value_and_Lock( "auto" )


TriggerFlags.triggerConfig.set_Value_and_Lock( runArgs.triggerConfig )

from TriggerJobOpts.TriggerGetter import TriggerGetter
triggerGetter = TriggerGetter()

#print ('Print topSequence ',topSequence)
#print ('topSequence type ',type(topSequence))

# Enable Aditors (i.e. AlgContextAuditor to get algorithm stack in CoreDumpSvc)
theApp.AuditAlgorithms = True



#-------------------------------------------------------------------------------

########################################
#
#   POST CONFIGURE RERUNNING OF LVL1
#
########################################

theApp.Dlls += [ "LArByteStream" ]
theApp.Dlls += [ "LArRawUtils", "LArROD" ]
theApp.Dlls += [ "TileByteStream" ]
theApp.Dlls += [ "TrigT1ResultByteStream" ]

# Not tested if necessary
if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
   from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
   svcMgr += ByteStreamAddressProviderSvc()

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "CTP_RDO/CTP_RDO", "MuCTPI_RIO/MUCTPI_RIO", "CTP_RIO/CTP_RIO", 
"PixelRDO_Container/PixelRDOs", 
"SCT_RDO_Container/SCT_RDOs", 
"TRT_RDO_Container/TRT_RDOs", 
"LArRawChannelContainer/LArRawChannels", 
"LArHitContainer/LArHitEMB",
"TileRawChannelContainer/TileRawChannelCnt", 
"RpcPadContainer/RPCPAD",
"MdtCsmContainer/MDTCSM",
"TgcRdoContainer/TGCRDO",
"CscRawDataContainer/CSCRDO",
"LArTTL1Container/LArTTL1EM",
"LArTTL1Container/LArTTL1HAD",
"TileTTL1Container/TileTTL1Cnt",
"TileTTL1Container/TileTTL1MBTS",
"MuCTPI_RDO/MUCTPI_RDO",
"CTP_RDO/CTP_RDO",
"MuCTPI_RIO/MUCTPI_RIO",
"CTP_RIO/CTP_RIO",
"LVL1::MbtsCTP/Run/L1MBTSAtoCTPLocation",
"LVL1::MbtsCTP/Run/L1MBTSCtoCTPLocation"
]

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ 
  "LArRawChannelContainer/LArRawChannels", 
  "TileRawChannelContainer/TileRawChannelCnt", 
  "MuCTPI_RDO/MUCTPI_RDO",        
  "CTP_RDO/CTP_RDO"
  ] 

#include( "LArL1Sim/LArL1Sim_G4_jobOptions.py" )
##protectedInclude( "TrigT1CaloSim/TrigT1CaloSimJobOptions_TTL1.py" )
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__TriggerTowerMaker
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMTowerMaker
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetElementMaker
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EmTauTrigger
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetTrigger
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyTrigger
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMMMaker
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEPCMMMaker
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__ROD
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Tester
#from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__DumpTriggerObjects

#
## switch neighbour initialisation off (default = true = perform init.)
##DetDescrCnvSvc.DoInitNeighbours = FALSE
#
## The following are properties for LArL1Sim:
##
## --------------------------------------------------------
## ............ declare the used top algo.
## --------------------------------------------------------
##
## --------------------------------------------------------
## ............ Algorithms Private Options
## --------------------------------------------------------
##
##
## .................. sub-detector hit containers
## 
##
#LArTTL1Maker = Algorithm( "LArTTL1Maker" )
#LArTTL1Maker.SubDetectors = "LAr_All"
## .................. set the TTL1 container name
##
#LArTTL1Maker.EmTTL1ContainerName = "LArTTL1EM"
#LArTTL1Maker.HadTTL1ContainerName = "LArTTL1HAD"

#-------------------------------------------------------------------------------

########################################
#
#  Define output
#
########################################

include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )
include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )

#athenaCommonFlags.BSRDOOutput = _o # Expected to be set elsewhere
from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")
StreamBSFileOutput.OutputFile = athenaCommonFlags.BSRDOOutput()
ServiceMgr.ByteStreamCnvSvc.IsSimulation = True
ServiceMgr.ByteStreamCnvSvc.InitCnvs += [
"LArRawChannelContainer", 
#"LArRawChannelCollection",
"TileRawChannelContainer", 
"RpcPadContainer",
"MdtCsmContainer",
"TgcRdoContainer",
"CscRawDataContainer",
"LArTTL1Container",
"TileTTL1Container",
"LArHitContainer",
"DataVector<LVL1::TriggerTower>#*",
"DataVector<LVL1::CPMTower>#*",
"DataVector<LVL1::JetElement>#*",
"DataVector<LVL1::CPMHits>#*",
"DataVector<LVL1::CPMRoI>#*",
"DataVector<LVL1::CMMCPHits>#*",
"DataVector<LVL1::CMMJetHits>#*",
"DataVector<LVL1::CMMEtSums>#*",
"DataVector<LVL1::JEMHits>#*",
"DataVector<LVL1::JEMRoI>#*",
"DataVector<LVL1::JEMEtSums>#*",
"LVL1::CMMRoI#*",
"DataVector<LVL1::CMMEtSums>",
"DataVector<LVL1::JEMRoI>",
"LVL1::MbtsCTP"
]

StreamBSFileOutput.ItemList += [ "LArTTL1Container#*" ]

# Taken from storegate dump
StreamBSFileOutput.ItemList += [ "CTP_RDO#*" ]
#StreamBSFileOutput.ItemList += [ "CTP_RIO#*" ] # No RIO
StreamBSFileOutput.ItemList += [ "CscDigitContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "CscRawDataContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "DataHeader#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "DataVector< LVL1::RecEnergyRoI >#*" ] # No converter
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMCPHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMEtSums>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CMMJetHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ] # Chosen by data address
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMTower>#CPMTowers" ] # Chosen by data address
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1::EmTauROI>#*" ] # No converter
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMEtSums>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMHits>#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JEMRoI>#JEMRoIs" ] # Chosen by data address
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JetElement>#*" ]
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1::JetROI>#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ] # Chosen by data address
#StreamBSFileOutput.ItemList += [ "DataVector<LVL1CTP::SlinkWord>#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "DataVector<TrigMissingET>#*" ] # No converter
StreamBSFileOutput.ItemList += [ "EventInfo#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "HLT::HLTResult#*" ]
#StreamBSFileOutput.ItemList += [ "HLT::TrigEDMSizes#*" ] # No converter
StreamBSFileOutput.ItemList += [ "InDet::PixelClusterContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "InDet::SCT_ClusterContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "InDet::SiClusterContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "InDet::TRT_DriftCircleContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "InDetBSErrContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "L1MUINT::MuCTPIToRoIBSLink#*" ] # No converter
StreamBSFileOutput.ItemList += [ "LArRawChannelContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "LVL1::CMMRoI#*" ]
StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#*" ]
#StreamBSFileOutput.ItemList += [ "LVL1::EmTauCTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::EnergyCTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::EnergyRoI#*" ] # No converter
StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#*" ]
StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#*" ]
#StreamBSFileOutput.ItemList += [ "LVL1::JetCTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::JetEtRoI#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1::MuCTPICTP#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1CTP::CTPSLink#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "LVL1CTP::Lvl1Result#*" ] # No converter
StreamBSFileOutput.ItemList += [ "MdtCsmContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "MdtDigitContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "MuCTPI_RDO#MUCTPI_RDO+" ]
#StreamBSFileOutput.ItemList += [ "MuCTPI_RIO#*" ] # No RIO
#StreamBSFileOutput.ItemList += [ "ROIB::RecRoIBResult#*" ] # No RIO
#StreamBSFileOutput.ItemList += [ "ROIB::RoIBResult#*" ]
StreamBSFileOutput.ItemList += [ "2771#*" ]
StreamBSFileOutput.ItemList += [ "RpcDigitContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "RpcPadContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "RpcSectorLogicContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "SpacePointContainer#*" ] # No converter
StreamBSFileOutput.ItemList += [ "TRT_BSErrContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "TRT_BSIdErrContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "TgcDigitContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "TgcRdoContainer#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "TileCellIDC#*" ] # Not relevant
StreamBSFileOutput.ItemList += [ "TileRawChannelContainer#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TrigDec::TrigDecision#*" ] # No converter
StreamBSFileOutput.ItemList += [ "TrigInDetTrackTruthMap#*" ] # Not relevant
#StreamBSFileOutput.ItemList += [ "TrigMissingETContainer#*" ] # No converter
#StreamBSFileOutput.ItemList += [ "TrigOperationalInfoCollection#*" ] # No converter
StreamBSFileOutput.ItemList += [ "std::vector<unsigned int>#*" ] # Not relevant

# LVL1
StreamBSFileOutput.ItemList = [ "ROIB::RoIBResult#*" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::TriggerTower>#TriggerTowers" ]
StreamBSFileOutput.ItemList += [ "LVL1::JEPBSCollection#JEPBSCollection" ]
StreamBSFileOutput.ItemList += [ "LVL1::JEPRoIBSCollection#JEPRoIBSCollection" ]
StreamBSFileOutput.ItemList += [ "LVL1::CPBSCollection#CPBSCollection" ]
StreamBSFileOutput.ItemList += [ "DataVector<LVL1::CPMRoI>#CPMRoIs" ]

# LVL2
StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]



StreamBSFileOutput.ItemList   += [ "CTP_RDO#CTP_RDO" ]
StreamBSFileOutput.ItemList   += [ "MuCTPI_RDO#MUCTPI_RDO+" ]


from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
svcMgr.ToolSvc+=LArRawDataContByteStreamToolConfig(InitializeForWriting=True,
                                                   stream = StreamBSFileOutput)


#-------------------------------------------------------------------------------

###################
### POST CONFIG ###
###################


svcMgr.RPCcablingServerSvc.Atlas = True
svcMgr.RPCcablingServerSvc.useMuonRPC_CablingSvc = True
svcMgr.TGCcablingServerSvc.forcedUse = True

for i in topSequence.getChildren():
   #print ("child type ",i.getName(), '\n')
   if "TrigSteer_L2" in i.getName():
      for j in i.getAllChildren():
        #print ("TrigSteer_L2 child ", j.getName(), "\n")
        if "muFast" in j.getType():
           #print ("muFast algo ",j.getName(), " ",j.getFullName(), "\n")
           j.DomuFastESD = False
           j.DetMaskCheck = False

theApp.EvtMax = runArgs.maxEvents

print ('######################## Storegate dump ########################')
svcMgr.StoreGateSvc.Dump = True
print ('######################## End of Storegate dump ########################')

print (svcMgr)

svcMgr.EventSelector.Input = athenaCommonFlags.BSRDOInput()

#svcMgr.ByteStreamCnvSvc.OutputLevel = DEBUG

#print ("Joerg All Services:")
#for svc in svcMgr.getChildren():
#    print ("       ",svc)
#    if "CnvSvc" in svc.name():
#        svc.OutputLevel = VERBOSE
#    if "InputSvc" in svc.name():
#        svc.OutputLevel = VERBOSE



svcMgr.ByteStreamEventStorageOutputSvc.OutputLevel = DEBUG
svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()

#if hasattr(svcMgr,"EventSelector"):
#    svcMgr.EventSelector.SkipEvents = runArgs.skipEvents

#svcMgr.MuonRPC_CablingSvc.RPCTriggerRoadsfromCool = True

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.TriggerTowerMaker.CellType = 2
topSequence.TriggerTowerMaker.TriggerTowerLocation = "TriggerTowers2"

#print ("Joerg Algorithms")
import AthenaCommon.Configurable as Configurable
Configurable.log.setLevel( INFO )
#print (topSequence.getChildren())
#print (topSequence)

svcMgr.ToolSvc.TrigDataAccess.OutputLevel = DEBUG
svcMgr.ToolSvc.TrigDataAccess.TileROD_Decoder.VerboseOutput = True
from TileByteStream.TileByteStreamConf import TileROD_Decoder
ToolSvc += TileROD_Decoder()
ToolSvc.TileROD_Decoder.VerboseOutput = True

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#print (svcMgr)
#print ("all services")
#for i in svcMgr.getAllChildren():
#   print (i," ", i.getName()," ",i.getType(),"\n")
#   if "RPCcablingServerSvc" in i.getType():
#      print ("RPCcablingServerSvc properties ", i.getProperties())
#   if "MuonRPC_CablingSvc" in i.getType():
#      print ("MuonRPC_CablingSvc properties ", i.getProperties())
#   if "ToolSvc" in i.getName():
#       for j in i.getAllChildren():
#          print ("tool service ",j.getName(),"\n")
#          if "RPCCablingDbTool" in j.getName():
#             print ("RPCCablingDbTool options ",j.getProperties())
#          if "TrigDataAccess" in j.getName():
#             print ("TrigDataAccess options ",j.getProperties())
#          if "LArRawDataContByteStreamTool" in j.getName():
#             j.OutputLevel = DEBUG
#             print ("LArRawDataContByteStreamTool options ",j.getProperties())



from TrigSteerMonitor.TrigSteerMonitorConf import TrigChainMoni
chainMoni = TrigChainMoni()
chainMoni.OutputLevel=VERBOSE
topSequence.TrigSteer_L2.MonTools += [chainMoni]
topSequence.TrigSteer_EF.MonTools += [chainMoni]

MessageSvc = svcMgr.MessageSvc
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"
MessageSvc.debugLimit = 100000

# commenting-out, this causes ERROR message "No variables to be monitored"
# topSequence.TrigSteer_L2.muFast_900GeV.AthenaMonTools = PrivateToolHandleArray(['TrigGenericMonitoringTool/TrigmuFastOnlineMonitoring', 'TrigGenericMonitoringTool/TrigmuFastCosmicMonitoring', 'TrigTimeHistTool/Time'])

defaultOptions = {}
defaultOptions['setModifiers']=[#Common modifiers for MC and data
                                'noLArCalibFolders',
                                'ForceMuonDataType',
                                'useNewRPCCabling',
                                'enableCostMonitoring',
                                'enableCoherentPS',
                                'useOracle',
                                'detailedErrorStreams',
                                'optimizeChainOrder',
                                'enableHotIDMasking',
                                'openThresholdRPCCabling',
                                #special streaming setup
                                'enable7BitL1TTStreaming']

modifierList=[]
import TriggerJobOpts.Modifiers
try:  # Temporary backwards compatible hack
    from TrigConfigSvc.TrigConfMetaData import TrigConfMetaData
    meta = TrigConfMetaData()
except:
    log.warning("TrigConfigSvc.TrigConfMetaData not available in this release")
    meta = None

from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
atlux = AtRanluxGenSvc()
atlux.Seeds = ['TriggerTowerMaker_Pedestal 8594832 5736213', 'TriggerTowerMaker_Digitization 8631309 4492432', 'CTPSimulation 1979283043 1924452189', 'ToolSvc.HLT::RandomScaler 1857585203 1857585203', 'TrigSteer_L2.OPIScaler 1857585203 1857585203', 'TrigSteer_L2.TrigCostExecL2.TrigCostScaler 1857585203 1857585203', 'TrigSteer_L2.TrigCostAthenaL2.TrigCostScaler 1857585203 1857585203', 'TrigSteer_EF.OPIScaler 1857585203 1857585203', 'TrigSteer_EF.TrigCostExecEF.TrigCostScaler 1857585203 1857585203', 'TrigSteer_EF.TrigCostAthenaEF.TrigCostScaler 1857585203 1857585203']
ServiceMgr += atlux

for i in svcMgr.getAllChildren():
   if "AtRanluxGenSvc" in i.getType():
      print ("Setting AtRanluxGenSvc","\n")
      i.Seeds = ['TriggerTowerMaker_Pedestal 8594832 5736213', 'TriggerTowerMaker_Digitization 8631309 4492432', 'CTPSimulation 1979283043 1924452189', 'ToolSvc.HLT::RandomScaler 1857585203 1857585203', 'TrigSteer_L2.OPIScaler 1857585203 1857585203', 'TrigSteer_L2.TrigCostExecL2.TrigCostScaler 1857585203 1857585203', 'TrigSteer_L2.TrigCostAthenaL2.TrigCostScaler 1857585203 1857585203', 'TrigSteer_EF.OPIScaler 1857585203 1857585203', 'TrigSteer_EF.TrigCostExecEF.TrigCostScaler 1857585203 1857585203', 'TrigSteer_EF.TrigCostAthenaEF.TrigCostScaler 1857585203 1857585203']

setModifiers = []
for mod in dir(TriggerJobOpts.Modifiers):
    if not hasattr(getattr(TriggerJobOpts.Modifiers,mod),'preSetup'): continue
    if mod in dir():  #allow turning on and off modifiers by variable of same name
        if globals()[mod]:
            if not mod in setModifiers:
                setModifiers+=[mod]
        else:
            if mod in setModifiers: setModifiers.remove(mod)
    if mod in setModifiers:
        modifierList+=[getattr(TriggerJobOpts.Modifiers,mod)()]
        if meta: meta.Modifiers += [mod]    # store in trig conf meta data
        setModifiers.remove(mod)

if setModifiers:
    log.error('Unknown modifier(s): '+str(setModifiers))

#-------------------------------------------------------------------------------

log.info("="*20 + " End of BStoTRIGBS " + "="*20)
