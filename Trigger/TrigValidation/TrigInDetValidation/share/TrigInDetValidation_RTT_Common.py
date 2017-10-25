if not ('EventMax' in dir()):
  EventMax=-1
athenaCommonFlags.EvtMax=EventMax

#Import datasets from RTT--------------------------------------------------------------------------
def importRTTdatasets(jobID):
    from AthenaCommon.Utils.unixtools import find_datafile
    xmlFile = find_datafile("TrigInDetValidation_TestConfiguration.xml")
    import xml.etree.ElementTree as elemTree
    tree = elemTree.parse(xmlFile)
    namespace = "{http://www.hep.ucl.ac.uk/atlas/AtlasTesting/rtt}"
    datasetList = []
    root = tree.getroot()
    for job in root.iter(namespace + 'athena'):
        if job.get('userJobId') == jobID:
            for dataset in job.findall(namespace + 'dataset'):
                eosDataset = "root://eosatlas/" + dataset.text
                datasetList.append(eosDataset)
    return datasetList
#--------------------------------------------------------------------------------------------------

if 'ARTConfig' in dir(): 
   from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
   athenaCommonFlags.FilesInput=ARTConfig

if 'XMLDataSet' in dir() and XMLDataSet!="":
   print XMLDataSet
   athenaCommonFlags.FilesInput = importRTTdatasets(XMLDataSet)

if (not 'enableCostMonitoring' in dir()):
  enableCostMonitoring = True

#RTT runs with costMonitoring on - test whether it is available in a given release
#test whether a package is useable in this release
import imp
try:
  imp.find_module('TrigCostD3PDMaker')
except:
  print 'CostMonitoring packages not available, setting  enableCostMonitoring=False'
  enableCostMonitoring=False

if not ('doIDNewTracking' in dir()):
  doIDNewTracking=False

if not ('OutputLevel' in dir()):
  OutputLevel=INFO

useROOTNtuple=True # False : hbook ntuple

if not ('writexAOD' in dir()):
  writexAOD=False
     
#switch off all ESD making algorithms
from RecExConfig.RecFlags import rec
rec.doESD=False
rec.doAOD=False
rec.doWriteESD=False
rec.doWriteAOD=writexAOD

try: 
  rec.doWritexAOD=writexAOD
except:
  print "property doWritexAOD does not exist"

rec.doWriteTAG=False


# switch on detectors
rec.doTruth=False     # needs to be true if creating Fake RoI

rec.doForwardDet=False
rec.doInDet=True
rec.doCalo=True
rec.doMuon=True
rec.doEgamma=False
rec.doMuonCombined=False
rec.doJetMissingETTag=False
rec.doTau=False
rec.doTrigger=True


from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.Enabled.set_Value_and_Lock(False)

from RecExConfig.RecAlgsFlags import recAlgs
# recAlgs.doCaloTrkMuId.set_Value_and_Lock(False)
# recAlgs.doMuonIDStandAlone.set_Value_and_Lock(False)
# recAlgs.doMuonIDCombined.set_Value_and_Lock(False)
# recAlgs.doMuGirl.set_Value_and_Lock(False)
# recAlgs.doTileMuID.set_Value_and_Lock(False)
# recAlgs.doMuonSpShower.set_Value_and_Lock(False)
# recAlgs.doMissingET.set_Value_and_Lock(False)
# recAlgs.doStaco.set_Value_and_Lock(False)
# recAlgs.doMuTag.set_Value_and_Lock(False)
# recAlgs.doTrigger.set_Value_and_Lock(True)


### JK Do reidual monitoring for FTK tracks
from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings as InDetSettings
InDetSettings[('doResMon','FTK')]=True
InDetSettings[('doResMon','FTKRefit')]=True


flags = {}
flags.update(recAlgs.__dict__)
#flags.update(muonRecFlags.__dict__)

for domethod in flags:
  if domethod.startswith('do') and domethod is not 'doTrigger':
    runlockon = recAlgs.__dict__[domethod]
    runlockon.set_Value_and_Lock(False)




from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)



#switch off some InDet reco
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doNewTracking=doIDNewTracking
# disable the pixel neural net clustering
InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False)
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)
InDetFlags.doBackTracking=False
InDetFlags.doTRTStandalone=False
InDetFlags.doiPatRec=False
InDetFlags.doTruth=False
InDetFlags.doPRDFormation=doIDNewTracking
InDetFlags.doSpacePointFormation=doIDNewTracking
InDetFlags.doParticleCreation=doIDNewTracking
InDetFlags.doSecVertexFinder=False
InDetFlags.doV0Finder=False
InDetFlags.preProcessing=doIDNewTracking
InDetFlags.postProcessing=doIDNewTracking



doAODLVL1=False

doTrigger=True
TriggerModernConfig=True
from TriggerJobOpts.TriggerFlags import TriggerFlags

no_Bphysics=True
include("RecExCond/RecExCommon_flags.py")




# pre set up trigger monitoring
TriggerFlags.enableMonitoring = [ 'Validation','Time' ]
if 'enableCostMonitoring' in dir() and bool(enableCostMonitoring) == True:
    import TriggerRelease.Modifiers
    getattr(TriggerRelease.Modifiers,'enableCostMonitoring')().preSetup()
    getattr(TriggerRelease.Modifiers,'enableCostForCAF')().preSetup()

#
if 'use_new_tm' in dir() and use_new_tm:
  if 'triggerMenuVersion' in dir():
    TriggerFlags.triggerMenuSetup = triggerMenuVersion
  else:
    TriggerFlags.triggerMenuSetup = 'Physics_pp_v6'
  TriggerFlags.doHLT=True
  from TriggerMenu.menu.GenerateMenu import GenerateMenu
else:
  TriggerFlags.triggerMenuSetup = 'MC_pp_v4_no_prescale'
  from TriggerMenuPython.GenerateMenu import GenerateMenu

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

#TriggerFlags.doMuon=False
TriggerFlags.doHypo=False
TriggerFlags.doTruth=True


GenerateMenu.overwriteSignaturesWith(resetSigs)



#-------------------------------------------------------------
# End of setting flags
#-------------------------------------------------------------


include( "RecExCommon/RecExCommon_topOptions.py" )

TriggerFlags.abortOnConfigurationError=True

Service ("StoreGateSvc" ).ActivateHistory=False 
Service( "RegSelSvc" ).enableCalo = False
# disabling pixel barrel layer 1 for robustness test
if 'disablePixelLayer' in dir() and disablePixelLayer == True:
  import TrigInDetValidation.InDetModules as IDM
  pixel_barrel_layer1_hashes = IDM.getHashes(IDM.getLayer(IDM.getBarrel(IDM.Pixel),1))
  # RegSelSvcDefault.DeletePixelHashList= pixel_barrel_layer1_hashes
  Service( "RegSelSvc" ).DeletePixelHashList=pixel_barrel_layer1_hashes
  #a new svc
  from TrigIDUtils.TrigIDUtilsConf import TestPixelModuleDisablingSvc
  tpmdsvc = TestPixelModuleDisablingSvc(name="TestPixelModuleDisablingSvc")
  tpmdsvc.ModuleHashes = pixel_barrel_layer1_hashes
  from AthenaCommon.AppMgr import ServiceMgr as svcMgr
  svcMgr += tpmdsvc

  from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
  from InDetTrigRecExample.InDetTrigConditionsAccess import PixelConditionsSetup
  InDetTrigPixelConditionsSummarySvc = PixelConditionsSummarySvc(PixelConditionsSetup.instanceName('PixelConditionsSummarySvc'))
  InDetTrigPixelConditionsSummarySvc.UseTDAQ = True
  InDetTrigPixelConditionsSummarySvc.TDAQSvcName = "TestPixelModuleDisablingSvc/TestPixelModuleDisablingSvc"


MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"
Service ("StoreGateSvc" ).ActivateHistory=False


#from AthenaCommon.AppMgr import ServiceMgr as svcMgr                                                                                                                         
Service("TrigTimerSvc").IncludeName=".+"
#svcMgr.TrigTimerSvc.IncludeName=".+"                      

from InDetTrigRecExample.EFIDUtils import debuglevel,muondebugoff,tooldiff
#debuglevel()
#muondebugoff()
#tooldiff()


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


##### Trigger Decision Part #####################

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
# tdt.OutputLevel = INFO

##### Histogram File Part #################

from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "HLTMonManager")
HLTMonManager = topSequence.HLTMonManager
##
# 
from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr 
svcMgr += BeamCondSvc(name="InDetBeamSpotOnline")



############ TrigInDetAnalysis part ################################

from TrigInDetAnalysisExample.TrigInDetAnalysisExampleConf import TrigTestMonToolAC
TestMonTool = TrigTestMonToolAC( name="TestMonToolAC")
# TestMonTool.OutputLevel = INFO
try:
  TestMonTool.EnableLumi = False
except:
  pass



TestMonTool.buildNtuple = True
#TestMonTool.AnalysisConfig = "Tier0" #Change to Tier0 for T0 Analysis
TestMonTool.AnalysisConfig = "nTuple" #Change to Tier0 for T0 Analysis
if (rec.doTruth == True):
	TestMonTool.mcTruth = True 
	TestMonTool.ntupleChainNames = ['Truth']

else:
	TestMonTool.mcTruth = False
	TestMonTool.ntupleChainNames = ['']

#Tau 3-prong - false by default
#TestMonTool.doTauThreeProng = True
#Tau Et cut
#TestMonTool.tauEtCutOffline= True

if('PdgId' in dir()):
     TestMonTool.SelectTruthPdgId=PdgId
     print "select truth pdgid : ", TestMonTool.SelectTruthPdgId	

#

#TestMonTool.KeepAllEvents=True
#print TestMonTool
#print HLTMonManager.AthenaMonTools


if 'tidaAnalysischains' in dir():
     TestMonTool.ntupleChainNames += tidaAnalysischains
else:
     TestMonTool.ntupleChainNames += chainNames

from AthenaCommon.AppMgr import release_metadata
d = release_metadata()
TestMonTool.releaseMetaData = d['nightly name'] + " " + d['nightly release'] + " " + d['date'] + " " + d['platform'] + " " + d['release']

TestMonTool.outputFileName="TrkNtuple.root"
ToolSvc += TestMonTool
#TestMonTool.OutputLevel=DEBUG

HLTMonManager.AthenaMonTools += [ "TrigTestMonToolAC/TestMonToolAC" ]

# post set up trigger monitoring
if 'enableCostMonitoring' in dir() and bool(enableCostMonitoring) == True:
    import TriggerRelease.Modifiers
    getattr(TriggerRelease.Modifiers,'enableCostMonitoring')().postSetup()
    getattr(TriggerRelease.Modifiers,'enableCostForCAF')().postSetup()
    getattr(TriggerRelease.Modifiers,'enableCostD3PD')().postSetup()
    # Check if we are debugging the cost mon output - false by default
    if 'enableCostDebug' in dir() and bool(enableCostDebug) == True:
      getattr(TriggerRelease.Modifiers,'enableCostDebug')().postSetup()

print TestMonTool 


if hasattr(topSequence, 'TrigSteer_L2') or hasattr(topSequence, 'TrigSteer_HLT'):
  print 'topSequence has an TrigSteer object, continuing execution'
else:
  print 'topSequence has no TrigSteer object, aborting execution'
  print "Don't report this error, check for an configuration ERROR earlier in the log file"
  import sys
  sys.exit(1)

