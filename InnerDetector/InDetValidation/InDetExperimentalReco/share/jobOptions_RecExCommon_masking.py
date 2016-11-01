
# --- brem recovery
doBremRecovery   = True
# --- calo seeded brem
doCaloSeededBrem = True

#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# top 40 pileup
athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/phys-rig/pileupSamples/LS1Samples/valid1.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.recon.RDO.e1565_s1499_s1504_r4242_tid01185596_00/RDO.01185596._000001.pool.root.1"]
# top no pileup (old default)
#athenaCommonFlags.FilesInput = ["root://eosatlas///eos/atlas/atlasgroupdisk/perf-idtracking/dq2/rucio/mc12_valid/ac/91/RDO.01385812._000001.pool.root.1"]
# H-gamgam, 40 pileup
#athenaCommonFlags.FilesInput = ["root://eosatlas///eos/atlas/atlasdatadisk/mc11_valid/RDO/e850_s1310_s1300_d577/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d577_tid512895_00/RDO.512895._000039.pool.root.1"]
# H-gamgam, 20 pileup
#athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e850_s1310_s1300_d576/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d576_tid512888_00/RDO.512888._000001.pool.root.1"]
# H-gamgam, 0 pileup (filename ?)
#athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-egamma/dq2/mc11_valid/RDO/e850_s1310_s1300_d578/mc11_valid.106384.PythiaH120gamgam.digit.RDO.e850_s1310_s1300_d578_tid512902_00/RDO.512902._000001.pool.root.1"]
# electrons 5 GeV
# athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105128.singlepart_e_pt5.digit.RDO.e605_s933_s946_d369/RDO.219312._000024.pool.root.2"]
# electrons 100 GeV
# athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105129.singlepart_e_pt100.digit.RDO.e605_s933_s946_d369/RDO.219313._000002.pool.root.1"]
# pions 1 GeV
# athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105130.singlepart_singlepi_pt1.digit.RDO.e605_s933_s946_d369/RDO.197118._000005.pool.root.1"]
# taus 10-500 GeV
#athenaCommonFlags.FilesInput = ["root://eosatlas//eos/atlas/atlasgroupdisk/perf-tau/dq2/mc11_7TeV/RDO/e1063_s1378_s1370_d642/mc11_7TeV.107380.SingleTauHad_Pt10_500_GeV.digit.RDO.e1063_s1378_s1370_d642_tid704612_00/RDO.704612._000001.pool.root.1"]

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-R1-2012-01-00-00'
import AtlasGeoModel.SetGeometryVersion


import MagFieldServices.SetupField

# --- number of events to process
athenaCommonFlags.EvtMax     =  3
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

rec.doWriteESD.set_Value_and_Lock        (True)###
rec.doAOD.set_Value_and_Lock             (True)
rec.doWriteAOD.set_Value_and_Lock        (True)
rec.doDPD.set_Value_and_Lock             (False)
rec.doCBNT.set_Value_and_Lock            (False)
rec.doWriteTAG.set_Value_and_Lock        (False)

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

recAlgs.doTrackParticleCellAssociation.set_Value_and_Lock(False)

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
InDetFlags.doTruth.set_Value_and_Lock (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')

# --- enable brem recovery
# InDetFlags.doBremRecovery.set_Value_and_Lock                       (doBremRecovery)
# --- calo seeding does not work standalone 
# InDetFlags.doCaloSeededBrem.set_Value_and_Lock                     (doCaloSeededBrem)

# --- cut level
#InDetFlags.priVtxCutLevel.set_Value_and_Lock                       (3)

# --- enable forward tracks
# InDetFlags.doForwardTracks.set_Value_and_Lock                      (True)

# --- enable low mu run setup
# InDetFlags.doLowMuRunSetup.set_Value_and_Lock                      (True)

# --- activate monitorings
InDetFlags.doMonitoringGlobal.set_Value_and_Lock                   (False)
InDetFlags.doMonitoringPrimaryVertexingEnhanced.set_Value_and_Lock (False)
InDetFlags.doMonitoringPixel.set_Value_and_Lock                    (False)
InDetFlags.doMonitoringSCT.set_Value_and_Lock                      (False)
InDetFlags.doMonitoringTRT.set_Value_and_Lock                      (False)
InDetFlags.doMonitoringAlignment.set_Value_and_Lock                (False)

# activate the print InDetXYZAlgorithm statements
InDetFlags.doPrintConfigurables.set_Value_and_Lock                 (True)

# Activate TIDE Ambi
#InDetFlags.doTIDE_Ambi.set_Value_and_Lock                          (True)

# --- test new cut level
# InDetFlags.cutLevel.set_Value_and_Lock                             (11)
InDetFlags.doSCTModuleVeto.set_Value_and_Lock                      (True)

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")


ServiceMgr.SpecialPixelMapSvc.MaskLayers = True
ServiceMgr.SpecialPixelMapSvc.LayersToMask = [0]

ServiceMgr.InDetSCT_ModuleVetoSvc.MaskLayers = True
ServiceMgr.InDetSCT_ModuleVetoSvc.MaskSide = -1
ServiceMgr.InDetSCT_ModuleVetoSvc.DisksToMask = [-9]

import AtlasGeoModel.GeoModelInit
svcMgr.GeoModelSvc.IgnoreTagDifference = True
