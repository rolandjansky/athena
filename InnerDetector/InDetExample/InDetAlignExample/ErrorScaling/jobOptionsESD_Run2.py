# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration



#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs


rec.AutoConfiguration=['everything']

print athenaCommonFlags.FilesInput()

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2015-08")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-03-01-00")
globalflags.DataSource.set_Value_and_Lock("data")

import MagFieldServices.SetupField

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag(globalflags.ConditionsTag())


# --- number of events to process
athenaCommonFlags.EvtMax     = 50
athenaCommonFlags.SkipEvents = 0


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
DetFlags.BField_setOn()


#--------------------------------------------------------------
# Control
#--------------------------------------------------------------
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
rec.OutputLevel.set_Value_and_Lock       (INFO)

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

# --- turn off combined reconstruction
rec.doEgamma.set_Value_and_Lock          (False)
rec.doMuonCombined.set_Value_and_Lock    (False)
rec.doTau.set_Value_and_Lock             (False)
rec.doJetMissingETTag.set_Value_and_Lock (False)

# --- turn off global monitoring 
rec.doMonitoring.set_Value_and_Lock      (False)


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
rec.doTagRawSummary.set_Value_and_Lock   (False)



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

if datasample.getFormat()=='ESD': 
    InDetFlags.doRefit=True

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


InDetFlags.cutLevel.set_Value_and_Lock                             (12) 
InDetFlags.doTRTStandalone.set_Value_and_Lock                      (False)


InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock         (False)
InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False);

#NN clustering for pixel
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(True);
InDetFlags.doTIDE_Ambi.set_Value_and_Lock(True);



# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")

conddb.blockFolder("/Indet/TrkErrorScaling")
conddb.addFolderWithTag('','<dbConnection>sqlite://X;schema=mycool.db;dbname=CONDBR2</dbConnection>/Indet/TrkErrorScaling',errorScalingOverride,True);
#conddb.addOverride('/Indet/Beampos','IndetBeampos-data15_13TeV-ReproJun15-v0')


conddb.blockFolder("/Indet/Align")
conddb.blockFolder("/TRT/Align")
from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += CondProxyProvider()
ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
# set this to the file containing AlignableTransform objects
ServiceMgr.CondProxyProvider.InputCollections = ["/afs/cern.ch/user/m/mdanning/hias/public/13TeV/20.1.5.8/run_DCSfix/Iter3/Iter3_AlignmentConstants.root"]
print "INPUT POOL FILES COLLECTION", ServiceMgr.CondProxyProvider.InputCollections 
ServiceMgr.CondProxyProvider.OutputLevel = DEBUG
print ServiceMgr.CondProxyProvider
# this preload causes callbacks for read in objects to be activated,
# allowing GeoModel to pick up the transforms
ServiceMgr.IOVSvc.preLoadData = True
ServiceMgr.IOVSvc.OutputLevel = INFO


include("InDetMonitoringAlignment.py")


