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

class KinkedTrack_do2010Skim(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_do2010Skim)

class KinkedTrack_do2011Skim(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_do2011Skim)

class KinkedTrack_triggerFilterFlags2010(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = []
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = [
        "EF_j75_jetNoEF_EFxe25_noMu",
        "EF_xe55_noMu"
    ]
    MuonsTriggerNames = []
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_triggerFilterFlags2010)

class KinkedTrack_triggerFilterFlags2011(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = []
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = [
        #"EF_j75_a4_EFFS_xe45_loose_noMu",
        #"EF_j75_a4_EFFS_xe55_loose_noMu",
        #"EF_j75_a4tc_EFFS_xe45_loose_noMu",
        #"EF_j75_a4tc_EFFS_xe55_loose_noMu",
        #"EF_j75_a4tc_EFFS_xe55_noMu",
        #"EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10",
        #"EF_j65_a4tc_EFFS_xe65_medium_noMu_dphi2j30xe10",
        #"EF_j70_a4tc_EFFS_xe70_medium_noMu_dphi2j30xe10",
        #"EF_j80_a4tc_EFFS_xe60_noMu",
        "EF_j80_a4tchad_xe100_tclcw",
        "EF_j80_a4tchad_xe100_tclcw_loose",
        "EF_j80_a4tchad_xe100_tclcw_veryloose",
        "EF_j80_a4tchad_xe80_tclcw",
        "EF_j110_a4tchad_xe90_tclcw_loose",
        "EF_j110_a4tchad_xe90_tclcw_veryloose",
        "EF_j110_a4tchad_xe100_tclcw",
        "EF_j110_a4tchad_xe100_tclcw_veryloose",
        "EF_j110_a4tchad_xe75_tclcw",    
        "EF_j170_a4tchad_EFxe70_tclcw",
        "EF_j170_a4tchad_EFxe80_tclcw",
        "EF_j80_a4tchad_xe70_tclcw_dphi2j45xe10",
        "EF_j80_a4tchad_xe85_tclcw_dphi2j45xe10"
        ]
    MuonsTriggerNames = []
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_triggerFilterFlags2011)


class KinkedTrack_singleJetMetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    jetCollectionName = "AntiKt4LCTopoJets"
    cutEt1Min = 85.0*Units.GeV
    cutEta1Max = 3.2
    cutEt2Min = 50.0*Units.GeV
    cutEta2Max = 3.2
    doSecondJetVeto = False
    requireMet = True
    metCollectionNames = ["MET_LocHadTopo","MET_RefFinal","MET_RefFinal_em"]
    cutMetMin = 85.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_singleJetMetFilterFlags)


class KinkedTrack_multiJetMetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    jetCollectionType = "JetCollection"
    jetCollectionName = "AntiKt4LCTopoJets"
    cutsEtMin = [ 100.0*Units.GeV, 50.0*Units.GeV, 50.0*Units.GeV ]
    cutEtaMax = 3.2
    doSecondJetVeto = False
    requireMet = True
    metCollectionType = "MissingET"
    metCollectionNames = ["MET_LocHadTopo","MET_RefFinal","MET_RefFinal_em"]
    cutMetMin = 100.0*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_multiJetMetFilterFlags)
