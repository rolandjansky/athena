#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.Resilience import treatException
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax = 20#00
jobproperties.AthenaCommonFlags.SkipEvents = 0
jobproperties.AthenaCommonFlags.PoolHitsInput = [ "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1" ]
#"root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/SCT_SingleMuons/mc10_7TeV.107370.singlepart_mu_pt6_50_GeV.simul.HITS.e628.pool.root" ]
jobproperties.AthenaCommonFlags.PoolRDOOutput = "SCT_DigitizationRTTevents.root"

from Digitization.DigitizationFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties

jobproperties.Digitization.IOVDbGlobalTag='OFLCOND-SDR-BS7T-04-02'
jobproperties.Digitization.doMinimumBias=True
jobproperties.Digitization.numberOfCollisions=2.3
jobproperties.Digitization.bunchSpacing=25 # This now sets the bunch slot length.
jobproperties.Digitization.initialBunchCrossing=-2
jobproperties.Digitization.finalBunchCrossing=2
## mimic 25ns bunch spacing (using smallest repeatable unit)
#jobproperties.Digitization.BeamIntensityPattern=[1.0] 
#jobproperties.Beam.bunchSpacing=25

## mimic 50ns bunch spacing (using smallest repeatable unit)
#jobproperties.Digitization.BeamIntensityPattern=[0.0,1.0]
#jobproperties.Beam.bunchSpacing=50

## mimic 75ns bunch spacing (using smallest repeatable unit)
jobproperties.Digitization.BeamIntensityPattern=[0.0,1.0,0.0]
jobproperties.Beam.bunchSpacing=75

include("Digitization/mc10minbiasfiles.py")


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
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()
DetFlags.writeRDOPool.SCT_setOff()##
DetFlags.writeRDOPool.Truth_setOff()
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
## from AthenaCommon.AppMgr import ToolSvc
## #The rest of this section can be commented out with no ill effects
## from SCT_Digitization.SCT_DigitizationConf import SCT_FrontEnd
## theSCT_FrontEnd = SCT_FrontEnd("SCT_FrontEnd")
## try:
##    theSCT_FrontEnd.NoiseOn = True
## except:
##    treatException("Could not find theSCT_FrontEnd.NoiseOn property. Skipping...")
## # DataCompressionMode: 1 is level mode(default), 2 is edge mode
## try:
##    theSCT_FrontEnd.DataCompressionMode = 1
## except:
##    treatException("Could not find theSCT_FrontEnd.DataCompressionMode property. Skipping...")
## ToolSvc += theSCT_FrontEnd
## print      theSCT_FrontEnd

#--------------------------------------------------------------
# Setup Output File For Histograms
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr+=THistSvc()
ServiceMgr.THistSvc.Output = ["SCT_ReadoutModeChecker DATAFILE='SCT_ReadoutModeCheckerHists.root' opt='RECREATE'"];

DetFlags.Print()

#--------------------------------------------------------------
# Setup SCT_DigitizationRTT
#--------------------------------------------------------------
from SCT_DigitizationRTT.SCT_DigitizationRTTConf import SCT_ReadoutModeChecker
topSequence += SCT_ReadoutModeChecker("SCT_ReadoutModeChecker",
                                      RDOsWrittenOutInCompressedMode=True)
SCT_RMC = topSequence.SCT_ReadoutModeChecker
if (jobproperties.Beam.bunchSpacing() <= 25):
   SCT_RMC.ExpectedReadoutMode="EDGE_MODE"
elif (jobproperties.Beam.bunchSpacing() <= 50):
   SCT_RMC.ExpectedReadoutMode="LEVEL_MODE"
else:
   SCT_RMC.ExpectedReadoutMode="ANY_HIT_MODE"
## SCT_RMC.OutputLevel         = VERBOSE


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
#MessageSvc.OutputLevel = ERROR #INFO
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True  #true will dump data store contents

#use a wide name field to "see" the tools in use
#svcMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.enableSuppression =False
