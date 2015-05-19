# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ********************* WARNING **********************
#
# In order to preserve backward compatibility, please 
#                  !!!!DO NOT!!!
# modify an existing key, simply add the new
# one after the old one.
# If old one is not going to be needed leave the destionations tag list empty.
# (there are examples below)
# 
# This is to ensure we will be able to read datasets
# produced with older versions of the reconstruction program.
#
# ****************************************************

#type/key destination tag (this tag is configure in JO and selects the objects whcih go to ESD AOD (prefix)
# (note that for L2 order matters, there might be not enough space for this at the end of the list so add there low priority objects)

# In Trigger*List, 3rd field holds the category of the trigger container
# Current categories
#	Bjet
#	Bphys
#	Config
#	Cosmics
#	Egamma
#	ID
#	Jet
#	L1
#	MET
#	MinBias
#	Muon
#	Steer
#	Tau
#	Calo       <-- new
#	Tracking   <-- new
#	Combined   <-- new
#	Unknown    <-- should be temporary until you figure out what to classify it as
#	Other?...maybe inDet(ID?), Truth

from TrigEDMConfig.TriggerEDMRun1 import TriggerL2List,TriggerEFList,TriggerResultsRun1List

#####################
#####################
# Container Slimming
#
# Here are the definitions of the strings which will be used to slim containers
# String to separate EDM members
identifier = ".-"

# ID Triggers
# ID Variables to be slimmed away
UnusedIDVariables = ["trackParameterCovarianceMatrices", "parameterX", "parameterY", "parameterZ", "parameterPX",
                     "parameterPY", "parameterPZ", "parameterPosition"]

# Combine them into a string
RemoveIDVariables = ".-"+identifier.join(UnusedIDVariables)

# Tau Triggers
# Tau Trigger Variables to be slimmed away
PanTauVars = [ "pantau_CellBasedInput_isPanTauCandidate", "pantau_CellBasedInput_DecayModeProto", "pantau_CellBasedInput_DecayMode",
               "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n", "pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn",
               "pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn", "pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts",
               "pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt", "pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts",
               "pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM", "pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1",
               "pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2", "pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts",
               "pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts", "pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed",
               "pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged", "pantau_CellBasedInput_BDTVar_Charged_HLV_SumM",
               "ptPanTauCellBasedProto", "etaPanTauCellBasedProto", "phiPanTauCellBasedProto", "mPanTauCellBasedProto", "ptPanTauCellBased",
               "etaPanTauCellBased", "phiPanTauCellBased", "mPanTauCellBased" ]

PFOs = ["chargedPFOLinks", "neutralPFOLinks", "pi0PFOLinks", "protoChargedPFOLinks", "protoNeutralPFOLinks", "protoPi0PFOLinks",
        "shotPFOLinks", "hadronicPFOLinks" ]

EFlow = ["etEflow", "mEflow", "nPi0", "nCharged", "etEflowTopo", "mEflowTopo", "ptRatioEflowTopo", "nPi0Topo", "nChargedTopo",
         "ptRatioEflow", "pi0ConeDR" ]

Samplings = ["cellBasedEnergyRing1", "cellBasedEnergyRing2", "cellBasedEnergyRing3", "cellBasedEnergyRing4", "cellBasedEnergyRing5",
             "cellBasedEnergyRing6", "cellBasedEnergyRing7", "PSSFraction", "ele_E237E277", "ele_PresamplerFraction",
             "ele_ECALFirstFraction", "TRT_NHT_OVER_NLT" ]

UnusedOldTau = ["ipZ0SinThetaSigLeadTrk", "trkWidth2", "numEffTopoClusters", "topoInvMass",
                "effTopoInvMass", "topoMeanDeltaR", "effTopoMeanDeltaR", "isolFrac", "stripWidth2",
                "nStrip", "seedCalo_eta", "seedCalo_phi", "trkAvgDist", "trkRmsDist"
                "lead2ClusterEOverAllClusterE", "lead3ClusterEOverAllClusterE", "secMaxStripEt",
                "sumEMCellEtOverLeadTrkPt", "hadLeakEt", "caloIso", "caloIsoCorrected" ]

UnusedProperties = ["Likelihood", "SafeLikelihood", "BDTEleScore", "BDTJetScoreSigTrans", "BDTJetScoreBkgTrans",
                    "vetoFlags", "isTauFlags", "trackFlags", "trackFilterProngs", "trackFilterQuality", "trackEtaStrip", "trackPhiStrip",
                    "TauJetVtxFraction" ]

UnusedFourMom = ["ptIntermediateAxis", "etaIntermediateAxis", "phiIntermediateAxis", "mIntermediateAxis",
                 "ptTauEtaCalib", "etaTauEtaCalib", "phiTauEtaCalib", "mTauEtaCalib", "EM_TES_scale"]

# Combine them into a string
UnusedTauVariables = PanTauVars + PFOs + EFlow + Samplings + UnusedOldTau + UnusedProperties + UnusedFourMom
RemoveTauVariables = ".-"+identifier.join(UnusedTauVariables)


# Done Slimming
#####################
#####################

TriggerHLTList = [
 
    #egamma
    ('xAOD::ElectronContainer#HLT_egamma_Electrons',                'BS ESD AODFULL AODSLIM', 'Egamma'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_ElectronsAux.',         'BS ESD AODFULL AODSLIM', 'Egamma'), 
    ('xAOD::PhotonContainer#HLT_egamma_Photons',                    'BS ESD AODFULL AODSLIM', 'Egamma'),
    ('xAOD::PhotonAuxContainer#HLT_egamma_PhotonsAux.',             'BS ESD AODFULL AODSLIM', 'Egamma'), 

    #bphys
    ('xAOD::TrigBphysContainer#HLT_EFBMuMuFex',                               'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_EFBMuMuFexAux.',                        'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer#HLT_EFBMuMuXFex',                              'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_EFBMuMuXFexAux.',                       'BS ESD AODFULL AODSLIM',  'Bphys'),

    ('xAOD::TrigBphysContainer#HLT_EFTrackMass',                              'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_EFTrackMassAux.',                       'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer#HLT_EFMultiMuFex',                             'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_EFMultiMuFexAux.',                      'BS ESD AODFULL AODSLIM',  'Bphys'),

    ('xAOD::TrigBphysContainer#HLT_L2BMuMuXFex',                                 'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_L2BMuMuXFexAux.',                          'BS ESD AODFULL AODSLIM', 'Bphys'),

    #minbias
    # MinBias algs attach single objects therefore the pattern is slightly different, note also changes in the EDMList which define feature -> collection mapping
    ('xAOD::TrigSpacePointCounts#HLT_spacepoints',			         'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigSpacePointCountsAuxContainer#HLT_spacepointsAux.',		 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigT2MbtsBits#HLT_T2Mbts',					         'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigT2MbtsBitsAuxContainer#HLT_T2MbtsAux.',				 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigVertexCounts#HLT_vertexcounts',			     		 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigVertexCountsAuxContainer#HLT_vertexcountsAux.',			 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigTrackCounts#HLT_trackcounts',			       		 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigTrackCountsAuxContainer#HLT_trackcountsAux.',			 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigT2ZdcSignals#HLT_zdcsignals',                                    'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigT2ZdcSignalsAuxContainer#HLT_zdcsignalsAux.',                    'BS ESD AODFULL AODSLIM',  'MinBias'),

    #taus
    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged',                                 'BS ESD AODFULL AODSLIM', 'Tau'), 
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMergedAux'+RemoveTauVariables,        'BS ESD AODFULL AODSLIM', 'Tau'), 

    #MET
    ('xAOD::TrigMissingETContainer#HLT_TrigEFMissingET',                                 'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_TrigEFMissingETAux.',                          'BS ESD AODFULL AODSLIM',                'MET'),

    #tracking
    #fp
    ('xAOD::TrackParticleContainer#HLT_InDetTrigTrackingxAODCnv_Muon_EFID',             'BS ESD AODFULL AODSLIM',  'Muon'),

    ('xAOD::TrackParticleContainer#HLT_InDetTrigTrackingxAODCnv_Bphysics_IDTrig',      'BS ESD AODFULL AODSLIM',  'Bphys'),

    ('xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Muon_EFIDAux.-caloExtension'+RemoveIDVariables,             'BS ESD AODFULL AODSLIM',  'Muon'),

    ('xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Muon_FTFAux'+RemoveIDVariables,              'BS ESD AODFULL AODSLIM',  'Muon'),
    ('xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Bphysics_IDTrigAux'+RemoveIDVariables,          'BS ESD AODFULL AODSLIM',  'Bphys'),

    #jets
    # antiKt1.0
    # em  clusters Full Scan
    ('xAOD::JetContainer#HLT_a10tcemnojcalibFS',                         'BS ESD AODFULL AODSLIM', 'Jet'),
    ('xAOD::JetTrigAuxContainer#HLT_a10tcemnojcalibFSAux.',              'BS ESD AODFULL AODSLIM', 'Jet'),
    
    #btagging
    ('xAOD::BTaggingContainer#HLT_HLTBjetFex',                           'BS ESD AODFULL AODSLIM', 'Bjet'),
    ('xAOD::BTaggingAuxContainer#HLT_HLTBjetFexAux.',                    'BS ESD AODFULL AODSLIM', 'Bjet'),

    # b-jets  
    ('xAOD::JetContainer#HLT_EFJet',                                     'BS ESD AODFULL AODSLIM', 'Bjet'),
    ('xAOD::JetTrigAuxContainer#HLT_EFJetAux.',                          'BS ESD AODFULL AODSLIM', 'Bjet'),
    ('xAOD::JetContainer#HLT_SplitJet',                                  'BS ESD AODFULL AODSLIM', 'Bjet'),
    ('xAOD::JetTrigAuxContainer#HLT_SplitJetAux.',                       'BS ESD AODFULL AODSLIM', 'Bjet'),

    # start of L2+EF list

    # particles == L2 List
    ('TrigT2MbtsBits#HLT_T2Mbts',                                         'BS ESD AODFULL AODSLIM', 'MinBias'),
    ('TrigSpacePointCounts#HLT_spacepoints',                              'BS ESD AODFULL AODSLIM', 'MinBias'), # ? do we realy need it!

    # == EF List
    ('TrigTrackCounts#HLT_trackcounts',                                   'BS ESD AODFULL AODSLIM',  'MinBias'), #? this is the CMS trigger obj
    ('TrigVertexCounts#HLT_vertexcounts',                                 'BS ESD AODFULL AODSLIM',  'MinBias'),

    # == end of L2+EF lists

    ]

TriggerResultsList=[
    ('xAOD::TrigDecision#xTrigDecision' ,             'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',   'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys#TrigConfKeys' ,              'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigNavigation#TrigNavigation' ,          'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigNavigationAuxInfo#TrigNavigationAux.','ESD AODFULL AODSLIM', 'Steer')
    ]


TriggerLvl1List=[
    ('xAOD::MuonRoIContainer#LVL1MuonRoIs' ,          'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.' ,   'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::EnergySumRoI#LVL1EnergySumRoI' ,          'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.','ESD AODFULL AODSLIM', 'L1'),
    ]



TriggerIDTruth= [
    ('TrigInDetTrackTruthMap#TrigInDetTrackTruthMap', 'ESD AODFULL AODSLIM', 'Unknown')
    ]

