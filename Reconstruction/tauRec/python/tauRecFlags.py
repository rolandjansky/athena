# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

class tauRecToolsCVMFSPath(JobProperty):
    """ path to cvmfs file location
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="tauRecTools/00-02-00/"

#deprecated
class TauDiscriminantCVMFSPath(JobProperty):
    """ path to cvmfs file location
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="TauDiscriminant/02-00-09/"

class tauRecMVATrackClassification(JobProperty):
    """Run the MVA Track Classifier
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

#can set from the command line via:
# --preExec 'RAWtoESD:from tauRec.tauRecFlags import tauFlags;tauFlags.tauRecMVATrackClassificationConfig()[0]=["file", somenumber]'
class tauRecMVATrackClassificationConfig(JobProperty):
    """Config file for MVATrackClassification
    """
    statusOn=True
    allowedTypes=[[[]]]
    StoredValue=[ ["TMVAClassification_BDT_l1.weights.root", 0.0002], ["TMVAClassification_BDT_l2.weights.root", -0.0080], ["TMVAClassification_BDT_l3.r207.weights.root", 0.0005] ]

class tauRecSeedMaxEta(JobProperty):
    """ max allowed abs_eta of jet seed
    """
    statusOn=True
    allowedTypes=['float']
    StoredValue=2.5

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


class useVertexBasedConvFinder(JobProperty):
    """ switch for PhotonConversionVertex.cxx/h conversion veto
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class useNewPIDBasedConvFinder(JobProperty):
    """ switch for TauConversionTagger.cxx/h conversion veto
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
    """ switch on Pi0 Finder
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

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

class tauRecToolsLCCalibFile(JobProperty):
    """ TES calibration file for TauCalibrateLC
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="TES_MC16a_prelim.root"

class tauRecToolsCombP4Weights(JobProperty):
    """ Config file for CombinedP4FromRecoTaus
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="CalibLoopResult_v04-04.root"
    
class tauRecToolsMvaTESWeights(JobProperty):
    """ Weights file for MVA TES
    """
    statusOn=True
    allowedTypes=['string']
    StoredValue="MvaTES_20170207_v2_BDTG.weights.root"
    
class tauRecJetBDTConfig(JobProperty):
    """ Weights files for the tau/jet BDT evaluator
    """
    statusOn=True
    allowedTypes=[[[]]]
    StoredValue=[ ["vars2016_pt_gamma_1p_isofix.root", 0, 1], ["vars2016_pt_gamma_3p_isofix.root", 2, 1000] ]

class tauRecEleBDTConfig(JobProperty):
    """ Weights files for the tau/e BDT
    """
    statusOn=True
    allowedTypes=[[[]]]
    StoredValue=[ ["EleBDT1PBar.root", 1, 1.37], ["EleBDT1PEnd1.root", 1, 1.37, 2.0], ["EleBDT1PEnd23.root", 1, 2.0, 3.0] ]

class tauRecWPDecoratorJetBDTConfig(JobProperty):
    """ Flattening files for the tau/jet WP decorator
    """
    statusOn=True
    allowedTypes=[ [] ]
    StoredValue=["FlatJetBDT1Pv2.root", "FlatJetBDT3Pv2.root"]

class tauRecJetRNNConfig(JobProperty):
    """ Weights files for the tau/jet RNN evaluator
    """
    statusOn=True
    allowedTypes=[ [] ]
    StoredValue=["rnnid_prelim_config_deep_1p.json", "rnnid_prelim_config_deep_3p.json", 10, 6]

# Defines a sub-container for the algorithm switches
class tauRecFlags(JobPropertyContainer):
    """ tau information """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecFlags)

# I want always the following flags in the Rec container  
_list_tau=[Enabled,doTauRec,tauRecToolsCVMFSPath,TauDiscriminantCVMFSPath,tauRecMVATrackClassification,tauRecMVATrackClassificationConfig,tauRecSeedMaxEta,tauRecToolsDevToolList,tauRecToolsDevToolListProcessor,doRunTauDiscriminant,useVertexBasedConvFinder,useNewPIDBasedConvFinder,doPanTau,doPi0,pi0EtCuts,pi0MVACuts_1prong,pi0MVACuts_mprong,shotPtCut_1Photon,shotPtCut_2Photons,useOldVertexFitterAPI,tauRecToolsLCCalibFile,tauRecToolsCombP4Weights,tauRecToolsMvaTESWeights,tauRecJetBDTConfig,tauRecEleBDTConfig,tauRecWPDecoratorJetBDTConfig,tauRecJetRNNConfig]
for j in _list_tau: 
    jobproperties.tauRecFlags.add_JobProperty(j)
del _list_tau



#################################################
#### AOD flags ##################################
#                                               #
# only for re-running tau reco on AODs          #
# !not for normal reconstruction!               #
#################################################

class doUpdate(JobProperty):
    """ update the tau containers (if running in AODmode)
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class tauRecAODFlags(JobPropertyContainer):
    """ tau information if re-running on AODs """

# add the tau flags container to the top container 
jobproperties.add_Container(tauRecAODFlags)

# I want always the following flags in the Rec container  
_list_tau=[doUpdate]
for j in _list_tau: 
    jobproperties.tauRecAODFlags.add_JobProperty(j)
del _list_tau


tauFlags=jobproperties.tauRecFlags
#=======================================================================
    
