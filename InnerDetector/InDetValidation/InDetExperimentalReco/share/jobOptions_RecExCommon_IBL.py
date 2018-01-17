
# --- brem recovery
doBremRecovery   = True
# --- calo seeded brem
doCaloSeededBrem = True

#AtlasLayerMat_x16_ATLAS-IBL-03
from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_x16_ATLAS-IBL-03') 
#conddb.addOverride('/GLOBAL/TrackingGeo/LayerMaterialV2','AtlasLayerMat_v19s0_ATLAS-R2-2015-02') 
#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
import MagFieldServices.SetupField

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-RUN12-SDR-10')
#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# top no pileup
athenaCommonFlags.FilesInput = ["root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/rucio/valid1/b8/04/RDO.04860198._000028.pool.root.1"]

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
#InDetFlags.priVtxCutLevel.set_Value_and_Lock                       (2)

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


# --- test new cut level
# InDetFlags.cutLevel.set_Value_and_Lock                             (11)


# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")

#ToolSvc.NnClusterizationFactory.OutputLevel = VERBOSE
