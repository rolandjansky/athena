
# --- brem recovery
doBremRecovery   = True
# --- calo seeded brem
doCaloSeededBrem = True

#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------
#DetDescrVersion = "ATLAS-GEO-16-00-00"
#
#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
import MagFieldServices.SetupField

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# top no pileup (default)
#athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/60/11/RDO.07497143._000001.pool.root.1"]
#athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc15_13TeV/b6/19/RDO.06718152._000041.pool.root.1"]
#athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d366/RDO.197091._000015.pool.root.1"]
#athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/InDetRecExample/mc09_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e510_s624_s633_d287_tid112426_00/RDO.112426._000007.pool.root.1"]
# H-gamgam, 40 pileup
# athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch///eos/atlas/atlasdatadisk/mc11_valid/RDO/e850_s1310_s1300_d577/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d577_tid512895_00/RDO.512895._000039.pool.root.1"]
# H-gamgam, 20 pileup
# athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e850_s1310_s1300_d576/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d576_tid512888_00/RDO.512888._000001.pool.root.1"]
# H-gamgam, 0 pileup (filename ?)
# athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e850_s1310_s1300_d578/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d578_tid512902_00/RDO.512902._000001.pool.root.1"]
# electrons 5 GeV
# athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105128.singlepart_e_pt5.digit.RDO.e605_s933_s946_d369/RDO.219312._000024.pool.root.2"]
# electrons 100 GeV
# athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105129.singlepart_e_pt100.digit.RDO.e605_s933_s946_d369/RDO.219313._000002.pool.root.1"]
# pions 1 GeV
# athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105130.singlepart_singlepi_pt1.digit.RDO.e605_s933_s946_d369/RDO.197118._000005.pool.root.1"]

# --- number of events to process
if not 'EventMax' in dir():
  EventMax = 10
  print "EventMax not set - do",EventMax,"events"
else:
  print "EventMax set to",EventMax
athenaCommonFlags.EvtMax     =  EventMax
athenaCommonFlags.SkipEvents =   0

# --- disable error protection of RecExCommon
athenaCommonFlags.AllowIgnoreConfigError.set_Value_and_Lock(False)


#--------------------------------------------------------------
# Additional Detector Setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 

# --- switch on InnerDetector
DetFlags.ID_setOn()

# --- turn off Calo
if doCaloSeededBrem:
  DetFlags.Calo_setOn()
else:
  DetFlags.Calo_setOff()

# --- and switch off Muons
DetFlags.Muon_setOff()

#from AthenaCommon.BeamFlags import jobproperties as BeamFlags
#BeamFlags.Beam.bunchSpacing.set_Value_and_Lock(50)

#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
rec.OutputLevel.set_Value_and_Lock       (INFO)

# --- controls what is written out. ESD includes AOD, so it's normally enough
rec.doWriteESD.set_Value_and_Lock        (False)
rec.doAOD.set_Value_and_Lock             (False)
rec.doWriteAOD.set_Value_and_Lock        (False)
rec.doDPD.set_Value_and_Lock             (False)
rec.doCBNT.set_Value_and_Lock            (False)
rec.doWriteTAG.set_Value_and_Lock        (False)
rec.doWritexAOD.set_Value_and_Lock       (False)
#rec.doMonitoring.set_Value_and_Lock      (True)

# --- turn on InDet
rec.doInDet.set_Value_and_Lock           (True)
# --- turn off calo
if not doCaloSeededBrem:
  rec.doCalo.set_Value_and_Lock           (False)
else:
  rec.doCalo.set_Value_and_Lock           (True)
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
# --- set geometry version
#globalflags.DetDescrVersion = DetDescrVersion

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
InDetFlags.doTruth.set_Value_and_Lock (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

# --- enable brem recovery
InDetFlags.doBremRecovery.set_Value_and_Lock                       (doBremRecovery)
# --- calo seeding does not work standalone 
InDetFlags.doCaloSeededBrem.set_Value_and_Lock                     (doCaloSeededBrem)

# --- cut level
#InDetFlags.cutLevel.set_Value_and_Lock                             (8)
#InDetFlags.priVtxCutLevel.set_Value_and_Lock                       (2)

# --- enable forward tracks
InDetFlags.doForwardTracks.set_Value_and_Lock                      (False)

# --- activate monitorings
InDetFlags.doMonitoringGlobal.set_Value_and_Lock                   (False)
InDetFlags.doMonitoringPrimaryVertexingEnhanced.set_Value_and_Lock (False)
InDetFlags.doMonitoringPixel.set_Value_and_Lock                    (False)
InDetFlags.doMonitoringSCT.set_Value_and_Lock                      (False)
InDetFlags.doMonitoringTRT.set_Value_and_Lock                      (False)
InDetFlags.doMonitoringAlignment.set_Value_and_Lock                (False)

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables.set_Value_and_Lock                 (True)

#InDetFlags.doiPatRec.set_Value_and_Lock                            (True)

# --- possibility to run tracking on subdetectors separately (and independent from each other)
# --- Si track segments incompatible with TIDE setup, which is now default
#InDetFlags.doTrackSegmentsPixel.set_Value_and_Lock                 (True)
#InDetFlags.doTrackSegmentsSCT.set_Value_and_Lock                   (True)
InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock                   (True)

# --- activate non default algorithms
InDetFlags.doV0Finder.set_Value_and_Lock                           (True)

InDetFlags.doStandardPlots.set_Value_and_Lock                      (True)

# SCZ - no track slimming
InDetFlags.doSlimming.set_Value_and_Lock                           (False)

# SCZ - set fitter - does not complain if not explicitly set, just lets default stand
if 'FitterType' in dir():
  InDetFlags.trackFitterType.set_Value_and_Lock                    (FitterType)
  if FitterType=="KalmanDNAFitter" or FitterType=="GaussianSumFilter":
    InDetFlags.materialInteractionsType.set_Value_and_Lock         (1)

if 'LowPt' in dir():
  InDetFlags.doLowPt.set_Value_and_Lock                            (LowPt)

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

# --- Truth jet creation
from JetRec.JetRecStandard import jtm
jtm.addJetFinder("Run2AntiKt4TruthJets", "AntiKt", 0.4, "truth", ghostArea=0.0)

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.doTRTElectronMon.set_Value_and_Lock               (False)
#DQMonFlags.doLArMon.set_Value_and_Lock                       (False)

include ("RecExCommon/RecExCommon_topOptions.py")

# Turn off TRT validity gate, which is not configured for 50ns RDOs
ToolSvc.InDetTRT_DriftCircleTool.ValidityGateSuppression = False
ToolSvc.InDetTRT_DriftCircleTool.SimpleOutOfTimePileupSupression = True
