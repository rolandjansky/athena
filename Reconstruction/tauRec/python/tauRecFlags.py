# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

################################################################################
##
#@file tauRecFlags.py
#
#@brief define some tauRec flags 
################################################################################

#=======================================================================
# imports
#=======================================================================
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


class Enabled(JobProperty):
    """ if all tau algorithm to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTauRec(JobProperty):
    """ if TauRec to be enabled
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    def get_Value(self):
        return self.statusOn and self.StoredValue and jobproperties.tauRecFlags.Enabled()

class isStandalone(JobProperty):
    """ if standalone ESD -> AOD reconstruction
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class tauRecSeedJetCollection(JobProperty):
    """ jet collection used to seed tau reconstruction
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="AntiKt4LCTopoJets"

class tauRecToolsCVMFSPath(JobProperty):
    """ path to cvmfs file location
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="tauRecTools/R22_preprod"

class doTJVA(JobProperty):
    """ switch of TJVA """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doTJVA_Tiebreak(JobProperty):
    """ switch of TJVA Tiebreak """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class associateLRT(JobProperty):
    """ associate Large Radius Tracks with tau in TauTrackFinder """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class classifyLRT(JobProperty):
    """ classify Large Radius Tracks in tau track classifier """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class removeDuplicateCoreTracks(JobProperty):
    """Ignore core tracks already used in previous tau
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class useGhostTracks(JobProperty):
    """ use ghost-matched tracks in TauTrackFinder """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class ghostTrackDR(JobProperty):
    """ dR(tau,track) value beyond which tracks must be ghost-matched in TauTrackFinder """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.25

class tauRecRNNTrackClassification(JobProperty):
    """Run the RNN Track Classifier
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class tauRecRNNTrackClassificationConfig(JobProperty):
    """Config file for RNNTrackClassification
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="RNNTrackClassifier_2021-07-19_14-25-14_90_25_30.json"

class tauRecDecayModeNNClassifierConfig(JobProperty):
    """Config file for TauDecayModeNNClassifier
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='NNDecayMode_R22_v1.json'

class tauRecCalibrateLCConfig(JobProperty):
    """Config file for TauCalibrateLC
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='CaloTES_R22_Round2.5.root'

class tauRecMvaTESConfig(JobProperty):
    """Config file for >=1p taus for MvaTESEvaluator
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='MvaTES_R22_Round3.root'

class tauRec0pMvaTESConfig(JobProperty):
    """Config file for 0p taus for MvaTESEvaluator
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='MvaTES_0p_R22_Round3.root'

class tauRecCombinedTESConfig(JobProperty):
    """Config file for TauCombinedTES
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue='CombinedTES_R22_Round2.5.root'

class tauRecTauJetRNNConfig(JobProperty):
    """Config files for TauJetRNNEvaluator jet ID
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'tauid_rnn_1p_R22_v1.json', 'tauid_rnn_2p_R22_v1.json', 'tauid_rnn_3p_R22_v1.json' ]

class tauRecTauJetRNNWPConfig(JobProperty):
    """Config files for TauWPDecorator for RNN jet ID
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'tauid_rnnWP_1p_R22_v0.root', 'tauid_rnnWP_2p_R22_v0.root', 'tauid_rnnWP_3p_R22_v0.root' ]

class tauRecTauEleRNNConfig(JobProperty):
    """Config files for TauJetRNNEvaluator eVeto
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'taueveto_rnn_config_1P_r22.json', 'taueveto_rnn_config_3P_r22.json' ]

class tauRecTauEleRNNWPConfig(JobProperty):
    """Config files for TauWPDecorator for RNN eVeto
    """
    statusOn=True
    allowedTypes=[['string']]
    StoredValue=[ 'taueveto_rnn_flat_1P_r22.root', 'taueveto_rnn_flat_3P_r22.root' ]

class tauRecPi0ScoreConfig(JobProperty):
    """Config file for TauPi0ScoreCalculator
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="TauPi0BDTWeights.root"

class tauRecSeedMinPt(JobProperty):
    """ minimum seed jet pt
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=0.*Units.GeV

class tauRecSeedMaxEta(JobProperty):
    """ max allowed abs_eta of seed jet
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=2.5

class tauRecMinPt(JobProperty):
    """ minimum tau pt for >=1p taus (MVA TES calibration)
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=6.75*Units.GeV

class tauRec0pMinPt(JobProperty):
    """ minimum tau pt for 0p taus (MVA TES calibration)
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=9.25*Units.GeV

class tauRecMaxNTracks(JobProperty):
    """ maximum number of classifiedCharged tracks for a tau candidate
    """
    statusOn=True
    allowedTypes=['int']
    StoredValue=-1

class tauRecToolsDevToolList(JobProperty):
    """ add extra devTools to TauBuilderTool
    """
    statusOn=True
    allowedTypes=[[]]
    StoredValue=[]

class tauRecToolsDevToolListProcessor(JobProperty):
    """ add extra devTools TauProcessorTool
    """
    statusOn=True
    allowedTypes=[[]]
    StoredValue=[]
    
class doRunTauDiscriminant(JobProperty):
    """ switch for TauDiscriminant running
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPanTau(JobProperty):
    """ if pantau should run after tauRec
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doPi0(JobProperty):
    """ switch of Pi0 Finder """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class pi0EtCuts(JobProperty):
    """ Set |eta| dependent Et requirement for pi0 tag
    """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[2800.*Units.MeV,2700.*Units.MeV,2500.*Units.MeV,2900.*Units.MeV,2700.*Units.MeV]

class pi0MVACuts_1prong(JobProperty):
    """ Set |eta| dependent MVA requirement for pi0 tag for 1-prong taus
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[0.45,0.37,0.39,0.40,0.38]

class pi0MVACuts_mprong(JobProperty):
    """ Set |eta| dependent MVA requirement for pi0 tag for multi-prong taus
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[0.73,0.69,0.58,0.69,0.67]

class shotPtCut_1Photon(JobProperty):
    """ Set |eta| dependent pt requirement for counting one photon in a shot
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[430.*Units.MeV,300.*Units.MeV,9999999.*Units.MeV,330.*Units.MeV,350.*Units.MeV]

class shotPtCut_2Photons(JobProperty):
    """ Set |eta| dependent pt requirement for counting two photons in a shot
    """
    statusOn=True
    allowedTypes=['list']
    StoredValue=[10000.*Units.MeV,10000.*Units.MeV,9999999.*Units.MeV,10000.*Units.MeV,10000.*Units.MeV]

class useOldVertexFitterAPI(JobProperty):
    """ use the old (AOD-style) API of the AdaptiveVertexFitter.
    The AdaptiveVertexFitter is used for finding the tau decay vertex (aka. secondary vertex) and called in TauVertexVariables.
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class inAOD(JobProperty):
    """ indicate that the reconstruction is done in AOD. in R22 no jetSeed is stored some additional tweakings are needed
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# Defines a sub-container for the algorithm switches
class tauRecFlags(JobPropertyContainer):
    """ tau information """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecFlags)

# I want always the following flags in the Rec container  
_list_tau=[Enabled,doTauRec,isStandalone,tauRecSeedJetCollection,tauRecToolsCVMFSPath,doTJVA,doTJVA_Tiebreak,associateLRT,classifyLRT,removeDuplicateCoreTracks,useGhostTracks,ghostTrackDR,tauRecRNNTrackClassification,tauRecRNNTrackClassificationConfig,tauRecDecayModeNNClassifierConfig,tauRecCalibrateLCConfig,tauRecMvaTESConfig,tauRec0pMvaTESConfig,tauRecCombinedTESConfig,tauRecTauJetRNNConfig,tauRecTauJetRNNWPConfig,tauRecTauEleRNNConfig,tauRecTauEleRNNWPConfig,tauRecPi0ScoreConfig,tauRecSeedMinPt,tauRecSeedMaxEta,tauRecMinPt,tauRec0pMinPt,tauRecMaxNTracks,tauRecToolsDevToolList,tauRecToolsDevToolListProcessor,doRunTauDiscriminant,doPanTau,doPi0,pi0EtCuts,pi0MVACuts_1prong,pi0MVACuts_mprong,shotPtCut_1Photon,shotPtCut_2Photons,useOldVertexFitterAPI, inAOD]
for j in _list_tau: 
    jobproperties.tauRecFlags.add_JobProperty(j)
del _list_tau




tauFlags=jobproperties.tauRecFlags
#=======================================================================
    
