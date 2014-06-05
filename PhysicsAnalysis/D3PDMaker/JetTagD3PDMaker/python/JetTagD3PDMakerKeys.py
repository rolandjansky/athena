# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagD3PDMakerKeys.py
## @brief Python module to hold JetTagD3PDMaker common Keys
## @author Georges Aad
## @date Nov, 2010
##



"""
This file is used to set different conventions for the JetTagD3PD objects:
block names and common labels ...
In most cases you do not need to change them.
"""

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

###### addBTagInfoToJetObject specific keys

### fat jets list
class FatJetsList(JobProperty):
    """ list of fat jets
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

### jet tag association

class JetConstituantTruthAssocBlockName(JobProperty):
    """ blockname for jet constit truth assoc
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetConstituantTruthAssoc"


class JetTrackAssocPrefix(JobProperty):
    """ prefix for jet track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assoctrk_"

class JetTrackAssocBlockName(JobProperty):
    """ blockname for jet track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetTrackAssoc"

class JetTrackBlockName(JobProperty):
    """ blockname for jet track 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetTrackCont"

class JetMuonAssocPrefix(JobProperty):
    """ prefix for jet muon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocmuon_"

class JetMuon2AssocPrefix(JobProperty):
    """ prefix for jet muon2 association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocmuon2_"

class JetMuonAssocBlockName(JobProperty):
    """ blockname for jet muon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetMuonAssoc"

class JetMuon2AssocBlockName(JobProperty):
    """ blockname for jet muon2 association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetMuon2Assoc"


class JetElectronAssocPrefix(JobProperty):
    """ prefix for jet electron association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocelectron_"

class JetElectronAssocBlockName(JobProperty):
    """ blockname for jet electron association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetElectronAssoc"


class JetPhotonAssocPrefix(JobProperty):
    """ prefix for jet photon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocphoton_"

class JetPhotonAssocBlockName(JobProperty):
    """ blockname for jet photon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetPhotonAssoc"


class JetJFVxOnJetAxisAssocPrefix(JobProperty):
    """ prefix for jet (JetFitter) vertex on jet axis association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfvxonjetaxis_"

class JetJFVxOnJetAxisAssocBlockName(JobProperty):
    """ blockname for jet (JetFitter) vertex on jet axis association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetJFVxOnJetAxisAssoc"

class JetJFFlipVxOnJetAxisAssocPrefix(JobProperty):
    """ prefix for jet (JetFitterFlip) vertex on jet axis association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfflipvxonjetaxis_"

class JetJFFlipVxOnJetAxisAssocBlockName(JobProperty):
    """ blockname for jet (JetFitter) vertex on jet axis association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetJFFlipVxOnJetAxisAssoc"


class JetJFTwoTrackVertexAssocPrefix(JobProperty):
    """ prefix for jet JetFitter two track vertex association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jftwotrackvertex_"

class JetJFTwoTrackVertexAssocBlockName(JobProperty):
    """ blockname for jet JetFitter two track vertex association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetJFTwoTrackVertexAssoc"

class JetJFFlipTwoTrackVertexAssocPrefix(JobProperty):
    """ prefix for jet JetFitter two track vertex association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jffliptwotrackvertex_"

class JetJFFlipTwoTrackVertexAssocBlockName(JobProperty):
    """ blockname for jet JetFitter two track vertex association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetJFFlipTwoTrackVertexAssoc"



class JetVKalVxBadTrackAssocPrefix(JobProperty):
    """ prefix for jet (SV) VKalVx bad track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_VKalbadtrack_"

class JetVKalVxBadTrackAssocBlockName(JobProperty):
    """ blockname for jet (SV) VKalVx bad track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetVKalVxBadTrackAssoc"


class JetGenSoftLeptonAssocPrefix(JobProperty):
    """ prefix for jet generated truth lepton association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_gentruthlepton_"

class JetGenSoftLeptonAssocBlockName(JobProperty):
    """ blockname for jet generated truth lepton association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetGenSoftLeptonAssoc"

class JetGenSoftLeptonBlockName(JobProperty):
    """ blockname for jet generated truth lepton 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetGenSoftLeptonCont"

class JetGenSoftLeptonPropBlockName(JobProperty):
    """ blockname for jet generated truth lepton contained association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetGenSoftLeptonProp"


## ghost assoc
class JetTrackGhostAssocPrefix(JobProperty):
    """ prefix for jet track ghost association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocghost_trk_"

class JetTrackGhostAssocBlockName(JobProperty):
    """ blockname for jet track ghost association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetTrackGhostAssoc"


class JetBHadronGhostAssocPrefix(JobProperty):
    """ prefix for jet bhadron ghost association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocghost_bhadron_"

class JetBHadronGhostAssocBlockName(JobProperty):
    """ blockname for jet bhadron ghost association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetBHadronGhostAssoc"


class JetTruthGhostAssocPrefix(JobProperty):
    """ prefix for jet truth ghost association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_assocghost_mcpart_"

class JetTruthGhostAssocBlockName(JobProperty):
    """ blockname for jet truth ghost association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetTruthGhostAssoc"


### btagging weights

class BTagWeightsPrefix(JobProperty):
    """ prefix for jet btag weights filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_weight_"

class BTagWeightsBlockName(JobProperty):
    """ blockname for  btag weights filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_BTagWeights"

### jet tag info objects

class TruthInfoPrefix(JobProperty):
    """ prefix for jet truth info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_truth_"

class TruthInfoBlockName(JobProperty):
    """ blockname for jet truth info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_TruthInfo"
class QGPartonTruthInfoPrefix(JobProperty):
    """ prefix for jet truth QG info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_truth_qg_"

class QGPartonTruthInfoBlockName(JobProperty):
    """ blockname for jet truth QG info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_QGPartonTruthInfo"

class PUTruthInfoPrefix(JobProperty):
    """ prefix for jet PU truth info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_putruth_"

class PUTruthInfoBlockName(JobProperty):
    """ blockname for jet PU truth info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_PUTruthInfo"
 
class PUQGPartonTruthInfoPrefix(JobProperty):
    """ prefix for jet PU truth QG info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_putruth_qg_"

class PUQGPartonTruthInfoBlockName(JobProperty):
    """ blockname for jet PU truth QG info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_PUQGPartonTruthInfo"

class IP2DInfoBasePrefix(JobProperty):
    """ prefix for jet IP2D (IP) info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_ip2d_"

class IP2DInfoBaseBlockName(JobProperty):
    """ blockname for jet IP2D info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IP2DInfoBase"

class IP2DIPInfoBaseBlockName(JobProperty):
    """ blockname for jet  IP2D IP info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IP2DIPInfoBase"


class IP3DInfoBasePrefix(JobProperty):
    """ prefix for jet IP3D (IP) info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_ip3d_"

class IP3DInfoBaseBlockName(JobProperty):
    """ blockname for jet IP3D info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IP3DInfoBase"

class IP3DIPInfoBaseBlockName(JobProperty):
    """ blockname for jet  IP3D IP info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IP3DIPInfoBase"

class JetProbInfoBasePrefix(JobProperty):
    """ prefix for jet JetProb info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jetprob_"

class JetProbInfoBaseBlockName(JobProperty):
    """ blockname for jet  IP3D IP info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetProbInfoBase"


class SV1InfoBasePrefix(JobProperty):
    """ prefix for jet SV1 info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_sv1_"

class SV1InfoBaseBlockName(JobProperty):
    """ blockname for jet SV1 info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SV1InfoBase"


class SV2InfoBasePrefix(JobProperty):
    """ prefix for jet SV2 info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_sv2_"

class SV2InfoBaseBlockName(JobProperty):
    """ blockname for jet SV2 info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SV2InfoBase"

class GbbNNInfoPrefix(JobProperty):
    """ prefix for jet GbbNN info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_gbbnn_"

class GbbNNInfoBlockName(JobProperty):
    """ blockname for jet GbbNN info  filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_GbbNNInfo"

class NewGbbNNInfoPrefix(JobProperty):
    """ prefix for jet NewGbbNN info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_newgbbnn_"

class NewGbbNNInfoBlockName(JobProperty):
    """ blockname for jet NewGbbNN info  filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_NewGbbNNInfo"


class QGInfoPrefix(JobProperty):
    """ prefix for jet QG info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_qg_"

class QGInfoBlockName(JobProperty):
    """ blockname for jet QG info  filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_QGInfo"


class JetFitterTagInfoPrefix(JobProperty):
    """ prefix for jet JetFitter info base and tag info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfit_"

class JetFitterCharmTagInfoPrefix(JobProperty):
    """ prefix for jet JetFitterCharm info base and tag info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfitc_"


class JetFitterInfoBaseBlockName(JobProperty):
    """ blockname for jet JetFitter info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterInfoBase"

class JetFitterCharmInfoBaseBlockName(JobProperty):
    """ blockname for jet JetFitterCharm info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterCharmInfoBase"

class JetFitterTagInfoBlockName(JobProperty):
    """ blockname for jet JetFitter tag info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterTagInfo"

class JetFitterCharmTagInfoBlockName(JobProperty):
    """ blockname for jet JetFitterCharm tag info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterCharmTagInfo"


class JetFitterTagFlipInfoPrefix(JobProperty):
    """ prefix for jet JetFitterFlip info base and tag info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfitflip_"

class JetFitterFlipInfoBaseBlockName(JobProperty):
    """ blockname for jet JetFitterFlip info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterFlipInfoBase"

class JetFitterTagFlipInfoBlockName(JobProperty):
    """ blockname for jet JetFitterFlip tag info filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterTagFlipInfo"

class JetFitterCombInfoBasePrefix(JobProperty):
    """ prefix for jet JetFitterComb info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfitcomb_"

class JetFitterCombInfoBaseBlockName(JobProperty):
    """ blockname for jet JetFitterComb info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterCombInfoBase"

class JetFitterCombIP3DPosInfoBasePrefix(JobProperty):
    """ prefix for jet JetFitterCombIP3DPos info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfitcombip3dpos_"

class JetFitterCombIP3DPosInfoBaseBlockName(JobProperty):
    """ blockname for jet JetFitterComb info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterCombIP3DPosInfoBase"

class JetFitterCombIP3DNegInfoBasePrefix(JobProperty):
    """ prefix for jet JetFitterCombIP3DNeg info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfitcombip3dneg_"

class JetFitterCombIP3DNegInfoBaseBlockName(JobProperty):
    """ blockname for jet JetFitterComb info base filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterCombIP3DNegInfoBase"


## fillers and associators for info plus

class IPInfoPlusTrackAssocPrefix(JobProperty):
    """ prefix for jet IPInfoPlus track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_ipplus_trk_"

class IPInfoPlusTrackAssocBlockName(JobProperty):
    """ blockname for jet IPInfoPlus track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IPInfoPlusTrackAssoc"

class IPInfoPlusTrackBlockName(JobProperty):
    """ blockname for jet IPInfoPlus track 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IPInfoPlusTrackCont"

class IPInfoPlusTrkP4BlockName(JobProperty):
    """ blockname for jet IPInfoPlus track four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_IPInfoPlusTrkP4"


class SVInfoPlusPrefix(JobProperty):
    """ prefix for jet SVInfoPlus filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_svp_"

class SVInfoPlusBlockName(JobProperty):
    """ blockname for jet SVInfoPlus filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SVInfoPlus"


class SVInfoPlusTrackAssocPrefix(JobProperty):
    """ prefix for jet SVInfoPlus track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_svp_trk_"

class SVInfoPlusTrackAssocBlockName(JobProperty):
    """ blockname for jet SVInfoPlus track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SVInfoPlusTrackAssoc"

class SVInfoPlusTrackBlockName(JobProperty):
    """ blockname for jet SVInfoPlus track
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SVInfoPlusTrackCont"


class SVInfoPlusTrkP4BlockName(JobProperty):
    """ blockname for jet SVInfoPlus track four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SVInfoPlusTrkP4"


class SV0InfoPlusPrefix(JobProperty):
    """ prefix for jet SV0InfoPlus filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_sv0p_"

class SV0InfoPlusBlockName(JobProperty):
    """ blockname for jet SV0InfoPlus filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SV0InfoPlus"

 
class SV0InfoPlusTrackAssocPrefix(JobProperty):
    """ prefix for jet SV0InfoPlus track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_sv0p_trk_"

class SV0InfoPlusTrackAssocBlockName(JobProperty):
    """ blockname for jet SV0InfoPlus track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SV0InfoPlusTrackAssoc"

class SV0InfoPlusTrackBlockName(JobProperty):
    """ blockname for jet SV0InfoPlus track
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SV0InfoPlusTrackCont"

class MultiSVInfoPlusPrefix(JobProperty):
    """ prefix for jet MultiSVInfoPlus filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_msvp_"

class MultiSVInfoPlusBlockName(JobProperty):
    """ blockname for jet MultiSVInfoPlus filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_MultiSVInfoPlus"


class MultiSVInfoPlusMSVVtxAssocPrefix(JobProperty):
    """ prefix for jet MultiSVInfoPlus MSVVtxInfo association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_msvp_msvinjet_"

class MultiSVInfoPlusMSVVtxAssocBlockName(JobProperty):
    """ blockname for jet MultiSVInfoPlus MSVVtxInfo association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_MultiSVInfoPlusMSVVtxAssoc"


class SV0InfoPlusTrkP4BlockName(JobProperty):
    """ blockname for jet SV0InfoPlus track four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SV0InfoPlusTrkP4"
               

 
class SoftMuonInfoMuonAssocPrefix(JobProperty):
    """ prefix for jet SoftMuonInfo muon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_softmuoninfo_muon_"

class SoftMuon2InfoMuon2AssocPrefix(JobProperty):
    """ prefix for jet SoftMuonInfo muon2 association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_softmuon2info_muon2_"

class SoftMuonInfoMuonAssocBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonAssoc"

class SoftMuonInfoMuonBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonCont"

class SoftMuon2InfoMuon2AssocBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon2 association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuon2InfoMuon2Assoc"

class SoftMuon2InfoMuon2BlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon2 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuon2InfoMuon2Cont"

class SoftMuonInfoP4BlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonP4"

   
class SoftMuonInfoChargeBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon charge
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonCharge"

class SoftMuonInfoAuthorBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon author
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonAuthor"

class SoftMuonInfomatchChi2BlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon matchchi2 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonMChi2"

class SoftMuonInfoAcceptBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon accept methods
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonAcceptMethods"

class SoftMuonInfoELossBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon energy loss
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonELoss"

class SoftMuonInfoHitsBlockName(JobProperty):
    """ blockname for jet SoftMuonInfo muon hits
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonInfoMuonHits"


              
class SoftMuon2InfoP4BlockName(JobProperty):
    """ blockname for jet SoftMuon2Info muon2 four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuon2InfoMuon2P4"


class SoftMuonChi2InfoMuonAssocPrefix(JobProperty):
    """ prefix for jet SoftMuonChi2Info muon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_softmuonchi2info_muon_"

class SoftMuon2Chi2InfoMuon2AssocPrefix(JobProperty):
    """ prefix for jet SoftMuonChi2Info muon2 association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_softmuon2chi2info_muon2_"

class SoftMuonChi2InfoMuonAssocBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonAssoc"

class SoftMuonChi2InfoMuonBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonCont"

class SoftMuon2Chi2InfoMuon2AssocBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon2 association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuon2Chi2InfoMuon2Assoc"

class SoftMuon2Chi2InfoMuon2BlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon2 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuon2Chi2InfoMuon2Cont"

class SoftMuonChi2InfoP4BlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonP4"

   
class SoftMuonChi2InfoChargeBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon charge
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonCharge"

class SoftMuonChi2InfoAuthorBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon author
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonAuthor"

class SoftMuonChi2InfomatchChi2BlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon matchchi2 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonMChi2"

class SoftMuonChi2InfoAcceptBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon accept methods
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonAcceptMethods"

class SoftMuonChi2InfoELossBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon energy loss
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonELoss"

class SoftMuonChi2InfoHitsBlockName(JobProperty):
    """ blockname for jet SoftMuonChi2Info muon hits
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuonChi2InfoMuonHits"


              
class SoftMuon2Chi2InfoP4BlockName(JobProperty):
    """ blockname for jet SoftMuon2Chi2Info muon2 four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftMuon2Chi2InfoMuon2P4"


class SoftElectronInfoElectronAssocPrefix(JobProperty):
    """ prefix for jet SoftElectronInfo electron association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_softelectron_electron_"

class SoftElectronInfoElectronAssocBlockName(JobProperty):
    """ blockname for jet SoftElectronInfo electron association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftElectronInfoElectronAssoc"

class SoftElectronInfoElectronBlockName(JobProperty):
    """ blockname for jet SoftElectronInfo electron 
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftElectronInfoElectronCont"

class SoftElectronInfoP4BlockName(JobProperty):
    """ blockname for jet SoftElectronInfo electron four momentum
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_SoftElectronInfoElectronP4"


class JetFitterVxAssocPrefix(JobProperty):
    """ prefix for jet JetFitterVxAssoc filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="flavor_component_jfitvx_"

class JetFitterVxAssocBlockName(JobProperty):
    """ blockname for jet JetFitterVxAssoc filler
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JetFitterVxAssoc"

### MSVVtxInfo in jet objects
class MSVVtxInfoInJetGetterLabel(JobProperty):
    """ label for MSVVtxInfo in jet getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_MSVVtxInfoInJet"

class MSVVtxInfoInJetPrefix(JobProperty):
    """ prefix for MSVVtxInfo in jet object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="msvinjet_"

class MSVVtxInfoTrackAssocPrefix(JobProperty):
    """ prefix for MSVVtxInfo in jet track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="trk_"

### muon in jet object

class MuonInJetGetterLabel(JobProperty):
    """ label for muon in jet getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_MuonInJet"

class Muon2InJetGetterLabel(JobProperty):
    """ label for muon2 in jet getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_Muon2InJet"

class MuonInJetPrefix(JobProperty):
    """ prefix for muon in jet object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="muoninjet_"


class Muon2InJetPrefix(JobProperty):
    """ prefix for muon2 in jet object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="muon2injet_"

class MuonPrimaryTrackAssocPrefix(JobProperty):
    """ prefix for muon in jet primary track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="primtrk_"

class MuonInDetTrackAssocPrefix(JobProperty):
    """ prefix for muon in jet indet track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="trk_"


### electron in jet object

class ElectronInJetGetterLabel(JobProperty):
    """ label for electron in jet getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_ElectronInJet"

class ElectronInJetPrefix(JobProperty):
    """ prefix for electron in jet object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="elecinjet_"

class ElectronTrackAssocPrefix(JobProperty):
    """ prefix for electron in jet track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="trk_"


### photon in jet object

class PhotonInJetGetterLabel(JobProperty):
    """ label for photon in jet getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_PhotonInJet"

class PhotonInJetPrefix(JobProperty):
    """ prefix for photon in jet object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="photoninjet_"


### Jet fitter on jet axis vertices

class JFVxOnJetAxisMakerAlgPostfix(JobProperty):
    """ postfix for JF VxOnJetAxis collection maker alg
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="_JFVxOnJetAxisMaker_D3PD"

class JFVxOnJetAxisGetterLabel(JobProperty):
    """ label for JF VxOnJetAxis getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JFVxOnJetAxisInJet"

class JFVxOnJetAxisPrefix(JobProperty):
    """ prefix for jet fitter VxOnJetAxis object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="jfvxonjetaxis_"

class JFVxOnJetAxisDataSGKey(JobProperty):
    """ storegate key for the jet fitter VxOnJetAxis collection
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JFVxOnJetAxisData"

class VxOnJetAxisTrackAssocPrefix(JobProperty):
    """ prefix for VxOnJetAxis track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="trk_"


### Jet fitter two track vertices

class JFTwoTrackVertexMakerAlgPostfix(JobProperty):
    """ postfix for JF TwoTrackVertex collection maker alg
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="_JFTwoTrackVertexMaker_D3PD"

class JFTwoTrackVertexGetterLabel(JobProperty):
    """ label for JF TwoTrackVertex getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_JFTwoTrackVertexInJet"

class JFTwoTrackVertexPrefix(JobProperty):
    """ prefix for TwoTrackVertex object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="jftwotrkvertex_"

class JFTwoTrackVertexDataSGKey(JobProperty):
    """ storegate key for the jet fitter TwoTrackVertex collection
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JFTwoTrackVertexData"

class TwoTrackVertexTrackAssocPrefix(JobProperty):
    """ prefix for TwoTrackVertex track association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="trk_"



class FilteredJetMakerAlgPostfix(JobProperty):
    """ postfix for filtered jet maker alg
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTagFilteredJets_D3PD"

### tracks

class TrackGetterLabel(JobProperty):
    """ label for track getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_Track"
 
class TrackPrefix(JobProperty):
    """ prefix for track object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="trk_"

### primary vertex

class PrimaryVertexGetterLabel(JobProperty):
    """ label for primary vertex getter
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="JetTag_PrimaryVertex"
 

class PrimaryVertexPrefix(JobProperty):
    """ prefix for primary vertex object
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="primvx_"


class D3PDAlgName(JobProperty):
    """ name of the D3PD algorithm
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="BTagD3PDAlg"


class D3PDTupleName(JobProperty):
    """ name of the D3PD tree
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="btagd3pd"

class D3PDFileName(JobProperty):
    """ name of the D3PD output file
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="btagd3pd.root"

class FilteredJetCollectionPostfix(JobProperty):
    """ postfix for filtered jet collections storegate key
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="_JetTagJetFilter_D3PD"


##-----------------------------------------------------------------------------
## Definition of the TrackD3PDMaker flag container

class JetTagD3PDMakerKeys(JobPropertyContainer):
    """ The JetTag D3PD keys container
    """
# add the key container to the top container
jobproperties.add_Container(JetTagD3PDMakerKeys)

# and now add the properties

jobproperties.JetTagD3PDMakerKeys.add_JobProperty(FatJetsList)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetConstituantTruthAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTrackAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTrackGhostAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTrackGhostAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetBHadronGhostAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetBHadronGhostAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTruthGhostAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTruthGhostAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetTrackBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetMuonAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetMuonAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetMuon2AssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetMuon2AssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetElectronAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetElectronAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetPhotonAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetPhotonAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFVxOnJetAxisAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFVxOnJetAxisAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFFlipVxOnJetAxisAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFFlipVxOnJetAxisAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFTwoTrackVertexAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFTwoTrackVertexAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFFlipTwoTrackVertexAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetJFFlipTwoTrackVertexAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetVKalVxBadTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetVKalVxBadTrackAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetGenSoftLeptonAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetGenSoftLeptonAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetGenSoftLeptonBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetGenSoftLeptonPropBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(BTagWeightsPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(BTagWeightsBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(TruthInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(TruthInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(QGPartonTruthInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(QGPartonTruthInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PUTruthInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PUTruthInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PUQGPartonTruthInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PUQGPartonTruthInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IP2DInfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IP2DInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IP2DIPInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IP3DInfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetProbInfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IP3DInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IP3DIPInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetProbInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV1InfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV1InfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV2InfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV2InfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(GbbNNInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(GbbNNInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(NewGbbNNInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(NewGbbNNInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(QGInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(QGInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterTagInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterTagInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCharmTagInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCharmInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCharmTagInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterTagFlipInfoPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterFlipInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterTagFlipInfoBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCombInfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCombInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCombIP3DPosInfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCombIP3DPosInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCombIP3DNegInfoBasePrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterCombIP3DNegInfoBaseBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IPInfoPlusTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IPInfoPlusTrackAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IPInfoPlusTrackBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(IPInfoPlusTrkP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SVInfoPlusPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SVInfoPlusBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SVInfoPlusTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SVInfoPlusTrackAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SVInfoPlusTrackBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SVInfoPlusTrkP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV0InfoPlusPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV0InfoPlusBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV0InfoPlusTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV0InfoPlusTrackAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV0InfoPlusTrackBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SV0InfoPlusTrkP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MultiSVInfoPlusPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MultiSVInfoPlusBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MultiSVInfoPlusMSVVtxAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MultiSVInfoPlusMSVVtxAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoMuonAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoMuonAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoMuonBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoChargeBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoAuthorBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfomatchChi2BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoAcceptBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoELossBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonInfoHitsBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2InfoMuon2AssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2InfoMuon2AssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2InfoMuon2BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2InfoP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoMuonAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoMuonAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoMuonBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoChargeBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoAuthorBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfomatchChi2BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoAcceptBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoELossBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuonChi2InfoHitsBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2Chi2InfoMuon2AssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2Chi2InfoMuon2AssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2Chi2InfoMuon2BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftMuon2Chi2InfoP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftElectronInfoElectronAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftElectronInfoElectronAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftElectronInfoElectronBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(SoftElectronInfoP4BlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterVxAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JetFitterVxAssocBlockName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MSVVtxInfoInJetGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MSVVtxInfoInJetPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MSVVtxInfoTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MuonInJetGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MuonInJetPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(Muon2InJetGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(Muon2InJetPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MuonPrimaryTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(MuonInDetTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(ElectronInJetGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(ElectronInJetPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(ElectronTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PhotonInJetGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PhotonInJetPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFVxOnJetAxisMakerAlgPostfix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFVxOnJetAxisGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFVxOnJetAxisPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFVxOnJetAxisDataSGKey)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(VxOnJetAxisTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFTwoTrackVertexMakerAlgPostfix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFTwoTrackVertexGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFTwoTrackVertexPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(JFTwoTrackVertexDataSGKey)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(TwoTrackVertexTrackAssocPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(FilteredJetMakerAlgPostfix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(TrackGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(TrackPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PrimaryVertexGetterLabel)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(PrimaryVertexPrefix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(D3PDAlgName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(D3PDTupleName)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(FilteredJetCollectionPostfix)
jobproperties.JetTagD3PDMakerKeys.add_JobProperty(D3PDFileName)

## use this shortcut
#from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
##
JetTagD3PDKeys=jobproperties.JetTagD3PDMakerKeys
