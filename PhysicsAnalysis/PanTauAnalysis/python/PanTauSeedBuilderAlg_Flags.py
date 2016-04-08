# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file PanTauAnalysis/python/PanTauSeedBuilderAlg_Flags.py
## @purpose Python module to hold the flags that configure the TauSeedBuilderAlg
##

""" PanTauSeedBuilderAlg_Flags
    Python module to hold flags to configure PanTau.

    From the python prompt:
    >>> from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilerAlgFlags

"""

__author__ = "C.Limbach"
__version__= "$Revision: 500031 $"
__doc__    = "PanTauSeedBuilderAlg_Flags"

__all__    = [ "PanTauSeedBuilderAlg_Flags" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes


class ContainerName_eflowJets(JobProperty):
    """Name of SG container with jets consisting of EFOs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "EflowCone4Jets"

class ContainerName_eflowObjectsFromTaus(JobProperty):
    """Name of SG container with efos created from tauRec taus"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "eflowObjects_tauMode"

class ContainerName_tauRec(JobProperty):
    """Name of SG container with tauRec taus"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TauJets"

class ContainerName_PrimaryVertices(JobProperty):
    """Name of SG container with primary vertices"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "VxPrimaryCandidate"

class ContainerName_TruthTaus(JobProperty):
    """Name of SG container with truth taus"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TruthTaus"

class ContainerName_PanTauGoodOutputSeeds(JobProperty):
    """Name of SG container that is used by the JetBuilderAlg and contains the good pantau seeds"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "PanTauSeeds"

class ContainerName_PanTauBadOutputSeeds(JobProperty):
    """Name of SG container that is used by the JetBuilderAlg and contains the bad pantau seeds"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "PanTauRejectedSeeds"

class ContainerName_TruePi0MatchedNeutralEFOs(JobProperty):
    """Name of SG container with efos that have been matched to neutral pions"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "EFOsMatchedToNeutralPions"
        


# #############################################################################################
# Basic Config flags

class Config_UseTauRecAsSeeds(JobProperty):
    """Use the tauRec container rather than the eflowJet container as input"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Config_EfoCollectionDeltaR(JobProperty):
    """in the case of UseTauRecAsSeeds=true, this is the collection deltaR"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.4

class Config_DeltaRCoreRegion(JobProperty):
    """DeltaR of core region used to calculate the 4 momentum"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.2

class Config_DoTauRecVicinityCheck(JobProperty):
    """flag to turn tauRec vicinity check on/off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Config_DeltaRTauRecVicinityCheck(JobProperty):
    """property to set deltaR of vicinitychecktool"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.4

class Config_KeepOverlapTauRecVicinity(JobProperty):
    """property to set deltaR of vicinitychecktool"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Config_DoRunTimeHists(JobProperty):
    """Flag to switch creation of root file with runtime hists on/off"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# #############################################################################################
# Flags for the seed preselection


class Config_Presel_DoPreselection(JobProperty):
    """flag to tell whether the seed preselection will be executed or not"""
    statusOn    = True
    allowedType = ['bool']
    StoredValue = True

class Config_Presel_MaxAbsEta(JobProperty):
    """Maximal seed eta"""
    statusOn     = True
    allowedType  = ['float']
    StoredValue  = 2.7

class Config_Presel_MinNumberChargedComp(JobProperty):
    """Minimal number of charged components in seed"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class Config_Presel_MaxNumberChargedComp(JobProperty):
    """Maximal number of charged components in seed"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 7

class Config_Presel_MinAbsSeedCharge(JobProperty):
    """Minimal absolute charge of seed"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0

class Config_Presel_MaxAbsSeedCharge(JobProperty):
    """Maximal absolute charge of seed"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 5

class Config_Presel_MinSeedPt(JobProperty):
    """Minimal Pt of seed"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 15.* Units.GeV



# #############################################################################################
# Flags for eflowObject selection


class Config_EfoSel_DoEfoSelection(JobProperty):
    """Minimal ET of eflow objects"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Config_EfoSel_MinEflowObjectEt(JobProperty):
    """Minimal ET of eflow objects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0* Units.GeV


class Config_EfoSel_EtaBins(JobProperty):
    """Seed Building: Eta Bin edges for eta dependent Et cut on PiNeutral EFOs"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 0.0, 0.8, 1.35, 1.55, 2.5 ]
    #bin edges from Benedict

class Config_EfoSel_EtaBinEtCuts(JobProperty):
    """Seed Building: Et cuts on Pi0Neutral EFOs in the different Eta bins"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 1.5* Units.GeV, 1.5* Units.GeV, 1.5* Units.GeV, 2.0* Units.GeV]
    # et cuts guessed at the moment...


class Config_EfoSel_Pi0EtBins(JobProperty):
    """Seed Building: EtBin edges for the et dependent cut on pi0 BDT"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 0.0, 100* Units.GeV ]
    #bin edges from Benedict

class Config_EfoSel_Pi0EtBinBDTCuts(JobProperty):
    """Seed Building: Pi0 BDT cuts on neutral efos in the different ET bins"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ -0.04]
    # et cuts guessed at the moment...


class Config_EfoSel_MaxEflowObjectEta(JobProperty):
    """Max abs(eta) of eflow objects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 2.5

class Config_EfoSel_MinHADNeutralEt(JobProperty):
    """Minimum Et of hadronic neutral eflow objets"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.* Units.GeV

class Config_EfoSel_MinEMNeutralEt(JobProperty):
    """Minimum EMNeutral ET of neutral eflow objects (MeV)"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.* Units.GeV

class Config_EfoSel_MinEMNeutralE(JobProperty):
    """Minimum EMNeutral E of neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.* Units.GeV

class Config_EfoSel_MinPi0NeutralE(JobProperty):
    """Minimum EMNeutral E of neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.* Units.GeV

class Config_EfoSel_MinPi0NeutralEt(JobProperty):
    """Minimum EMNeutral E of neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 2.5* Units.GeV

class Config_EfoSel_UseEMNeutralHadronicVeto(JobProperty):
    """use hadronic veto to define EM neutrals"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Config_EfoSel_ApplyEMNeutralECut(JobProperty):
    """Apply E cut on EM neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Config_EfoSel_ApplyEMNeutralEtCut(JobProperty):
    """Apply Et cut on EM neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Config_EfoSel_ApplyPi0NeutralECut(JobProperty):
    """Apply Et cut on EM neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Config_EfoSel_ApplyPi0NeutralEtCut(JobProperty):
    """Apply Et cut on EM neutral eflow objects"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Config_EfoSel_ApplyPionMassHypothesis(JobProperty):
    """Apply the pion mass hypotheses to eflowObjects"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Config_EfoSel_PiZeroBDTThreshold(JobProperty):
    """Apply the pion mass hypotheses to eflowObjects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.04

class Config_EfoSel_TrackMatch_DeltaEta(JobProperty):
    """Apply the pion mass hypotheses to eflowObjects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0001

class Config_EfoSel_TrackMatch_DeltaPhi(JobProperty):
    """Apply the pion mass hypotheses to eflowObjects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0001

class Config_EfoSel_TrackMatch_RelEMatch(JobProperty):
    """Apply the pion mass hypotheses to eflowObjects"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.10


##-----------------------------------------------------------------------------
## 2nd step
## Definition of the PanTau flag container
class PanTauSeedBuilderAlg_Flags(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(PanTauSeedBuilderAlg_Flags)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_eflowJets)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_eflowObjectsFromTaus)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_tauRec)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_PrimaryVertices)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_TruthTaus)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_PanTauGoodOutputSeeds)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_PanTauBadOutputSeeds)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(ContainerName_TruePi0MatchedNeutralEFOs)

jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_UseTauRecAsSeeds)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoCollectionDeltaR)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_DeltaRCoreRegion)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_DoTauRecVicinityCheck)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_DeltaRTauRecVicinityCheck)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_KeepOverlapTauRecVicinity)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_DoRunTimeHists)

jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_DoPreselection)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_MaxAbsEta)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_MinNumberChargedComp)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_MaxNumberChargedComp)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_MinAbsSeedCharge)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_MaxAbsSeedCharge)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_Presel_MinSeedPt)

jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_DoEfoSelection)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_EtaBins)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_EtaBinEtCuts)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_Pi0EtBins)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_Pi0EtBinBDTCuts)
    
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MinEflowObjectEt)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MaxEflowObjectEta)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MinHADNeutralEt)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MinEMNeutralEt)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MinEMNeutralE)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MinPi0NeutralEt)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_MinPi0NeutralE)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_UseEMNeutralHadronicVeto)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_ApplyEMNeutralECut)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_ApplyEMNeutralEtCut)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_ApplyPi0NeutralECut)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_ApplyPi0NeutralEtCut)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_ApplyPionMassHypothesis)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_PiZeroBDTThreshold)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_TrackMatch_DeltaEta)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_TrackMatch_DeltaPhi)
jobproperties.PanTauSeedBuilderAlg_Flags.add_JobProperty(Config_EfoSel_TrackMatch_RelEMatch)


##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select PanTauFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags
PanTauSeedBuilderAlgFlags = jobproperties.PanTauSeedBuilderAlg_Flags
