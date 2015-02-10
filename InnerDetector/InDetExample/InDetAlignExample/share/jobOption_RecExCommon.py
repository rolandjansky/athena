## The Following options can be specified seperately in a file that include this one
loadInDetRec_Options = {"detectorDescription" : ""
                        ,"doReadBS":False
                        ,"realData":False
                        ,"preIBLgeometry":False
                        ,"globalTag" : ""
                        ,"doTrkNtuple":False
                        ,"numberOfEvents":30
                        ,"useTRT": True
                        ,"GoodRunList":""
                        ,"inputFiles":["root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s624_s633_d287_tid112426_00/RDO.112426._000007.pool.root.1"]
                        ,"Cosmics":False
                        ,"BField":False
                        ,"SkipEvents":0
                        ,"projectName" : ""
                        ,"dataSource" : ""
                        }

# If not defined the defaults given above are used
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

print "Starting loadInDet_Rec"

#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs


#--------------------------------------------------------------------------------
#Setup jobProperties  - Override Autoconfiguration of the BField. Needed for MC -
#--------------------------------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.BFieldFlags import jobproperties
if not loadInDetRec_Options["realData"]:
  print "INFO::Setting Up Manually for MC Cosmics"
  jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.0)
  jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
  jobproperties.BField.solenoidOn.set_Value_and_Lock	(loadInDetRec_Options["BField"])
  jobproperties.BField.barrelToroidOn.set_Value_and_Lock(loadInDetRec_Options["BField"])
  jobproperties.BField.endcapToroidOn.set_Value_and_Lock(loadInDetRec_Options["BField"])
  rec.doESD.set_Value_and_Lock(True)
  jobproperties.Beam.energy.set_Value_and_Lock(0)
  jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
  

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
  print "setting global tag"
  print rec.projectName()
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

if not loadInDetRec_Options["realData"]:
  DetFlags.BField_setOff()

if not loadInDetRec_Options["useTRT"]:
  DetFlags.TRT_setOff()
  DetFlags.detdescr.TRT_setOn()

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
# --- turn off trigger
rec.doTrigger.set_Value_and_Lock         (False)

# --- turn off combined reconstruction
rec.doEgamma.set_Value_and_Lock          (False)
rec.doMuonCombined.set_Value_and_Lock    (False)
rec.doTau.set_Value_and_Lock             (False)
rec.doJetMissingETTag.set_Value_and_Lock (False)

# --- turn of calo stuff we don't need anyway
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock  (False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoTower.set_Value_and_Lock    (False)

# --- turn of jets (Hack!!!)
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
recAlgs.doTrigger.set_Value_and_Lock         (False)


#--------------------------------------------------------------
# Printout config
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
print "globalflags configuration:"
print globalflags

print "detflags configuration:"
DetFlags.Print()

print "rec configuration:"
print rec

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties (default)
from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doTruth.set_Value_and_Lock (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
InDetFlags.doTruth.set_Value_and_Lock (False)

#Anthony's recommendation
InDetFlags.ForceCoraCool.set_Value_and_Lock                        (True)

# --- enable brem recovery
InDetFlags.doBremRecovery.set_Value_and_Lock                       (False)
InDetFlags.doCaloSeededBrem.set_Value_and_Lock                     (False)
# --- enable forward tracks
InDetFlags.doForwardTracks.set_Value_and_Lock                      (False)
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

# --- Options that we may change in the alignment
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock              (False)
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock         (True)
InDetFlags.useBroadClusterErrors.set_Value_and_Lock                (False)

#InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock                 (True) 
#InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock                   (True)
#InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock                   (True)

InDetFlags.doTRTStandalone.set_Value_and_Lock                      (False)

# --- if we are using ESD (make flag) // Revist later
#InDetFlags.doPRDFormation = False

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")

#if (ErrorScaling or (".db" in errorScalingTag) ):
#    print " <jobObtion_RecExCommon> Using Error Scaling " 
#    from AthenaCommon.AppMgr import ToolSvc;
#    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackErrorScalingTool;
#    tool = Trk__RIO_OnTrackErrorScalingTool(name = 'RIO_OnTrackErrorScalingTool',
#                                            overrideDatabaseID = False,
#                                            OutputLevel = 1);
#    ToolSvc += tool 
#else:
#    print " <jobObtion_RecExCommon> NOT using Error Scaling " 
    
    

if doVP1:
  from VP1Algs.VP1AlgsConf import VP1Alg
  topSequence += VP1Alg()

