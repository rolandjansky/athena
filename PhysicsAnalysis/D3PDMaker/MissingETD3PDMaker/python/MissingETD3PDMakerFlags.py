# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file MissingETD3PDMaker/python/MissingETD3PDMakerFlags.py
# @author Jet Goodson <jgoodson@cern.ch>
# @date 23 Sep, 2009
# @brief Common flags for MissingETD3PD making.
# @modelled directly on the flag file in D3PDMakerConfig.
#


"""Common flags for D3PD making.
"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


class MissingETD3PDMakerFlags (JobPropertyContainer):
    """Common flags for MissingETD3PD making.
"""
    pass


jobproperties.add_Container (MissingETD3PDMakerFlags)


# Helper to create a SG key property.
def _sgkey_prop (name, val):
    prop = type (name, (JobProperty,), {})
    prop.statusOn = True
    prop.allowedTypes = ['str']
    prop.StoredValue = val
    jobproperties.MissingETD3PDMakerFlags.add_JobProperty(prop)
    return

_sgkey_prop ('METCompositionSGKey', 'MET_RefComposition')
_sgkey_prop ('METRefFinalSGKey', 'MET_RefFinal')
_sgkey_prop ('METMuonSGKey', 'MET_Muon')
_sgkey_prop ('METMuonIsolStacoSGKey', 'MET_Muon_Isol_Staco')#
_sgkey_prop ('METMuonNonIsolStacoSGKey', 'MET_Muon_NonIsol_Staco')#
_sgkey_prop ('METMuonTotalStacoSGKey', 'MET_Muon_Total_Staco')#
_sgkey_prop ('METMuonIsolMuidSGKey', 'MET_Muon_Isol_Muid')#
_sgkey_prop ('METMuonNonIsolMuidSGKey', 'MET_Muon_NonIsol_Muid')#
_sgkey_prop ('METMuonTotalMuidSGKey', 'MET_Muon_Total_Muid')#
_sgkey_prop ('METMuonsIsolSGKey', 'MET_Muons_Isol')# 3rd chain
_sgkey_prop ('METMuonsNonIsolSGKey', 'MET_Muons_NonIsol')# 3rd chain
_sgkey_prop ('METMuonsTotalSGKey', 'MET_Muons_Total')# 3rd chain
_sgkey_prop ('METMuonBoySGKey', 'MET_MuonBoy')
_sgkey_prop ('METFinalSGKey', 'MET_Final')
_sgkey_prop ('METCryoSGKey', 'MET_Cryo')
_sgkey_prop ('METCryoConeSGKey', 'MET_CryoCone')
_sgkey_prop ('METRefEleSGKey', 'MET_RefEle')
_sgkey_prop ('METRefJetSGKey', 'MET_RefJet')
_sgkey_prop ('METSoftJetsSGKey', 'MET_SoftJets')#
_sgkey_prop ('METRefMuonSGKey', 'MET_RefMuon')
_sgkey_prop ('METRefMuonStacoSGKey', 'MET_RefMuon_Staco')#
_sgkey_prop ('METRefMuonMuidSGKey', 'MET_RefMuon_Muid')#
_sgkey_prop ('METRefMuonsSGKey', 'MET_RefMuons')# 3rd chain
_sgkey_prop ('METRefGammaSGKey', 'MET_RefGamma')
_sgkey_prop ('METRefTauSGKey', 'MET_RefTau')
_sgkey_prop ('METCellOutSGKey', 'MET_CellOut')
_sgkey_prop ('METCellOutEflowSGKey', 'MET_CellOut_Eflow')#
_sgkey_prop ('METCellOutEflowMuidSGKey', 'MET_CellOut_Eflow_Muid')#
_sgkey_prop ('METCellOutEflowMuonsSGKey', 'MET_CellOut_Eflow_Muons*')#
_sgkey_prop ('METBaseSGKey', 'MET_Base')
_sgkey_prop ('METCalibSGKey', 'MET_Calib')
_sgkey_prop ('METTruthSGKey', 'MET_Truth')
_sgkey_prop ('METMuonBoySpectroSGKey', 'MET_MuonBoy_Spectro')
_sgkey_prop ('METMuonBoyTrackSGKey', 'MET_MuonBoy_Track')
_sgkey_prop ('METRefMuonTrackSGKey', 'MET_RefMuon_Track')
_sgkey_prop ('METRefMuonTrackStacoSGKey', 'MET_RefMuon_Track_Staco')#
_sgkey_prop ('METRefMuonTrackMuidSGKey', 'MET_RefMuon_Track_Muid')#
_sgkey_prop ('METRefMuonsTrackSGKey', 'MET_RefMuons_Track')# 3rd chain
_sgkey_prop ('METTrackSGKey', 'MET_Track')
_sgkey_prop ('METCellOutMiniJetSGKey', 'MET_CellOut_MiniJet')
_sgkey_prop ('METBase0SGKey', 'MET_Base0')
_sgkey_prop ('METTopoSGKey', 'MET_Topo')
_sgkey_prop ('METCorrTopoSGKey', 'MET_CorrTopo')
_sgkey_prop ('METLocHadTopoSGKey', 'MET_LocHadTopo')
_sgkey_prop ('METLocHadTopoObjSGKey', 'MET_LocHadTopoObj')
_sgkey_prop ('METTopoObjSGKey', 'MET_TopoObj')
_sgkey_prop ('METDMCryoSGKey', 'MET_DM_Cryo')
_sgkey_prop ('METDMCrack1SGKey', 'MET_DM_Crack1')
_sgkey_prop ('METDMCrack2SGKey', 'MET_DM_Crack2')
_sgkey_prop ('METDMAllSGKey', 'MET_DM_All')
_sgkey_prop ('METMuonMuidSGKey', 'MET_MuonMuid')
_sgkey_prop ('METMuidSGKey', 'MET_Muid')
_sgkey_prop ('METMuidSpectroSGKey', 'MET_Muid_Spectro')
_sgkey_prop ('METMuidTrackSGKey', 'MET_Muid_Track')

_sgkey_prop ('METMuonMuonsSGKey', 'MET_MuonMuons') # 3rd chain
_sgkey_prop ('METMuonsSGKey', 'MET_Muons') # 3rd chain
_sgkey_prop ('METMuonsSpectroSGKey', 'MET_Muons_Spectro') # 3rd chain
_sgkey_prop ('METMuonsTrackSGKey', 'MET_Muons_Track') # 3rd chain

_sgkey_prop ('METTruthPileUpSGKey', 'MET_Truth_PileUp')
_sgkey_prop ('AllCaloCellsSGKey',  'AllCalo') ##change to AllCalo for ESD use AODCellContainer for AOD
_sgkey_prop ('METDefaultJetCollectionSGKey', 'AntiKt4LCTopoJets,Cone4H1TopoJets') ##probably want to key to default later
_sgkey_prop ('METDefaultTrackCollectionSGKey', 'TrackParticleCandidate') ##probably want to key to default later
_sgkey_prop ('METRefFinalEMSGKey', 'MET_RefFinal_em')
_sgkey_prop ('METRefEleEMSGKey', 'MET_RefEle_em')
_sgkey_prop ('METRefJetEMSGKey', 'MET_RefJet_em')
_sgkey_prop ('METSoftJetsEMSGKey', 'MET_SoftJets_em')#
_sgkey_prop ('METRefMuonEMSGKey', 'MET_RefMuon_em')
_sgkey_prop ('METRefMuonTrackEMSGKey', 'MET_RefMuon_Track_em')
_sgkey_prop ('METRefGammaEMSGKey', 'MET_RefGamma_em')
_sgkey_prop ('METRefTauEMSGKey', 'MET_RefTau_em')
_sgkey_prop ('METCellOutEMSGKey', 'MET_CellOut_em')

_sgkey_prop ('METRefJetJVFSGKey','MET_RefJet_JVF')
_sgkey_prop ('METRefJetJVFCutSGKey','MET_RefJet_JVFCut')
_sgkey_prop ('METCellOutEflowSTVFSGKey','MET_CellOut_Eflow_STVF')
_sgkey_prop ('METCellOutEflowJetAreaSGKey','MET_CellOut_Eflow_JetArea')
_sgkey_prop ('METCellOutEflowJetAreaJVFSGKey','MET_CellOut_Eflow_JetAreaJVF')
_sgkey_prop ('METCellOutEflowJetAreaRhoEta5JVFSGKey','MET_CellOut_Eflow_JetAreaRhoEta5JVF')
_sgkey_prop ('METRefFinalSTVFSGKey','MET_RefFinal_STVF')

#Prefixes for object association
_sgkey_prop ('METStacoPrefix', 'mu_staco_MET_')
_sgkey_prop ('METMuidPrefix', 'mu_muid_MET_')
_sgkey_prop ('METMuonsPrefix', 'mu_MET_')
_sgkey_prop ('METPhotonPrefix', 'ph_MET_')
_sgkey_prop ('METElectronPrefix', 'el_MET_')
_sgkey_prop ('METTauPrefix', 'tau_MET_')
_sgkey_prop ('METClusterPrefix', 'cl_MET_')
_sgkey_prop ('METTrackPrefix', 'trk_MET_')
_sgkey_prop ('METDefaultJetPrefix', 'jet_antikt4LCtopo_MET_')
_sgkey_prop ('METDefaultTrackPrefix', 'trk_MET_')

##########Trigger Flags
_sgkey_prop ('METL1SGKey' , 'LVL1_ROI')
_sgkey_prop ('METL2SGKey' , 'HLT_T2MissingET')
_sgkey_prop ('METEFSGKey' , 'HLT_TrigEFMissingET')
_sgkey_prop ('METEFNoiseSGKey' , 'HLT_TrigEFMissingET_noiseSupp')
_sgkey_prop ('METEFFEBSGKey' , 'HLT_TrigEFMissingET_FEB')

class DoTruth (JobProperty):
    """If true, put truth information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(DoTruth)


class DoTrigger (JobProperty):
    """If true, put trigger information in D3PD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(DoTrigger)

class doCells (JobProperty):
    """If true, the dataset has the calo cells."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doCells)

class doCellOutMiniJet (JobProperty):
    """If true, the dataset has the calo cells."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doCellOutMiniJet)


class doEtaRingsNoiseCut (JobProperty):
    """If true, do a noise cut on the EtaRings."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doEtaRingsNoiseCut)

MissingETD3PDMakerFlags = jobproperties.MissingETD3PDMakerFlags

                 
class doMissingETRegions (JobProperty):
    """If true, do a noise cut on the EtaRings."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doMissingETRegions)

MissingETD3PDMakerFlags = jobproperties.MissingETD3PDMakerFlags

                 
class doCaloEMFracSumet (JobProperty):
    """If true, do a noise cut on the EtaRings."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doCaloEMFracSumet)

class doCaloRegions (JobProperty):
    """If true, do a noise cut on the EtaRings."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doCaloRegions)


class doCaloSamplings (JobProperty):
    """If true, do a noise cut on the EtaRings."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doCaloSamplings)


class doCellOutEflow (JobProperty):
    """If true, do a noise cut on the EtaRings."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
jobproperties.MissingETD3PDMakerFlags.add_JobProperty(doCellOutEflow)



MissingETD3PDMakerFlags = jobproperties.MissingETD3PDMakerFlags

                 
