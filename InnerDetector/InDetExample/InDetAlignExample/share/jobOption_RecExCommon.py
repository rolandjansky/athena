## The Following options can be specified seperately in a file that include this one
loadInDetRec_Options = {"detectorDescription" : ""
                        ,"doReadBS":False
                        ,"realData":False
                        ,"preIBLgeometry":False
                        ,"globalTag" : ""
                        ,"doTrkNtuple":False
                        ,"numberOfEvents":30
                        ,"DigitalClustering":False
                        ,"useTRT": True
                        ,"GoodRunList":""
                        ,"inputFiles":["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s624_s633_d287_tid112426_00/RDO.112426._000007.pool.root.1"]
                        ,"Cosmics":False
                        ,"BField":False
                        ,"SkipEvents":0
                        ,"projectName" : ""
                        ,"dataSource" : ""
                        ,"HeavyIons" : False
                        }

# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

printfunc ("Starting loadInDet_Rec")

#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs


from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
ServiceMgr += AthenaEventLoopMgr()
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100


#--------------------------------------------------------------------------------
#Setup jobProperties  - Override Autoconfiguration of the BField. Needed for MC BOFF-
#--------------------------------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.BFieldFlags import jobproperties
if not loadInDetRec_Options["realData"] and loadInDetRec_Options["Cosmics"]: # and not loadInDetRec_Options["BField"] PF: I think this causes the wrong setup in Cosmics MC
  printfunc ("INFO::Setting Up Manually for MC Cosmics")
  jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.0)
  jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
  jobproperties.BField.solenoidOn.set_Value_and_Lock	(loadInDetRec_Options["BField"])
  jobproperties.BField.barrelToroidOn.set_Value_and_Lock(loadInDetRec_Options["BField"])
  jobproperties.BField.endcapToroidOn.set_Value_and_Lock(loadInDetRec_Options["BField"])
  #rec.doESD.set_Value_and_Lock(False)    #Important that this flag has to be true. Otherwise no recontruction algorithms are called
  jobproperties.Beam.energy.set_Value_and_Lock(0)
  jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
  
if loadInDetRec_Options["HeavyIons"]:
  jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
else:
  jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)
#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

athenaCommonFlags.FilesInput = loadInDetRec_Options["inputFiles"]

from AthenaCommon.GlobalFlags import globalflags
if len(loadInDetRec_Options["globalTag"])!=0:
  globalflags.ConditionsTag.set_Value_and_Lock(loadInDetRec_Options["globalTag"])

if len(loadInDetRec_Options["detectorDescription"])!=0:
  globalflags.DetDescrVersion.set_Value_and_Lock(loadInDetRec_Options["detectorDescription"])

if len(loadInDetRec_Options["dataSource"])!=0:
  globalflags.DataSource.set_Value_and_Lock(loadInDetRec_Options["dataSource"])

if len(loadInDetRec_Options["projectName"])!=0:
  rec.projectName.set_Value_and_Lock(loadInDetRec_Options["projectName"])


if len(globalflags.ConditionsTag())!=0:
  printfunc ("setting global tag")
  printfunc (rec.projectName())
  from IOVDbSvc.CondDB import conddb
  conddb.setGlobalTag(globalflags.ConditionsTag())

import MagFieldServices.SetupField

# --- number of events to process
athenaCommonFlags.EvtMax     = int(loadInDetRec_Options["numberOfEvents"])
athenaCommonFlags.SkipEvents = 0

doVP1 = False

# --- disable error protection of RecExCommon
athenaCommonFlags.AllowIgnoreConfigError.set_Value_and_Lock(False)

#--------------------------------------------------------------
# Additional Detector Setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 

# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- turn off Calo
DetFlags.Calo_setOff()
# --- and switch off Muons
DetFlags.Muon_setOff()

if loadInDetRec_Options["BField"]:
  DetFlags.BField_setOn()
else:
  DetFlags.BField_setOff()
if not loadInDetRec_Options["useTRT"]:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()

DetFlags.pileup.ID_setOff()
DetFlags.overlay.ID_setOff()
DetFlags.digitize.ID_setOff()
DetFlags.geometry.ID_setOff()
DetFlags.pileup.ID_setOff()
DetFlags.readRDOPool.ID_setOff()
DetFlags.readRIOBS.ID_setOff()
DetFlags.readRIOPool.ID_setOff()
DetFlags.simulate.ID_setOff()
DetFlags.simulateLVL1.ID_setOff()
DetFlags.writeBS.ID_setOff()
DetFlags.writeRDOPool.ID_setOff()
DetFlags.writeRIOPool.ID_setOff()

#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
rec.OutputLevel.set_Value_and_Lock       (INFO)

# --- controls what is written out. ESD includes AOD, so it's normally enough
# --- ESD writing temporarily disabled post 2013 migration - to be reinstated! 

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

rec.doWriteESD.set_Value_and_Lock        (False)
rec.doAOD.set_Value_and_Lock             (False)
rec.doWriteAOD.set_Value_and_Lock        (False)
rec.doDPD.set_Value_and_Lock             (False)
rec.doCBNT.set_Value_and_Lock            (False)
rec.doWriteTAG.set_Value_and_Lock        (False)

# --- turn on InDet
rec.doInDet.set_Value_and_Lock           (True)
# --- turn off calo
rec.doCalo.set_Value_and_Lock            (False)
# --- turn off muons
rec.doMuon.set_Value_and_Lock            (False) 
# --- turn off forward detectors
rec.doForwardDet.set_Value_and_Lock      (False)
# --- turn off trigger for Cosmics Data
if loadInDetRec_Options["realData"] and loadInDetRec_Options["Cosmics"]:
    rec.doTrigger.set_Value_and_Lock     (False)

# --- turn off combined reconstruction
rec.doEgamma.set_Value_and_Lock          (False)
rec.doMuonCombined.set_Value_and_Lock    (False)
rec.doTau.set_Value_and_Lock             (False)
rec.doJetMissingETTag.set_Value_and_Lock (False)


# --- turn off global monitoring 
rec.doMonitoring.set_Value_and_Lock      (False)

# --- Heavy Ions
if loadInDetRec_Options["HeavyIons"]:
  rec.doHeavyIon.set_Value_and_Lock      (True)
  from HIRecExample.HIRecExampleFlags import jobproperties
  jobproperties.HIRecExampleFlags.doHIGlobal.set_Value_and_Lock         (False)
  rec.Commissioning.set_Value_and_Lock(True)
  #rec.doExpressProcessing.set_Value_and_Lock(True);
  
 
# --- For Commissioning phase
#rec.Commissioning.set_Value_and_Lock     (True);  #should keep the InDet25nsec flag to False (check the implications). Relax max number of holes for the InDetTriggerTrackingCuts
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec.set_Value_and_Lock(True)


# --- turn of calo stuff we don't need anyway
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock  (False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoTower.set_Value_and_Lock    (False)

# --- turn off jets (Hack!!!)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock          (False)

# --- turn off egamma Brem
recAlgs.doEgammaBremReco.set_Value_and_Lock  (False)
# --- turn off Eflow and missing ET
recAlgs.doEFlow.set_Value_and_Lock           (False)
recAlgs.doEFlowJet.set_Value_and_Lock        (False)
recAlgs.doMissingET.set_Value_and_Lock       (False)
recAlgs.doMissingETSig.set_Value_and_Lock    (False)
recAlgs.doObjMissingET.set_Value_and_Lock    (False)
# --- turn off combined muons
recAlgs.doMuGirl.set_Value_and_Lock          (False)
recAlgs.doMuTag.set_Value_and_Lock           (False)
recAlgs.doMuidLowPt.set_Value_and_Lock       (False)
recAlgs.doMuonIDCombined.set_Value_and_Lock  (False)
recAlgs.doMuonIDStandAlone.set_Value_and_Lock(False)
recAlgs.doMuonSpShower.set_Value_and_Lock    (False)
recAlgs.doStaco.set_Value_and_Lock           (False)
recAlgs.doCaloTrkMuId.set_Value_and_Lock     (False)
recAlgs.doTileMuID.set_Value_and_Lock        (False)
# --- trigger
rec.doTagRawSummary.set_Value_and_Lock   (False)
rec.doTrigger.set_Value_and_Lock             (False)


#--------------------------------------------------------------
# Printout config
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
printfunc ("globalflags configuration:")
printfunc (globalflags)

printfunc ("detflags configuration:")
DetFlags.Print()

printfunc ("rec configuration:")
printfunc (rec)

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties (default)
from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doTruth.set_Value_and_Lock (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
InDetFlags.doTruth.set_Value_and_Lock (False)

#Anthony's recommendation: Is to load old SCT conditions. To be off >= M8
#InDetFlags.ForceCoraCool.set_Value_and_Lock                        (True)

# --- enable brem recovery
InDetFlags.doBremRecovery.set_Value_and_Lock                       (False)
InDetFlags.doCaloSeededBrem.set_Value_and_Lock                     (False)
# --- enable forward tracks
InDetFlags.doForwardTracks.set_Value_and_Lock                      (False)
# --- enable 
InDetFlags.doTrackSegmentsDisappearing.set_Value_and_Lock   (False)
# --- enable low mu run setup
InDetFlags.doLowMuRunSetup.set_Value_and_Lock                      (False)
InDetFlags.doTRTSeededTrackFinder.set_Value_and_Lock               (False)
InDetFlags.doBackTracking.set_Value_and_Lock                       (False)
InDetFlags.doPseudoTracking.set_Value_and_Lock                     (False)
# --- activate monitorings
InDetFlags.doMonitoringGlobal.set_Value_and_Lock                   (False)
InDetFlags.doMonitoringPrimaryVertexingEnhanced.set_Value_and_Lock (False)
InDetFlags.doMonitoringPixel.set_Value_and_Lock                    (False)
InDetFlags.doMonitoringSCT.set_Value_and_Lock                      (False)
InDetFlags.doMonitoringTRT.set_Value_and_Lock                      (False)
InDetFlags.doMonitoringAlignment.set_Value_and_Lock                (False)

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables.set_Value_and_Lock                 (True)


# deactivate slimming 
InDetFlags.doSlimming.set_Value_and_Lock                           (False)

# Store Gate deletion in case of HI to off
if loadInDetRec_Options["HeavyIons"]:
  InDetFlags.doSGDeletion.set_Value_and_Lock                            (False) 


# --- Options that we may change in the alignment
if loadInDetRec_Options["Cosmics"]:
    printfunc ("#### INFO:: jobOptions_RecExCommon: Cosmics")
    InDetFlags.doPixelClusterSplitting.set_Value_and_Lock          (False)
    InDetFlags.doTIDE_Ambi.set_Value_and_Lock                      (False);
    #InDetFlags.cutLevel.set_Value_and_Lock                             (8) #is for cosmics
else:
    # Disable seed TRT tracking
  if loadInDetRec_Options["HeavyIons"]:
    InDetFlags.cutLevel.set_Value_and_Lock                             (3)
  else:
    InDetFlags.cutLevel.set_Value_and_Lock                             (12)    
    



# Disable TRT only tracking  
InDetFlags.doTRTStandalone.set_Value_and_Lock                      (False)
InDetFlags.doBackTracking.set_Value_and_Lock                       (False)

# --- We are in commissioning phase in case of cosmics and comm
#if "cos" in loadInDetRec_Options["projectName"] or "comm" in loadInDetRec_Options["projectName"]:
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock         (False)
InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);
if loadInDetRec_Options["DigitalClustering"]:
    printfunc ("jobOptions_RecExCommon: DigitalClustering")
    InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False);
    InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False);
else:
    InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(True);
    InDetFlags.doTIDE_Ambi.set_Value_and_Lock(True);

#if "comm" in loadInDetRec_Options["projectName"]:
#    InDetFlags.useDCS.set_Value_and_Lock(False)

#InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock                 (True) 
#InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock                   (True)
#InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock                   (True)

# --- if we are using ESD (make flag) // Revist later
#InDetFlags.doPRDFormation = False

# Only for Data

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")

#Don't write any override here.
#Use the jobOptions_ConditionsOverrider.py  


if doVP1:
  from VP1Algs.VP1AlgsConf import VP1Alg
  topSequence += VP1Alg()

