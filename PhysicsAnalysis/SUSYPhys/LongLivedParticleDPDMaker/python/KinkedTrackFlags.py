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

## TriggerAPI ##
from LongLivedParticleDPDMaker.RPVLLTriggers import RPVLLTriggers
apitriggers = RPVLLTriggers()

class KinkedTrack_containerFlags(JobProperty):
    statusOn = True
    jetCollectionName = "AntiKt4EMTopoJets"
    electronCollectionName = "Electrons"
    muonCollectionName = "Muons"
    METCollectionName = "MET_Reference_AntiKt4EMTopo"
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
        'HLT_xe70_mht',
        'HLT_xe80_L1XE50',
        'HLT_xe80_tc_lcw_L1XE50',
        'HLT_xe80_tc_em_L1XE50',
        'HLT_xe80_mht_L1XE50',
        'HLT_xe80_pueta_L1XE50',
        'HLT_xe80_pufit_L1XE50',
        'HLT_xe80_wEFMu_L1XE50',
        'HLT_xe80_tc_lcw_wEFMu_L1XE50',
        'HLT_xe80_tc_em_wEFMu_L1XE50',
        'HLT_xe80_mht_wEFMu_L1XE50',
        'HLT_xe80_pueta_wEFMu_L1XE50',
        'HLT_xe80_pufit_wEFMu_L1XE50',
        'HLT_xe80',
        'HLT_xe80_tc_lcw',
        'HLT_xe80_tc_em',
        'HLT_xe80_mht',
        'HLT_xe80_pueta',
        'HLT_xe80_pufit',
        'HLT_xe80_wEFMu',
        'HLT_xe80_tc_lcw_wEFMu',
        'HLT_xe80_tc_em_wEFMu',
        'HLT_xe80_mht_wEFMu',
        'HLT_xe80_pueta_wEFMu',
        'HLT_xe80_pufit_wEFMu',
        'HLT_xe80_L1XE70',
        'HLT_xe80_tc_lcw_L1XE70',
        'HLT_xe80_tc_em_L1XE70',
        'HLT_xe80_mht_L1XE70',
        'HLT_xe80_pueta_L1XE70',
        'HLT_xe80_pufit_L1XE70',
        'HLT_xe80_wEFMu_L1XE70',
        'HLT_xe80_tc_lcw_wEFMu_L1XE70',
        'HLT_xe80_tc_em_wEFMu_L1XE70',
        'HLT_xe80_mht_wEFMu_L1XE70',
        'HLT_xe80_pueta_wEFMu_L1XE70',
        'HLT_xe80_pufit_wEFMu_L1XE70',
        'HLT_xe90_L1XE50',
        'HLT_xe90_tc_lcw_L1XE50',
        'HLT_xe90_mht_L1XE50',
        'HLT_xe90_tc_em_L1XE50',
        'HLT_xe90_wEFMu_L1XE50',
        'HLT_xe90_tc_lcw_wEFMu_L1XE50',
        'HLT_xe90_mht_wEFMu_L1XE50',
        'HLT_xe90_tc_em_wEFMu_L1XE50',
        'HLT_xe90_pueta_L1XE50',
        'HLT_xe90_L1XE60',
        'HLT_xe90_mht',
        'HLT_xe90_mht_L1XE60',
        'HLT_xe90_mht_wEFMu',
        'HLT_xe90_pueta',
        'HLT_xe90_pueta_L1XE60',
        'HLT_xe90_pueta_wEFMu',
        'HLT_xe90_pufit',
        'HLT_xe90_pufit_L1XE50',
        'HLT_xe90_pufit_L1XE60',
        'HLT_xe90_pufit_wEFMu',
        'HLT_xe90_tc_em',
        'HLT_xe90_tc_em_wEFMu',
        'HLT_xe90_tc_lcw',
        'HLT_xe90_tc_lcw_L1XE60',
        'HLT_xe90_tc_lcw_wEFMu',
        'HLT_xe90_wEFMu',
        'HLT_xe100',
        'HLT_xe100_L1XE60',
        'HLT_xe100_mht_L1XE50',
        'HLT_xe100_mht_L1XE60',
        'HLT_xe100_tc_lcw_L1XE60',
        'HLT_xe100_L1XE50',
        'HLT_xe100_L1XE80',
        'HLT_xe100_tc_lcw',
        'HLT_xe100_tc_lcw_L1XE50',
        'HLT_xe100_tc_lcw_L1XE80',
        'HLT_xe100_tc_em',
        'HLT_xe100_tc_em_L1XE50',
        'HLT_xe100_tc_em_L1XE80',
        'HLT_xe100_mht',
        'HLT_xe100_mht_L1XE50',
        'HLT_xe100_mht_L1XE80',
        'HLT_xe100_pueta',
        'HLT_xe100_pueta_L1XE80',
        'HLT_xe100_pufit',
        'HLT_xe100_pufit_L1XE50',
        'HLT_xe100_pufit_L1XE55',
        'HLT_xe100_pufit_L1XE80',
        'HLT_xe100_wEFMu',
        'HLT_xe100_wEFMu_L1XE50',
        'HLT_xe100_wEFMu_L1XE80',
        'HLT_xe100_tc_lcw_wEFMu',
        'HLT_xe100_tc_lcw_wEFMu_L1XE50',
        'HLT_xe100_tc_lcw_wEFMu_L1XE80',
        'HLT_xe100_tc_em_wEFMu',
        'HLT_xe100_tc_em_wEFMu_L1XE50',
        'HLT_xe100_tc_em_wEFMu_L1XE80',
        'HLT_xe100_mht_wEFMu',
        'HLT_xe100_mht_wEFMu_L1XE50',
        'HLT_xe100_mht_wEFMu_L1XE80',
        'HLT_xe100_pueta_wEFMu',
        'HLT_xe100_pueta_wEFMu_L1XE80',
        'HLT_xe100_pufit_wEFMu',
        'HLT_xe100_pufit_wEFMu_L1XE80',
        'HLT_xe110_pueta_L1XE60',
        'HLT_xe110_pufit_L1XE50',
        'HLT_xe110_pufit_L1XE55',
        'HLT_xe110_pufit_L1XE60',
        'HLT_xe100_pufit_xe75_L1XE60',
        'HLT_xe110_pufit_xe70_L1XE50',
        'HLT_xe110_pufit_xe65_L1XE55',
        'HLT_xe110_pufit_xe65_L1XE60',
        'HLT_xe110_L1XE60',
        'HLT_xe110_mht_L1XE60',
        'HLT_xe110_mht_L1XE50',
        'HLT_xe110_mht_L1XE50_AND_xe70_L1XE50',
        'HLT_xe110_tc_lcw_L1XE60',
        'HLT_xe120',
        'HLT_xe120_mht',
        'HLT_xe120_pueta',
        'HLT_xe120_pufit',
        'HLT_xe120_tc_em',
        'HLT_xe120_tc_lcw',
        'HLT_xe120_wEFMu',
        'HLT_xe120_mht_wEFMu',
        'HLT_xe120_mht_L1XE60_xe80_L1XE60',
        'HLT_xe120_pueta_wEFMu',
        'HLT_xe120_pufit_wEFMu',
        'HLT_xe120_tc_em_wEFMu',
        'HLT_xe120_tc_lcw_wEFMu',
        'HLT_xe120_pufit_L1XE50',
        'HLT_xe120_pufit_L1XE55',
        'HLT_xe120_pufit_L1XE60',
        'HLT_xe130_mht_L1XE50'        
        ]
    if apitriggers.doTriggerAPI:
        triggerNames += apitriggers.getKinkedTrackJetMetTriggers() ## TriggerAPI
    cutsEtMin = [80.0*Units.GeV, 40.0*Units.GeV]
    cutsEtMinForStublet = [90.0*Units.GeV, 40.0*Units.GeV]
    cutsEtMinForMultiJets = [60.0*Units.GeV, 60.0*Units.GeV]
    jetMetPtMin = 40.0*Units.GeV
    cutMetHt = 0.0
    cutEtaMax = 3.2
    doLeptonVeto = True
    requireIsolatedTrack = True
    electronIDKey = "Tight"
    muonIDKey = "Medium"
    leptonPtMax = 20.0*Units.GeV
    leptonEtaMax = 2.5
    cutMetMin = 60.0*Units.GeV  # no cut
    cutMetMinForStublet = 90.0*Units.GeV  # no cut
    cutMetMinForMultiJets = 170.0*Units.GeV
    jetMetDphiMin = 1.0
    preScaleStublet = 10
    pass
primRPVLLDESDM.add_JobProperty(KinkedTrack_singleJetMetFilterFlags)

class KinkedTrack_ZeeFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = True
    triggerNames = [
        ## 0.3e34
        'HLT_e24_lhmedium_iloose_L1EM18VH',
        'HLT_e24_lhmedium_ivarloose_L1EM18VH',
        'HLT_e24_lhmedium_nod0_iloose_L1EM18VH',
        'HLT_e24_lhmedium_nod0_ivarloose_L1EM18VH',
        ## 0.5e34
        'HLT_e24_lhmedium_L1EM20VH',
        'HLT_e24_lhmedium_nod0_L1EM20VH',
        'HLT_e24_lhmedium_iloose',
        'HLT_e24_lhmedium_ivarloose',
        'HLT_e24_lhmedium_nod0_iloose',
        'HLT_e24_lhmedium_nod0_ivarloose',
        'HLT_e24_lhmedium_iloose_L1EM20VH',
        'HLT_e24_lhmedium_ivarloose_L1EM20VH',
        'HLT_e24_lhmedium_nod0_iloose_L1EM20VH',
        'HLT_e24_lhmedium_nod0_ivarloose_L1EM20VH',
        ## runs up to 1e34 :
        'HLT_e24_lhtight_iloose',
        'HLT_e24_lhtight_nod0_iloose',
        'HLT_e24_lhtight_ivarloose',
        'HLT_e24_lhtight_nod0_ivarloose',
        ##  runs up to 2e34 :
        'HLT_e26_lhtight_smooth_iloose',
        'HLT_e26_lhtight_smooth_ivarloose',
        'HLT_e26_lhtight_iloose',
        'HLT_e26_lhtight_ivarloose',
        'HLT_e26_lhtight_nod0_iloose',
        'HLT_e26_lhtight_nod0_ivarloose',
        'HLT_e28_lhtight_smooth_iloose',
        'HLT_e28_lhtight_smooth_ivarloose',
        'HLT_e28_lhtight_iloose',
        'HLT_e28_lhtight_ivarloose',
        'HLT_e28_lhtight_nod0_iloose',
        'HLT_e28_lhtight_nod0_ivarloose',
        ]
    if apitriggers.doTriggerAPI:
        triggerNames += apitriggers.getKinkedTrackZeeTriggers() ## TriggerAPI
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
        ## 0.5e34
        'HLT_mu20_iloose_L1MU15',
        'HLT_mu20_ivarloose_L1MU15',
        ## 1.0e34
        'HLT_mu24_iloose_L1MU15',
        'HLT_mu24_ivarloose_L1MU15',
        ## 1.5e34
        'HLT_mu24_imedium',
        'HLT_mu24_ivarmedium',
        ## 2e34
        'HLT_mu26_imedium',
        'HLT_mu26_ivarmedium',
        ]
    if apitriggers.doTriggerAPI:
        triggerNames += apitriggers.getKinkedTrackZmumuTriggers() ## TriggerAPI
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


