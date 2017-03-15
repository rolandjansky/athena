from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer

job = AlgSequence()
seq = AthSequencer("AthFilterSeq")

###############
# GRL
###############

useGRL=False

if useGRL:
    include(os.environ["PWD"]+"/jobOptions_useGRL.py")

###############
# END GRL
###############

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
#from PerfMonComps.PerfMonFlags import jobproperties as pmjp
#pmjp.PerfMonFlags.doFastMon=True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags


athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = 0

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2016-19")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-03-29-00")

#not needed?
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.IgnoreTagDifference = True

from AthenaCommon.BeamFlags import jobproperties


## Forcing to have 25 ns bunch spacing (still needed?)
jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)

##can be removed
#from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.TrackParticleSlimmer=False
#AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False



from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.doAOD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(True) # uncomment if rec.do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if rec.do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if rec.do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if rec.do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if rec.do not write TAG
rec.doCBNT.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(True)
rec.doTile.set_Value_and_Lock(False)
rec.doLArg.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)

#rec.doMonitoring.set_Value_and_Lock(True)
#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.doInDetPerfMon.set_Value_and_Lock(True)


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
#DetFlags.Tile_setOff()

#DetFlags.makeRIO.Calo_setOff()
#DetFlags.detdescr.Calo_setOn()


# --- turn of calo stuff we don't need anyway
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock  (False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoTower.set_Value_and_Lock    (False)


# --- turn off jets (Hack!!!)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock          (False)

from RecExConfig.RecAlgsFlags import recAlgs
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
rec.doTagRawSummary.set_Value_and_Lock   (False)
rec.doTrigger.set_Value_and_Lock             (False)


# --- setup InDetJobProperties (default)

from InDetRecExample.InDetJobProperties import InDetFlags
#InDetFlags.doTruth.set_Value_and_Lock (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
InDetFlags.doTruth.set_Value_and_Lock (False)

#Anthony's recommendation: Is to load old SCT conditions. To be off >= M8
#InDetFlags.ForceCoraCool.set_Value_and_Lock                        (True)


#Reconstruction for the lowMu runs  - should be removed?
InDetFlags.disableInDetReco.set_Value_and_Lock                     (False)

#Applying the BeamConstraint
InDetFlags.useBeamConstraint.set_Value_and_Lock                    (True)

# --- enable brem recovery
InDetFlags.doBremRecovery.set_Value_and_Lock                       (False)
InDetFlags.doCaloSeededBrem.set_Value_and_Lock                     (False)
# --- enable forward tracks
InDetFlags.doForwardTracks.set_Value_and_Lock                      (False)
# --- enable 
InDetFlags.doTrackSegmentsPixelPrdAssociation.set_Value_and_Lock   (False)
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

# Disable TRT only tracking  
InDetFlags.doTRTStandalone.set_Value_and_Lock                      (False)
InDetFlags.doBackTracking.set_Value_and_Lock                       (False)

# --- We are in commissioning phase in case of cosmics and comm
#if "cos" in loadInDetRec_Options["projectName"] or "comm" in loadInDetRec_Options["projectName"]:
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock         (False)
InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(True);
InDetFlags.doTIDE_Ambi.set_Value_and_Lock(True);

# --- Dynamic alignment 
InDetFlags.useDynamicAlignFolders = True



# necessary?
include ("InDetRecExample/InDetRecConditionsAccess.py")

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

## Override NN clustering 

#conddb.blockFolder("/PIXEL/PixelClustering/PixelClusNNCalib")
#conddb.addFolder("","<dbConnection>sqlite://;schema=/afs/cern.ch/user/p/pbutti/spacework/ErrorScaling_2016_Test/source/InnerDetector/InDetExample/InDetAlignExample/ErrorScaling/dataNN.db;dbname=CONDBR2</dbConnection> /PIXEL/PixelClustering/PixelClusNNCalib <tag>PixelClusNNCalib-DATA-RUN12-000-01</tag>", force=True);


## Override alignment folders

from IOVDbSvc.CondDB import conddb
#Repro Alignment Tags
conddb.addOverride("/Indet/AlignL1/ID","IndetAlignL1ID-Repro-Rel21-UPD3-00")
conddb.addOverride("/Indet/AlignL2/PIX","IndetAlignL2PIX-Repro-Rel21-UPD3-00")
conddb.addOverride("/Indet/AlignL2/SCT","IndetAlignL2SCT-Repro-Rel21-UPD3-00")
conddb.addOverride("/Indet/IBLDist","IndetIBLDist-Repro-Rel21-UPD3-00")
conddb.addOverride("/TRT/AlignL1/TRT","TRTAlignL1-Repro-Rel21-UPD3-00")
conddb.addOverride("/Indet/AlignL3","IndetAlignL3-Repro-Rel21-UPD3-00")
conddb.addOverride("/TRT/AlignL2","TRTAlignL2-Repro-Rel21-UPD3-00")


#### Monitoring stuff - to be put in a post-Include





#Override of the Error Scaling - to do: if change the dbnane. You need to pass the information. This line works only for data.
conddb.blockFolder("/Indet/TrkErrorScaling")
conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema=mycool.db;dbname=CONDBR2</dbConnection>/Indet/TrkErrorScaling',errorScalingOverride,True)


readESD=False
# do the monitoring file.
print "Setting up monitoring"
include("InDetMonitoringAlignment.py")
