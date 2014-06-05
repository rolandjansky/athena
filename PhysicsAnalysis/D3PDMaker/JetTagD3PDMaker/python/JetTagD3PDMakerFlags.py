# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file JetTagD3PDMaker/python/JetTagD3PDMakerFlags.py
## @brief Python module to hold JetTagD3PDMaker common flags
## @author Georges Aad
## @date Nov, 2010
##



"""
This file is used to configure the JetTagD3PDMaker objects
"""



##-----------------------------------------------------------------------------
## Import
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys

## General flags
class StdInfoInclude(JobProperty):
    """ Standard include blocks needed by physics groups
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[
        JetTagD3PDKeys.BTagWeightsBlockName(),
        JetTagD3PDKeys.TruthInfoBlockName(),
        JetTagD3PDKeys.PUTruthInfoBlockName(),
        JetTagD3PDKeys.IP2DInfoBaseBlockName(),
        JetTagD3PDKeys.IP2DIPInfoBaseBlockName(),
        JetTagD3PDKeys.IP3DInfoBaseBlockName(),
        JetTagD3PDKeys.IP3DIPInfoBaseBlockName(),
        JetTagD3PDKeys.SV1InfoBaseBlockName(),
        JetTagD3PDKeys.SV2InfoBaseBlockName(),
        JetTagD3PDKeys.JetFitterInfoBaseBlockName(),
        JetTagD3PDKeys.JetFitterCombInfoBaseBlockName(),
        JetTagD3PDKeys.JetFitterTagInfoBlockName(),
        JetTagD3PDKeys.IPInfoPlusTrackBlockName(),
        JetTagD3PDKeys.SVInfoPlusBlockName(),
        JetTagD3PDKeys.SV0InfoPlusBlockName()
        ]
 
class DefaultTaggers(JobProperty):
    """ Default Taggers for which the weights will be filled
        list of taggers available in the AODs
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=["IP2D",
                 "IP3D",
                 "SV0", "SV1", "SV2", 
                 "SoftMuonTagChi2", "SecondSoftMuonTagChi2", 
                 "JetFitterTagNN",
                 "JetFitterCOMBNN",
                 "MV1","MV1c","MV2c00","MV2c10","MV2c20",
                 "GbbNN", 
                 ]


class Taggers(JobProperty):
    """ Taggers for which the weights will be filled
        list of taggers available in the AODs
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=["IP2D",
                 "IP3D",
                 "SV0", "SV1", "SV2", 
                 "SoftMuonTagChi2", "SecondSoftMuonTagChi2", 
                 "JetFitterTagNN",
                 "JetFitterCOMBNN",
                 "MV1","MV1c","MV2c00","MV2c10","MV2c20",
                 "GbbNN", 
                 ]

class AllTaggers(JobProperty):
    """ List of all taggers
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=["IP2D", "IP2DNeg","IP2DFlip","IP2DPos",
                 "IP2DSpc","IP2DSpcFlip","IP2DSpcPos","IP2DSpcNeg",
                 "IP3D","IP3DNeg","IP3DFlip","IP3DPos",
                 "IP3DSpc","IP3DSpcFlip","IP3DSpcPos","IP3DSpcNeg",
                 "SV0", 
                 "SV1", "SV1Flip",
                 "SV2", "SV2Flip",
                 "JetProb", "JetProbNeg", 
                 "TrackCounting2D", "TrackCounting2DNeg",
                 "LifetimeTag2D", 
                 "SecVtxTagBU", 
                 "lhSig",
                 "SoftMuonTag", "SoftMuonTagChi2", 
                 "SecondSoftMuonTag", "SecondSoftMuonTagChi2", 
                 #"SoftElectronTag", no calib
                 "JetFitterTagNN", "JetFitterTagNNFlip",
                 "JetFitterCOMBNN", "JetFitterCOMBNNIP3DNeg", "JetFitterCOMBNNIP3DPos",
                 "NewGbbNN", "QG",
                 "MV1Flip","MV2Flip", 
                 "IPFordG", "Gaia", 
                 "MV1","MV1c","MV2c00","MV2c10","MV2c20",
                 "MV1Flip","MV1cFlip","MV2c00Flip","MV2c10Flip","MV2c20Flip"
                 ]


class JetJFVxOnJetAxisAssocSGKey(JobProperty):
    """ storegate key of the JetVxOnJetAxis data object to associate jets with
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=JetTagD3PDKeys.JFVxOnJetAxisDataSGKey()


class JetJFTwoTrackVertexAssocSGKey(JobProperty):
    """ storegate key of the JetVxOnJetAxis data object to associate jets with
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=JetTagD3PDKeys.JFTwoTrackVertexDataSGKey()

## getter labels for association

class TrackAssocLabel(JobProperty):
    """ label for the track getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class MuonAssocLabel(JobProperty):
    """ label for the muon getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class Muon2AssocLabel(JobProperty):
    """ label for the muon2 getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""


class ElectronAssocLabel(JobProperty):
    """ label for the electron getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class PhotonAssocLabel(JobProperty):
    """ label for the photon getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class JFVxOnJetAxisAssocLabel(JobProperty):
    """ label for the jet fitter VxOnJetAxis getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class JFTwoTrackVertexAssocLabel(JobProperty):
    """ label for the jet fitter TwoTrackVertex getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

class MSVVtxInfoAssocLabel(JobProperty):
    """ label for the MSVVtxInfo getter for association
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=""

## switchs to turn on/off blocks

class JetTrackAssoc(JobProperty):
    """ switch to associate jets to tracks
        Analysis::TrackAssociation is used
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class JetTrackGhostAssoc(JobProperty):
    """ switch to associate jets to ghost tracks
        Analysis::TrackAssociation is used
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetBHadronGhostAssoc(JobProperty):
    """ switch to associate jets to ghost bhadron
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class JetTruthGhostAssoc(JobProperty):
    """ switch to associate jets to ghost truth particles
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetTrackGhostAssocName(JobProperty):
    """ track ghost association object name
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TrackAssoc"

class JetBHadronGhostAssocName(JobProperty):
    """ bhadron ghost association object name
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthBHadron"


class JetTruthGhostAssocName(JobProperty):
    """ truth ghost association object name
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue="TruthAssoc"

class JetConstituantTruthAssoc(JobProperty):
    """ switch to associate truth jets to Gen Particles
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class JetMuonAssoc(JobProperty):
    """ switch to associate jets to muons
        Analysis::MuonAssociation is used
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class JetElectronAssoc(JobProperty):
    """ switch to associate jet to electrons 
        Analysis::ElectronAssociation is used
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetPhotonAssoc(JobProperty):
    """ switch to associate jets to photons
        Analysis::PhotonAssociation is used
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetGenSoftLeptonAssoc(JobProperty):
    """ switch to associate truth lepton in jets to Gen particles
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class PileupLabeling(JobProperty):
    """ switch to add the PUTruthInfo objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class InfoBaseGaia(JobProperty):
    """ switch for InfoBaseGaia blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True



class InfoBase(JobProperty):
    """ switch for InfoBase blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class GbbNNInfo(JobProperty):
    """ switch for GbbNNInfo
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class NewGbbNNInfo(JobProperty):
    """ switch for NewGbbNNInfo
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class QGInfo(JobProperty):
    """ switch for QGInfo
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class InfoBaseCalib(JobProperty):
    """ switch for InfoBase blocks for fliped tags used for calibration
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class JetFitterTagInfo(JobProperty):
    """ switch for JetFitterTagInfo blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class JetFitterCharmTagInfo(JobProperty):
    """ switch for JetFitterTagInfo blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetFitterTagFlipInfo(JobProperty):
    """ switch for JetFitterTagFlipInfo blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class IPInfoPlus(JobProperty):
    """ switch for IPInfoPlus blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class SVInfoPlus(JobProperty):
    """ switch for SVInfoPlus blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    

class SV0InfoPlus(JobProperty):
    """ switch for SV0InfoPlus blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class MultiSVInfoPlus(JobProperty):
    """ switch for MultiSVInfoPlus blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class SoftMuonInfo(JobProperty):
    """ switch for SoftMuonInfo blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class SoftMuonChi2Info(JobProperty):
    """ switch for SoftMuonChi2 blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class SoftElectronInfo(JobProperty):
    """ switch for SoftElectronInfo blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetVKalVxBadTrack(JobProperty):
    """ switch for JetVKalVxBadTrack blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetFitterVertexFinderInfo(JobProperty):
    """ switch for JetFitterVertexFinderInfo blocks
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class JetFitterFlipVxOnJetAxisAssoc(JobProperty):
    """ switch for JetFitterFlip association to VxOnJetAxis
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetFitterFlipTwoTrackVertexAssoc(JobProperty):
    """ switch for JetFitterFlip association to TwoTrackVertex
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class AddContainedTrackP4(JobProperty):
    """ switch to add contained track p4 even if index association is ON
        used for debugging  
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

### filter config

class JetFilterPtMinCut(JobProperty):
    """ jet minimum pt
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=5000.

class JetFilterEtaCut(JobProperty):
    """ jet eta cut
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=2.7

class JetFilterUseEMScale(JobProperty):
    """ use jet EM scale for filter
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

### track filter
class TrackPtCut(JobProperty):
    """ track minimum pt
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.

### jettag D3PD event filter

class SkimD3PD(JobProperty):
    """ skim D3PD
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class UseJetEventFilter(JobProperty):
    """ use jet filter
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class UseMuonEventFilter(JobProperty):
    """ use muon filter
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class FilterJetPtCut(JobProperty):
    """ Minimum pt of jets used for event filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=7000.

class FilterJetWithMuonPtCut(JobProperty):
    """ Minimum pt of jets with muon used for event filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=7000.

class FilterJetEtaCut(JobProperty):
    """ Maximum eta of jets used for event filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=2.7

class FilterMinNJets(JobProperty):
    """ Minimum number of selected jets in an event
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0

class FilterMuonInJetPtCut(JobProperty):
    """ Minimum pt of muon in jets used for event filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=3000.

class FilterMinNJetsMuon(JobProperty):
    """  Minimum number of selected jets with a muon in a event
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=0

class FilterMuonCollections(JobProperty):
    """  muon collections used for filters
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=["MuidMuonCollection"]

class FilterMuonPtCut(JobProperty):
    """ Minimum pt of muons used for event filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=20000.  
    
class FilterMuonEtaCut(JobProperty):
    """ eta cut of muons used for event filter
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=2.5  

class FilterMuonTightCut(JobProperty):
    """ use tight cut of muons used for event filter
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class FilterMinNumberOfMuons(JobProperty):
    """ Minimum number of muons for event filter
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=2                                                   


### MC filter 

class MCPtMinCut(JobProperty):
    """  gen particle min pt cut [MeV]
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=100.

class MCEtaCut(JobProperty):
    """  gen particle eta cut
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=3.

class MCEmptyEvents(JobProperty):
    """  keep empty events
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class MCDummyEvents(JobProperty):
    """  keep dummy events (used to determine pileup type)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class MCPileUp(JobProperty):
    """  keep pileup events
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

### config objects to add to the D3PD alg
class JetCollections(JobProperty):
    """ list of jet collections
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]

class AddFatJets(JobProperty):
    """ add fat jets objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddMuonInJetsObject(JobProperty):
    """ add muon in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddInlineSoftMuonProperties(JobProperty):
    """ add inline info muon in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddInlineSoftMuonChi2Properties(JobProperty):
    """ add inline info muon in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddSecondMuonCollection(JobProperty):
    """ add muon in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddElectronInJetsObject(JobProperty):
    """ add electron in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddPhotonInJetsObject(JobProperty):
    """ add photon in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class JetFitterVertexFinderObjects(JobProperty):
    """ JetFitterVertexFinder to be added to VxOnJetAxis and TwoTrackVertex objects
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=['NewJetFitterVxFinder','NewJetFitterVxFinderFlip']

class AddVxOnJetAxisInJetsObject(JobProperty):
    """ add VxOnJetAxis in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddTwoTrackVertexInJetsObject(JobProperty):
    """ add TwoTrackVertex in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddMSVVtxInfoInJetsObject(JobProperty):
    """ add dMSVVtxInfo in jets object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddEventInfoObject(JobProperty):
    """ add event info object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddPileUpEventInfoObject(JobProperty):
    """ add pile up event info object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddTrackObject(JobProperty):
    """ add track object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class TrackParticleSGKey(JobProperty):
    """ track particle sg key
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=D3PDMakerFlags.TrackSGKey()

class TruthTrackParticleSGKey(JobProperty):
    """ track particle truth sg key
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue='TrackParticleTruthCollection'


class METObjects(JobProperty):
    """ list of MET objects
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=['MET_RefFinal']

class AddPixelClusterObject(JobProperty):
    """ add pixel cluster object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddPrimaryVertexObject(JobProperty):
    """ add primary vertex object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

 
class PrimaryVertexSGKey(JobProperty):
    """ primary vertex sg key
    """
    statusOn=True
    allowedTypes=['str']
    StoredValue=D3PDMakerFlags.VertexSGKey()


class AddBeamSpotObject(JobProperty):
    """ add BeamSpot object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddMuonMuidObject(JobProperty):
    """ add MuonMuid object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddMuonStacoObject(JobProperty):
    """ add MuonStaco object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddGenObjects(JobProperty):
    """ add gen particles/vertices/event/truthtracks objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddGenVertexObjects(JobProperty):
    """ add gen vertices objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddGenParticleObjects(JobProperty):
    """ add gen particles objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddGenEventObjects(JobProperty):
    """ add gen events objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


class AddTruthTrackObjects(JobProperty):
    """ add truth track objects
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddTriggerObject(JobProperty):
    """ add trigger object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddTriggerBits(JobProperty):
    """ add trigger bits
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class TriggerBitsList(JobProperty):
    """ trigger list bits (btag bits already in)
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=["EF_[2-9]?f?j[0-9]*.*","L2_[2-9]?f?j[0-9]*.*", "L1_[2-9]?F?J[0-9]*.*" ]

class AddJetTriggerObject(JobProperty):
    """ add jet trigger object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddBJetTriggerObject(JobProperty):
    """ add b-jet trigger object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class AddTrigL2IDTracks(JobProperty):
    """ add trigger L2 ID tracks used for btagging
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class AddTrigEFIDTracks(JobProperty):
    """ add trigger EF ID tracks used for btagging
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False


class AddPixCond(JobProperty):
    """ add pixel condition object
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

### ntuple type
class NtupleType(JobProperty):
    """  type of the ntuple
    """
    statusOn=True
    allowedTypes=['str']
    allowedValues=["", "Debug", "Full", "Slim", "Efficiency", "FakeRate", "Physics"]
    StoredValue=""

### property dictionary to be passed to the d3pd
class D3PDPropDict(JobProperty):
    """  property dictionary to be passed to the d3pd
    """
    statusOn=True
    allowedTypes=['dict']
    StoredValue={}

class LevelOffset(JobProperty):
    """  level offset for btagging d3pd
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=-10


class JetTagD3PDMakerFlags(JobPropertyContainer):
    """ The JetTag D3PD Flags container
    """

# add the flag container to the top container
jobproperties.add_Container(JetTagD3PDMakerFlags)

# and now add the properties
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(StdInfoInclude)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(Taggers)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(DefaultTaggers)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AllTaggers)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetJFVxOnJetAxisAssocSGKey)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetJFTwoTrackVertexAssocSGKey)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(TrackAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MuonAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(Muon2AssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(ElectronAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(PhotonAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JFVxOnJetAxisAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JFTwoTrackVertexAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MSVVtxInfoAssocLabel)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetTrackAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetTrackGhostAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetBHadronGhostAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetTruthGhostAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetTrackGhostAssocName)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetBHadronGhostAssocName)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetTruthGhostAssocName)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetConstituantTruthAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetMuonAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetElectronAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetPhotonAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetGenSoftLeptonAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(PileupLabeling)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(InfoBase)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(InfoBaseGaia)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(GbbNNInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(NewGbbNNInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(QGInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(InfoBaseCalib)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterTagInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterCharmTagInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterTagFlipInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(IPInfoPlus)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(SVInfoPlus)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(SV0InfoPlus)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MultiSVInfoPlus)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(SoftMuonInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(SoftMuonChi2Info)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(SoftElectronInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetVKalVxBadTrack)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterVertexFinderInfo)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterVertexFinderObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterFlipVxOnJetAxisAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFitterFlipTwoTrackVertexAssoc)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddContainedTrackP4)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterJetPtCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterJetWithMuonPtCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterJetEtaCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMuonInJetPtCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMinNJets)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMinNJetsMuon)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(UseMuonEventFilter)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(UseJetEventFilter)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMuonCollections)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMuonPtCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMuonEtaCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMuonTightCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(FilterMinNumberOfMuons)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFilterPtMinCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFilterEtaCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetFilterUseEMScale)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(TrackPtCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MCPtMinCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MCEtaCut)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MCEmptyEvents)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MCDummyEvents)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(MCPileUp)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(NtupleType)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(JetCollections)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddFatJets)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddMuonInJetsObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddInlineSoftMuonProperties)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddInlineSoftMuonChi2Properties)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddSecondMuonCollection)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddElectronInJetsObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddPhotonInJetsObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddVxOnJetAxisInJetsObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTwoTrackVertexInJetsObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddMSVVtxInfoInJetsObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddEventInfoObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddPileUpEventInfoObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTrackObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(TrackParticleSGKey)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(TruthTrackParticleSGKey)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(METObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddPixelClusterObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddPrimaryVertexObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(PrimaryVertexSGKey)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddBeamSpotObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddMuonMuidObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddMuonStacoObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddGenObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddGenVertexObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddGenParticleObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddGenEventObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTruthTrackObjects)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTriggerObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTriggerBits)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(TriggerBitsList)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddJetTriggerObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddBJetTriggerObject)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTrigL2IDTracks)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddTrigEFIDTracks)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(AddPixCond)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(SkimD3PD)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(D3PDPropDict)
jobproperties.JetTagD3PDMakerFlags.add_JobProperty(LevelOffset)

## use this shortcut
#from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
##
JetTagD3PDFlags=jobproperties.JetTagD3PDMakerFlags
