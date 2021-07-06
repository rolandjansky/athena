# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class SmpCaloId_triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    TriggerNames = [] # only put trigger names in this list if you want to override the below stream-specific ones
    EgammaTriggerNames = []
    JetTauEtmissTriggerNames = [
        "EF_tau125_medium1",
        "EF_2b35_loose_j145_j35_a4tchad",
        "EF_j360_a4tchad",
        "EF_xe80_tclcw"
        # "EF_xe60_noMu",
        # "EF_xe60_tight_noMu",
        # "EF_xe60_verytight_noMu",
        # "EF_xe70_noMu",
        # "EF_xe70_tight_noMu",
        # "EF_xe70_verytight_noMu",
        # "EF_xe80_noMu",
        # "EF_xe80_tight_noMu",
        # "EF_xe80_verytight_noMu",
        # "EF_xe90_noMu",
        # "EF_xe90_tight_noMu",
        # "EF_xe90_verytight_noMu",
        # "EF_j75_a4_EFFS_xe55_loose_noMu",
        # "EF_j75_a4tc_EFFS_xe55_noMu",
        # "EF_j55_a4tc_EFFS_xe55_medium_noMu_dphi2j30xe10",
        # "EF_j80_a4tchad_xe100_tclcw",
        # "EF_j80_a4tchad_xe100_tclcw_loose",
        # "EF_j80_a4tchad_xe100_tclcw_veryloose",
        # "EF_j80_a4tchad_xe80_tclcw",
        # "EF_j110_a4tchad_xe90_tclcw_loose",
        # "EF_j110_a4tchad_xe90_tclcw_veryloose",
        # "EF_j110_a4tchad_xe100_tclcw",
        # "EF_j110_a4tchad_xe100_tclcw",
        # "EF_j110_a4tchad_xe75_tclcw",    
        # "EF_j170_a4tchad_EFxe70_tclcw",
        # "EF_j170_a4tchad_EFxe80_tclcw",
        # "EF_j80_a4tchad_xe70_tclcw_dphi2j45xe10",
        # "EF_j80_a4tchad_xe85_tclcw_dphi2j45xe10",
        # "EF_xe70",
###        "EF_xe80",
###        "EF_xe90",
###        "EF_xe100",
###        "EF_xe75_tclcw",
###        "EF_xe80_tclcw",
###        "EF_xe80_tclcw_loose",
###        "EF_xe80T_tclcw_loose",
###        "EF_xe80_tclcw_tight",
###        "EF_xe80_tclcw_verytight",
###        "EF_xe90_tclcw",
###        "EF_xe90_tclcw_tight",
###        "EF_xe90_tclcw_verytight",
###        "EF_xe100_tclcw_loose",
###        "EF_xe100_tclcw",
###        "EF_xe100_tclcw_verytight",
###        "EF_j360_a4tchad"
    ]
    MuonsTriggerNames = [
        "EF_mu36_tight",
        "EF_mu24i_tight",
        "EF_mu40_slow_tight"
    ]
    HadDelayedTriggerNames = [
        "EF_xe60_tclcw_loose_delayed",
        "EF_xe60T_tclcw_loose_delayed"
    ]
primRPVLLDESDM.add_JobProperty(SmpCaloId_triggerFilterFlags)

class SmpCaloId_muonFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutEtMin = 60 * Units.GeV
    cutContainerMu = 'all'
    usingAOD = False
primRPVLLDESDM.add_JobProperty(SmpCaloId_muonFilterFlags)

class SmpCaloId_trackParticleFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    cutPtMin                = 80 * Units.GeV
    cutBLayerHitsMin        = 0
    cutPixelHitsMin         = 3
    cutSCTHitsMin           = 6
    cutSiliconHitsMin       = 0
    cutInputCollection      = 'InDetTrackParticles'
    cutOutputLinkCollection = 'SmpId_TrackParticleLinkCollection'
    usingAOD = False
primRPVLLDESDM.add_JobProperty(SmpCaloId_trackParticleFilterFlags)
