#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.Resilience import treatException
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax = 2000
jobproperties.AthenaCommonFlags.SkipEvents = 0
jobproperties.AthenaCommonFlags.PoolHitsInput = [ "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/SCT_SingleMuons/mc10_7TeV.107370.singlepart_mu_pt6_50_GeV.simul.HITS.e628.pool.root" ]
jobproperties.AthenaCommonFlags.PoolRDOOutput = "SCT_DigitizationRTTevents.root"

###Nasty Hacks
from AthenaCommon.AppMgr import ServiceMgr
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr += PoolSvc()
ServiceMgr.PoolSvc.AttemptCatalogPatch = True;
###End Nasty Hacks

#--------------------------------------------------------------
# Global flags
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-16-00-00'

#--------------------------------------------------------------
# PerfMon Setup
#--------------------------------------------------------------
#from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doMonitoring = True
#jobproperties.PerfMonFlags.OutputFile = "perfmon.root"

#--------------------------------------------------------------
# Detector flags
#--------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags

#Turn off everything except the SCT and Truth Information
DetFlags.ID_setOff()
DetFlags.SCT_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.writeRDOPool.SCT_setOff()##
#DetFlags.writeRDOPool.Truth_setOff()
DetFlags.Print()

#--------------------------------------------------------------
# inlude Digitization
#--------------------------------------------------------------
include ("Digitization/Digitization.py")

###Nasty Hacks
DetFlags.makeRIO.SCT_setOn()
###End Nasty Hacks


#--------------------------------------------------------------
# Setup SCT Digitization
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

theSCT_Digitization = topSequence.SCT_Digitization
theSCT_Digitization.OutputLevel = ERROR #INFO#
#theSCT_Digitization.OnlyElementsWithHits = TRUE
#theSCT_Digitization.TBBadChannels = FALSE
#theSCT_Digitization.CreateNoiseSDO = TRUE
#theSCT_Digitization.UseComTime = FALSE
#theSCT_Digitization.CosmicsRun = FALSE
#theSCT_Digitization.BarrelOnly = FALSE
print      theSCT_Digitization

#--------------------------------------------------------------
# Setup SCT FrontEnd AlgTool
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc
#The rest of this section can be commented out with no ill effects
from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
theSCT_FrontEnd = SCT_FrontEnd("SCT_FrontEnd")
try:
   theSCT_FrontEnd.NoiseOn = True
except:
   treatException("Could not find theSCT_FrontEnd.NoiseOn property. Skipping...")
# DataCompressionMode: 1 is level mode(default), 2 is edge mode
try:
   theSCT_FrontEnd.DataCompressionMode = 1
except:
   treatException("Could not find theSCT_FrontEnd.DataCompressionMode property. Skipping...")
ToolSvc += theSCT_FrontEnd
print      theSCT_FrontEnd

#--------------------------------------------------------------
# Setup Output File For Histograms
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr+=THistSvc()
ServiceMgr.THistSvc.Output = ["SCT_DigiRTT DATAFILE='SCT_DigitizationRTTHists.root' opt='RECREATE'"];

DetFlags.Print()

#--------------------------------------------------------------
# include SCT Clusterization
#--------------------------------------------------------------
if DetFlags.makeRIO.SCT_on():
   from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
   InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
                                                   PixelCalibSvc        = None,
                                                   PixelOfflineCalibSvc = None,
                                                   UsePixelCalibCondDB  = FALSE)
   ToolSvc += InDetClusterMakerTool
   print      InDetClusterMakerTool
   #
   # SCT_ClusteringTool (public)
   #
   from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
   InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name         = "InDetSCT_ClusteringTool",
                                                       globalPosAlg = InDetClusterMakerTool)
   ToolSvc += InDetSCT_ClusteringTool
   print      InDetSCT_ClusteringTool
   #
   # SCT_Clusterization algorithm
   #
   from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
   InDetSCT_Clusterization = InDet__SCT_Clusterization(name = "InDetSCT_Clusterization",
                                                       clusteringTool      = InDetSCT_ClusteringTool,
                                                       #ChannelStatus      = InDetSCT_ChannelStatusAlg,
                                                       DetectorManagerName = "SCT_Manager", 
                                                       DataObjectName      = "SCT_RDOs",
                                                       ClustersName        = "SCT_Clusters")
   topSequence += InDetSCT_Clusterization
   print          InDetSCT_Clusterization

#--------------------------------------------------------------
# Ensure Services are correctly setup
#--------------------------------------------------------------
if not hasattr( ServiceMgr, "SctPropertiesSvc" ):
   if not hasattr( ServiceMgr, "SCTLorentzAngleSvc" ):
      # Lorentz Angle Service
      from SiLorentzAngleSvc.LorentzAngleSvcSetup import lorentzAngleSvc

   # Silicon conditions service (set up by LorentzAngleSvcSetup)
   sctSiliconConditionsSvc = ServiceMgr.SCT_SiliconConditionsSvc

   # Silicon properties service
   from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesSvc;
   sctPropertiesSvc = SiPropertiesSvc(name = "SCT_SiPropertiesSvc",
                                      DetectorName="SCT",
                                      SiConditionsServices = sctSiliconConditionsSvc)
   ServiceMgr += sctPropertiesSvc
   
#--------------------------------------------------------------
# Setup SCT_DigitizationRTT
#--------------------------------------------------------------
from SCT_DigitizationRTT.SCT_DigitizationRTTConf import SCT_DigitizationRTT
topSequence += SCT_DigitizationRTT("SCT_DigitizationRTT")
SCT_DigiRTT = topSequence.SCT_DigitizationRTT
SCT_DigiRTT.SelectEvent         = False
SCT_DigiRTT.PrintGeomInfo       = False
SCT_DigiRTT.PrintEvents         = False
SCT_DigiRTT.RawDataHists        = True
SCT_DigiRTT.SimDataHists        = True
SCT_DigiRTT.G4HitsHists         = True
SCT_DigiRTT.CresHists           = True
SCT_DigiRTT.MinSctBrlClusMult   = 100
SCT_DigiRTT.MaxSctBrlClusMult   = 0
SCT_DigiRTT.OutputLevel         = ERROR

print "List all DLL"
print theApp.Dlls
print "List all ExtSvc"
print theApp.ExtSvc
print "List of all top algorithms"
print theApp.TopAlg
print "List of all tools"
print      ToolSvc
print "List of all DetFlags"
DetFlags.Print()

#--------------------------------------------------------------
# Debugging Options
#--------------------------------------------------------------
MessageSvc.OutputLevel = ERROR #INFO
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True  #true will dump data store contents

#use a wide name field to "see" the tools in use
#svcMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"
