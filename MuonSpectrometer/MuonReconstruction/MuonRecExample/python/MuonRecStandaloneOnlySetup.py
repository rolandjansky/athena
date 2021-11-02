# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """Set up the flags (for RecExCommon) so that only Muon Standalone Reconstruction is run.
It sets default values for muonRecFlags, recFlags, globalflags, DetFlags."""

from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec as recFlags
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

import os

# find floating point errors in standalone tests 
athenaCommonFlags.RuntimeStrictness = "abort"

#--------------------------------------------------------------
# Set default recFlags
#--------------------------------------------------------------
if os.environ.get('CMTCONFIG','').endswith('-dbg'):
    recFlags.doEdmMonitor       = True 
    recFlags.doNameAuditor      = True
else:
    recFlags.doEdmMonitor       = False
    recFlags.doNameAuditor      = False


recFlags.doMuon                 = True
recFlags.doInDet                = False
recFlags.doCalo                 = False
recFlags.doLArg                 = False
recFlags.doTile                 = False
recFlags.doMuonCombined         = False
recFlags.readESD                = False
recFlags.doESD                  = True
recFlags.doWriteESD             = False
recFlags.doWriteAOD             = False
recFlags.doWriteTAG             = False
recFlags.doJiveXML              = False
recFlags.doAOD                  = False
recFlags.doAODall               = False
recFlags.doPerfMon              = False
recFlags.doHeavyIon             = False
recFlags.doEgamma               = False
recFlags.doTau                  = False
recFlags.doJetMissingETTag      = False
#--------------------------------------------------------------
# Set default recAlgsFlags
#--------------------------------------------------------------
muonCombinedRecFlags.Enabled = False
recAlgsFlags.doTrackRecordFilter = True # filter truth muons getting into the spectrometer
recAlgsFlags.doMissingET        = False
recAlgsFlags.doMissingETSig     = False
recAlgsFlags.doObjMissingET     = False
recAlgsFlags.doEFlow            = False
recAlgsFlags.doEFlowJet         = False
recAlgsFlags.doAtlfast          = False
recAlgsFlags.doMuonSpShower     = False # needs JetRec
# disable JetRec (no RecFlags flag for that)
try:
    from JetRec.JetRecFlags import jetFlags
    jetFlags.Enabled = False
except ImportError:
    pass

# bug in RecExCond does not pass recFlags.doTau()
try:
    from tauRec.tauRecFlags import jobproperties
    jobproperties.tauRecFlags.Enabled = recFlags.doTau()
except ImportError:
    pass

#--------------------------------------------------------------
# Set default globalflags
#--------------------------------------------------------------
globalflags.DetGeo      = 'atlas'
globalflags.DataSource  = 'geant4'    
globalflags.InputFormat = 'pool'    
globalflags.Luminosity  = 'zero'

#--------------------------------------------------------------
# --- switch on only Muon Detector
#--------------------------------------------------------------
DetFlags.ID_setOff()
DetFlags.detdescr.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.detdescr.Calo_setOn()
DetFlags.Muon_setOn()

# no digitization
DetFlags.digitize.all_setOff()

if not recFlags.doTrigger():
    DetFlags.LVL1_setOff()

#--------------------------------------------------------------
# switch on writing of Full Calibration ntuple by default
#--------------------------------------------------------------
muonRecFlags.doCalibNtuple = False
muonRecFlags.calibNtupleSegments = False
muonRecFlags.calibNtupleTracks = False
muonRecFlags.calibNtupleTrigger = False

#--------------------------------------------------------------
# select Standalone reconstruction
#--------------------------------------------------------------
muonRecFlags.doStandalone = True

#--------------------------------------------------------------
# Muon DQA
#--------------------------------------------------------------
from AthenaMonitoring.DQMonFlags import DQMonFlags
# switch on all Muon DQA
DQMonFlags.doMuonRawMon=False
DQMonFlags.doMuonSegmentMon=False
DQMonFlags.doMuonTrackMon=False
DQMonFlags.doMuonAlignMon=False # crashes
DQMonFlags.doMuonPhysicsMon=False

# switch off everything else
DQMonFlags.doMuonCombinedMon=False
DQMonFlags.doGlobalMon=False
DQMonFlags.doLVL1CaloMon=False
DQMonFlags.doCTPMon=False
DQMonFlags.doHLTMon=False
DQMonFlags.doPixelMon=False
DQMonFlags.doSCTMon=False
DQMonFlags.doTRTMon=False
DQMonFlags.doTRTElectronMon=False
DQMonFlags.doInDetGlobalMon=False
DQMonFlags.doInDetAlignMon=False
DQMonFlags.doInDetPerfMon=False
DQMonFlags.doLArMon=False
DQMonFlags.doCaloMon=False
DQMonFlags.doTileMon=False
DQMonFlags.doEgammaMon=False
DQMonFlags.doMissingEtMon=False
DQMonFlags.doJetMon=False
DQMonFlags.doTauMon=False
DQMonFlags.doJetTagMon=False

