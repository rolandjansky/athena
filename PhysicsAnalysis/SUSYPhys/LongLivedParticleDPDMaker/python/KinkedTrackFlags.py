# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        KinkedTrackFlags.py
## Authors:     Shimpei Yamamoto
## Email:       shimpei.yamamoto@cern.ch
## Created:     Febrary 2011
##
## Description: Here, all necessary job flags for the RPV/LL ESD-based DPD
##              are defined.
##
##=============================================================================

__doc__ = """job flags for the RPV/LL KinkedTrack DESD"""
__version__ = "0.0.1"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class KinkedTrack_containerFlags(JobProperty):
    statusOn = True
    jetCollectionName = "AntiKt4LCTopoJets"
    electronCollectionName = "Electrons"
    muonCollectionName = "Muons"
    METCollectionName = "MET_Reference_AntiKt4LCTopo"
    METTermName = "FinalTrk" # or "FinalClus" ??
    egClusterCollectionName = "egammaClusters"
    msTrackCollectionName = "MuonSpectrometerTrackParticles"
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_containerFlags)

class KinkedTrack_singleJetMetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    triggerNames = [
        "HLT_j80_xe80_dphi10_L1J40_DPHI-J20s2XE50",
        "HLT_j80_xe80_dphi10_L1J40_DPHI-J20XE50",
        "HLT_j100_xe80_dphi10_L1J40_DPHI-J20s2XE50",
        "HLT_j100_xe80_dphi10_L1J40_DPHI-J20XE50",
        "HLT_j80_xe80",
        "HLT_j100_xe80"
        ]
    cutsEtMin = [120.0*Units.GeV, 60.0*Units.GeV]
    cutEtaMax = 3.2
    doLeptonVeto = True
    electronIDKey = "Tight"
    muonIDKey = "Medium"
    leptonPtMax = 20.0*Units.GeV
    leptonEtaMax = 2.5
    cutMetMin = -1  # no cut
    jetMetDphiMin = 1.0
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_singleJetMetFilterFlags)

class KinkedTrack_ZeeFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    triggerNames = [
        #"HLT_e24_medium_iloose",
        #"HLT_e24_tight_iloose",
        #"HLT_e26_tight_iloose",
        #"HLT_e28_tight_iloose",
        "HLT_e24_medium_iloose_L1EM18VH",
        "HLT_e24_lhmedium_iloose_L1EM18VH",
        "HLT_e24_medium_iloose_L1EM20VH",
        "HLT_e24_lhmedium_iloose_L1EM20VH",
        "HLT_e24_tight_iloose_L1EM20VH",
        "HLT_e24_lhtight_iloose_L1EM20VH",
        "HLT_e26_tight_iloose",
        "HLT_e26_lhtight_iloose"
        ]
    doTriggerMatch = False
    electronPtMin = 40*Units.GeV
    electronEtaMax = 2.5
    electronIDKeys = ["Tight"]
    clusterEtMin = 15*Units.GeV
    clusterEtaMax = 2.5
    diElectronMassLow = (91.1876-40)*Units.GeV
    diElectronMassHigh = (91.1876+40)*Units.GeV
    deltaPhiMax = 1.5
    preScaleClusterEtMax = 60.0*Units.GeV
    preScale = 5
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_ZeeFilterFlags)

class KinkedTrack_ZmumuFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    triggerNames = [
        "HLT_mu20_iloose",
        "HLT_mu20_iloose_L1MU15",
        "HLT_mu24_iloose",
        "HLT_mu24_iloose_L1MU15",
        "HLT_mu24_imedium",
        "HLT_mu26_imedium"
        ]
    doTriggerMatch = False
    muonPtMin = 40*Units.GeV
    muonEtaMax = 2.5
    muonIDKeys = ["Medium"]
    trackPtMin = 15*Units.GeV
    trackEtaMax = 2.5
    diMuonMassLow = (91.1876-40)*Units.GeV
    diMuonMassHigh = (91.1876+40)*Units.GeV
    deltaPhiMax = 1.5
    preScaleTrackPtMax = 60.0*Units.GeV
    preScale = 5
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_ZmumuFilterFlags)


