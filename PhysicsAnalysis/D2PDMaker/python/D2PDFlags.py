# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        D2PDFlags
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2009
##
## Description: Here, all neccessary job flags for the D2PDMaker
##              are defined.
##
## $Id: D2PDFlags.py,v 1.2 2009/04/16 08:39:22 kkoeneke Exp $
##=============================================================================

__doc__ = """Here, all neccessary job flags for the D2PDMaker are defined."""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
from RecExConfig.RecoFunctions import AddValidItemToList,RemoveValidItemFromList

#=====================================================================
# First define container for the D2PDMaker flags  
#=====================================================================
class D2PDFlags(JobPropertyContainer):
    """ The D2PDMaker flag/job property  container."""
jobproperties.add_Container(D2PDFlags)

#short-cut to get the D2PDFlags container with this one line:
#'from D2PDMaker.D2PDFlags import D2PDFlags'
#Note that name has to be different to avoid problems with pickle
D2PDFlags = jobproperties.D2PDFlags

#=====================================================================
# Now define each flag and add it to the container
#=====================================================================

#------------------------
# Input/output
#------------------------
class OutputDirectoryName(JobProperty):
    """ Name of the directory where the output files are written. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D2PDFlags.add_JobProperty(OutputDirectoryName)

class OutputMiddleName(JobProperty):
    """ Keyword added in the middle of the output files names. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D2PDFlags.add_JobProperty(OutputMiddleName)

class OutputPoolRootFileNamePrefix(JobProperty):
    """ File name prefix for the pool.root output files."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""
    pass
jobproperties.D2PDFlags.add_JobProperty(OutputPoolRootFileNamePrefix)



class GRLFile(JobProperty):
    """ File name prefix for the pool.root output files."""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "http://atlasdqm.web.cern.ch/atlasdqm/grlgen/StandardModel/WZjets_allchannels_v4/data10_7TeV.periodAllYear_DetStatus-v03-pro05_WZjets_allchannels.xml"
    pass
jobproperties.D2PDFlags.add_JobProperty(GRLFile)



##--------------------------------
## Flags to turn on/off each possible DPD
## Note: each DPD has its own internal flags.
##--------------------------------
listAODtoD2PD=[]
listESDtoD2PD=[]

class WriteExampleSimpleZeeStream(JobProperty):
    """ Produce the D2PD simple Z->ee DPD."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "D2AODM_ZEE"
    FileName       = ""
    DPDMakerScript = "D2PDMaker/D2PD_ExampleSimpleZee.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteExampleSimpleZeeStream)
listAODtoD2PD.append(WriteExampleSimpleZeeStream.StreamName)

class WriteExampleSimpleZmumuStream(JobProperty):
    """ Produce the D2PD simple Z->mumu DPD."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "D2AODM_ZMUMU"
    FileName       = ""
    DPDMakerScript = "D2PDMaker/D2PD_ExampleSimpleZmumu.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteExampleSimpleZmumuStream)
listAODtoD2PD.append(WriteExampleSimpleZmumuStream.StreamName)

class WriteExampleSimpleWmunuStream(JobProperty):
    """ Produce the D2PD simple W->mu nu DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "D2AODM_WMUNU"
    FileName     = ""
    DPDMakerScript="D2PDMaker/D2PD_ExampleSimpleWmunu.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteExampleSimpleWmunuStream)
listAODtoD2PD.append(WriteExampleSimpleWmunuStream.StreamName)

class WriteExampleSimpleH4lStream(JobProperty):
    """ Produce the D2PD simple H -> ZZ* -> 4l DPD."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "D2AOD_H4L"
    FileName     = ""
    DPDMakerScript="D2PDMaker/D2PD_ExampleSimpleH4l.py"
    ApplyThinning      = True
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteExampleSimpleH4lStream)
listAODtoD2PD.append(WriteExampleSimpleH4lStream.StreamName)

class WriteDAOD_SUSYEGammaStream(JobProperty):
    """Produce DAOD_SUSYEGAMMA"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "DAOD_SUSYEGAMMA"
    FileName     = ""
    DPDMakerScript="D2PDMaker/DAOD_SUSYEGamma.py"
    CutVersion = 'newest' #also: OldV1
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_SUSYEGammaStream)
listAODtoD2PD.append(WriteDAOD_SUSYEGammaStream.StreamName)

class WriteDAOD_SUSYMuonsStream(JobProperty):
    """Produce DAOD_SUSYMUONS"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "DAOD_SUSYMUONS"
    FileName     = ""
    DPDMakerScript="D2PDMaker/DAOD_SUSYMuons.py"
    CutVersion = 'newest' #also: OldV1
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_SUSYMuonsStream)
listAODtoD2PD.append(WriteDAOD_SUSYMuonsStream.StreamName)

class WriteDAOD_SUSYJetsStream(JobProperty):
    """Produce DAOD_SUSYJETS"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "DAOD_SUSYJETS"
    FileName     = ""
    DPDMakerScript="D2PDMaker/DAOD_SUSYJets.py"
    CutVersion = 'newest' #not used yet
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_SUSYJetsStream)
listAODtoD2PD.append(WriteDAOD_SUSYJetsStream.StreamName)

class WriteD2AOD_ZEEStream(JobProperty):
    """Produce D2AOD_ZEE"""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    StreamName         = "D2AOD_ZEE"
    FileName           = ""
    DPDMakerScript     = "D2PDMaker/D2PD_ZeeStream.py"
    ApplyThinning      = True
    ApplySlimming      = True
    CreateControlPlots = False
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteD2AOD_ZEEStream)
listAODtoD2PD.append(WriteD2AOD_ZEEStream.StreamName)

class WriteD2AOD_WENUStream(JobProperty):
    """Produce D2AOD_WENU"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "D2AOD_WENU"
    FileName     = ""
    DPDMakerScript="D2PDMaker/D2PD_WenuStream.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteD2AOD_WENUStream)
listAODtoD2PD.append(WriteD2AOD_WENUStream.StreamName)

class WriteD2ESDM_TRKJET(JobProperty):
    """Produce D2ESDM_TRKJET"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "D2ESDM_TRKJET"
    FileName     = ""
    DPDMakerScript="D2PDMaker/D2ESDM_TRKJET.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteD2ESDM_TRKJET)
listESDtoD2PD.append(WriteD2ESDM_TRKJET.StreamName)

class WriteDAOD_RNDMStream(JobProperty):
    """Produce DAOD_RNDM"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "DAOD_RNDM"
    FileName     = ""
    DPDMakerScript="D2PDMaker/DAOD_RNDM.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_RNDMStream)
listAODtoD2PD.append(WriteDAOD_RNDMStream.StreamName)

class WriteDAOD_ElLoose18Stream(JobProperty):
    """Produce DAOD_ELLOOSE18"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "DAOD_ELLOOSE18"
    FileName     = ""
    DPDMakerScript="D2PDMaker/DAOD_ElLoose18.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_ElLoose18Stream)
listAODtoD2PD.append(WriteDAOD_ElLoose18Stream.StreamName)

class WriteDAOD_H4lBremRecStream(JobProperty):
    """Produce DAOD_H4LBREMREC"""
    statusOn         = True
    allowedTypes     = ['bool']
    StoredValue      = False
    StreamName       = "DAOD_H4LBREMREC"
    FileName         = ""
    DPDMakerScript   = "D2PDMaker/DAOD_H4lBremRec.py"
    inputIsSimulation               = False
    DoBremRec                       = False
    ApplySkimming                   = True
    ApplyThinning                   = True
    ApplyMuonTruthMatching          = True
    ApplyElectronTruthMatching      = True
    ApplyPhotonTruthMatching        = True
    ApplyTrackParticleTruthMatching = False
    ApplyCaloClusterTruthMatching   = False
    ApplyElectronTrackIsolation     = False
    ApplyElectronCaloIsolation      = False
    ApplyCaloClusterTrackMatching   = False
    ApplyTrackCaloClusterMatching   = False
    WriteMCTruth                    = False
    WriteTrkTracks                  = True # Needed to see if muons pass
    WriteMuonTrkTracks              = True # Needed to see if muons pass
    WriteGSFTrkTracks               = False
    WriteHLTResult_EF               = False
    muonColl                        = 'StacoMuonCollection'
    MuonTriggerList  = [ "EF_mu18_MG",
                         "EF_mu18_MG_medium",
                         "EF_mu20_MG",
                         "EF_mu40_MSonly",
                         "EF_2mu10_loose",
                         "EF_mu24i_tight", #2012
                         "EF_mu36_tight", #2012
                         "EF_2mu13", #2012
                         "EF_mu18_tight_mu8_EFFS" #2012
                         ]
    ElectronTriggerList  = [ "EF_e20_medium",
                             "EF_e20_medium1",
                             "EF_e22_medium",
                             "EF_e22_medium1",
                             "EF_e22vh_medium1",
                             "EF_2e12_medium",
                             "EF_2e12T_medium",
                             "EF_2e12Tvh_medium",
                             "EF_e24vhi_medium1", #2012
                             "EF_e60_medium1", #2012
                             "EF_2e12Tvh_loose1" #2012
                             ]
    ElectronMuonTriggerList = [ "EF_e10_medium_mu6", #2011
                                "EF_e12Tvh_medium1_mu8", #2012
                                "EF_e24vhi_loose1_mu8" #2012
                                ]
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_H4lBremRecStream)
listAODtoD2PD.append(WriteDAOD_H4lBremRecStream.StreamName)

class WriteDAOD_HighMDiJetStream(JobProperty):
    """Produce DAOD_HIGHMDIJET"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "DAOD_HIGHMDIJET"
    FileName     = ""
    DPDMakerScript="D2PDMaker/DAOD_HighMDiJet.py"
    JetCollections = [ "AntiKt4TopoEMJets", "AntiKt6TopoEMJets", "AntiKt4LCTopoJets", "AntiKt6LCTopoJets" ]
    TriggerList  = [ "EF_L1J250_NoAlg",
                     "EF_L1FJ75_NoAlg",
                     "EF_j180_a4tc_EFFS",
                     "EF_j240_a4tc_EFFS",
                     "EF_j240_a10tc_EFFS",
                     "EF_j300_a10tc_EFFS",
                     "EF_j400_a10tc_EFFS" ]
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_HighMDiJetStream)
listAODtoD2PD.append(WriteDAOD_HighMDiJetStream.StreamName)

class WriteDAODM_HighMDiJetStream(JobProperty):
    """Produce DAODM_HIGHMDIJET"""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "DAODM_HIGHMJJ"
    FileName       = ""
    DPDMakerScript = "D2PDMaker/DAODM_HighMDiJet.py"
    ApplyThinning  = True
    ApplySlimming  = True
    JetCollections = [ "AntiKt4TopoEMJets", "AntiKt6TopoEMJets", "AntiKt4LCTopoJets", "AntiKt6LCTopoJets" ]
    TriggerList    = [ "EF_L1J250_NoAlg",
                       "EF_L1FJ75_NoAlg",
                       "EF_j180_a4tc_EFFS",
                       "EF_j240_a4tc_EFFS",
                       "EF_j240_a10tc_EFFS",
                       "EF_j300_a10tc_EFFS",
                       "EF_j400_a10tc_EFFS" ]
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAODM_HighMDiJetStream)
listAODtoD2PD.append(WriteDAODM_HighMDiJetStream.StreamName)

class WriteDAOD_2LHSG2Stream(JobProperty):
    """Produce DAOD_2LHSG2"""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_2LHSG2"
    FileName       = ""
    DPDMakerScript = "HSG2DPDUtils/hsg2_daodmaker_2lepton.prodJobOFragment.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_2LHSG2Stream)
listAODtoD2PD.append(WriteDAOD_2LHSG2Stream.StreamName)

class WriteDAOD_4LHSG2Stream(JobProperty):
    """Produce DAOD_4LHSG2"""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_4LHSG2"
    FileName       = ""
    DPDMakerScript = "HSG2DPDUtils/hsg2_daodmaker_4lepton.prodJobOFragment.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_4LHSG2Stream)
listAODtoD2PD.append(WriteDAOD_4LHSG2Stream.StreamName)

class WriteDAOD_2L2QHSG2Stream(JobProperty):
    """Produce DAOD_2L2QHSG2"""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_2L2QHSG2"
    FileName       = ""
    DPDMakerScript = "HSG2DPDUtils/hsg2_daodmaker_2L2Q.prodJobOFragment.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_2L2QHSG2Stream)
listAODtoD2PD.append(WriteDAOD_2L2QHSG2Stream.StreamName)

class WriteDAOD_JPSIHSG2Stream(JobProperty):
    """Produce DAOD_JPSIHSG2"""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    StreamName     = "StreamDAOD_JPSIHSG2"
    FileName       = ""
    DPDMakerScript = "HSG2DPDUtils/hsg2_daodmaker_Jpsi.prodJobOFragment.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_JPSIHSG2Stream)
listAODtoD2PD.append(WriteDAOD_JPSIHSG2Stream.StreamName)

class WriteDAOD_HSG1Stream(JobProperty):
    """Produce DAOD_HSG1"""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    StreamName         = "DAOD_HSG1"
    FileName           = ""
    DPDMakerScript     = "HSG1DPDUtils/DAOD_HSG1.py"
    ApplyThinning      = True
    ApplySlimming      = True
    CreateControlPlots = False
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_HSG1Stream)
listAODtoD2PD.append(WriteDAOD_HSG1Stream.StreamName)

class WriteDAOD_HWWStream(JobProperty):
    """Produce DAOD_HWW"""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    StreamName         = "DAOD_HWW"
    FileName           = ""
    DPDMakerScript     = "HSG3DPDUtils/DAOD_HWW.py"
    pass
jobproperties.D2PDFlags.add_JobProperty(WriteDAOD_HWWStream)
listAODtoD2PD.append(WriteDAOD_HWWStream.StreamName)



#class WriteDAODM_MYFIRSTStream(JobProperty):
#    """Produce DAODM_MYFIRST"""
#    statusOn           = True
#    allowedTypes       = ['bool']
#    StoredValue        = False
#    StreamName         = "DAODM_MYFIRST"
#    FileName           = ""
#    DPDMakerScript     = "MyNewPackage/MyFirstDAOD.py"
#    ApplyThinning      = True
#    ApplySlimming      = True
#    CreateControlPlots = False
#    pass
#jobproperties.D2PDFlags.add_JobProperty(WriteDAODM_MYFIRSTStream)
#listAODtoD2PD.append(WriteDAODM_MYFIRSTStream.StreamName)



class InputSUSYJetCollection(JobProperty):
    """Input Jet collection of SUSYFilters"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "AntiKt4H1TopoJets" #<--r15, for r16: AntiKt4TopoJets  
    pass
jobproperties.D2PDFlags.add_JobProperty(InputSUSYJetCollection)

class VerboseLevel(JobProperty):
    """Input Jet collection of SUSYFilters"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3 #3==INFO, 2==DEBUG
    pass
jobproperties.D2PDFlags.add_JobProperty(VerboseLevel)
