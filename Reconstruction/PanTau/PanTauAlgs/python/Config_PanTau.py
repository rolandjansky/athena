# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file PanTauAlgs/python/JobOptionFlags_PanTau.py
## @purpose Python module to hold common flags to configure PanTau
##

""" PanTauFlags
    Python module to hold flags to configure PanTau.

    From the python prompt:
    >>> from PanTauAlgs.JobOptionFlags_PanTau import panTauFlags

"""

__author__ = "C.Limbach"
__version__= "$Revision:  $"
__doc__    = "Config_PanTau"

__all__    = [ "Config_PanTau" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes


### ==========================================================
###
###          Flags to steer job options
###
### ==========================================================

### /////////////////////////
### Boolean values
### /////////////////////////

class UseDefaultCellBasedConfig(JobProperty):
    """ If this is true, the configuration for pi0s will be copied from cellbased"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True


class TauConstituents_UsePionMass(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True


class FeatureExtractor_UseEmptySeeds(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False


class TauConstituents_eflowRec_UseMomentumAsEnergy(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class TauConstituents_UseShrinkingCone(JobProperty):
    """ """
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False



### /////////////////////////
### Float values
### /////////////////////////

class eflowRec_Assoc_DeltaR(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.4

class TauConstituents_Types_DeltaRCore(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.2


class TauConstituents_MaxEta(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 9.9


class TauConstituents_PreselectionMinEnergy(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 500.


class TauConstituents_eflowRec_BDTThreshold_Pi0Neut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.04



#
# PanTau BDT Cut values --- CellBased
#

class DecayModeDeterminator_BDTCutValue_R10X_CellBased(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  =  0.52

class DecayModeDeterminator_BDTCutValue_R11X_CellBased(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  =  -0.33

class DecayModeDeterminator_BDTCutValue_R110_CellBased(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  =  0.47

class DecayModeDeterminator_BDTCutValue_R1XX_CellBased(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  =  -0.21

class DecayModeDeterminator_BDTCutValue_R30X_CellBased(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  =  -0.13

class DecayModeDeterminator_BDTCutValue_R3XX_CellBased(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  =  -0.08

#
# PanTau BDT Cut values --- eflowRec
#

class DecayModeDeterminator_BDTCutValue_R10X_eflowRec(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.15

class DecayModeDeterminator_BDTCutValue_R11X_eflowRec(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.60

class DecayModeDeterminator_BDTCutValue_R110_eflowRec(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.08

class DecayModeDeterminator_BDTCutValue_R1XX_eflowRec(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.03

class DecayModeDeterminator_BDTCutValue_R30X_eflowRec(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.25

class DecayModeDeterminator_BDTCutValue_R3XX_eflowRec(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = -0.23



### /////////////////////////
### String values
### /////////////////////////

class Name_TauRecContainer(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TauJets"


class Name_eflowRecContainer(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "eflowObjects_tauMode"


class Name_TrackParticleContainer(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TrackParticleCandidate"


class Name_PanTauSeedsContainer(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "PanTau_OutputSeeds"


#class Name_PanTauDetailsContainer(JobProperty):
#    """  """
#    statusOn     = True
#    allowedTypes = ['str']
#    StoredValue  = "PanTau_SeedDetails"


class ModeDiscriminator_ReaderOption(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "!Color:Silent"


class ModeDiscriminator_TMVAMethod(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "BDTG"



### /////////////////////////
### vector<float> values
### /////////////////////////


#
# Eta Binned    E T   C U T S
#

class TauConstituents_BinEdges_Eta(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [0.000, 0.800, 1.400, 1.500, 1.900, 9.900]


class TauConstituents_Selection_Neutral_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 2.1*Units.GeV, 2.5*Units.GeV, 2.6*Units.GeV, 2.4*Units.GeV, 1.9*Units.GeV  ]


class TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 2.1*Units.GeV, 2.5*Units.GeV, 2.6*Units.GeV, 2.4*Units.GeV, 1.9*Units.GeV  ]


class TauConstituents_Selection_Charged_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV  ]


class TauConstituents_Selection_OutNeut_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV  ]


class TauConstituents_Selection_OutChrg_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV, 1.0*Units.GeV  ]

# Temporary test neutral type with lower Et cut (-250 MeV)
class TauConstituents_Selection_NeutLowA_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 1.85*Units.GeV, 2.25*Units.GeV, 2.35*Units.GeV, 2.15*Units.GeV, 1.65*Units.GeV  ]

# Temporary test neutral type with lower Et cut (-500 MeV)
class TauConstituents_Selection_NeutLowB_EtaBinned_EtCut(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 1.6*Units.GeV, 2.0*Units.GeV, 2.1*Units.GeV, 1.9*Units.GeV, 1.4*Units.GeV  ]



class eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 2.5*Units.GeV, 2.5*Units.GeV, 1.9*Units.GeV, 2.5*Units.GeV, 2.3*Units.GeV  ]

class eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 2.5*Units.GeV, 2.5*Units.GeV, 2.5*Units.GeV, 2.5*Units.GeV, 2.5*Units.GeV  ]

#
# Eta Binned    P I 0 - B D T   C U T S
#

class CellBased_BinEdges_Eta(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [0.000, 0.800, 1.400, 1.500, 1.900, 9.900]


class CellBased_EtaBinned_Pi0MVACut_1prong(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [  0.46, 0.39, 0.51, 0.47, 0.54  ]


class CellBased_EtaBinned_Pi0MVACut_3prong(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [  0.47, 0.52, 0.60, 0.55, 0.50  ]



class eflowRec_BinEdges_Eta(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [0.000, 0.800, 1.400, 1.500, 1.900, 9.900]


class eflowRec_EtaBinned_Pi0MVACut_1prong(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [  0.09, 0.09, 0.09, 0.08, 0.05  ]


class eflowRec_EtaBinned_Pi0MVACut_3prong(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [  0.09, 0.09, 0.09, 0.09, 0.07  ]


#
# P T   B I N S
#

class ModeDiscriminator_BinEdges_Pt(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [10*Units.GeV, 100000*Units.GeV]



### /////////////////////////
### vector<string> values
### /////////////////////////

class Names_InputAlgorithms(JobProperty):
    """ Valid names are: CellBased and eflowRec """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["CellBased"]


class Names_ModeCases(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["1p0n_vs_1p1n",
                    "1p1n_vs_1pXn",
                    "3p0n_vs_3pXn"]


### ---> eflowRec BDT variables
class ModeDiscriminator_BDTVariableNames_eflowRec_1p0n_vs_1p1n(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Charged_Ratio_EtOverEtAllConsts",
                    "Basic_NPi0NeutConsts",
                    "Neutral_PID_BDTValues_EtSort_1",
                    "Combined_DeltaR1stNeutralTo1stCharged"]

class ModeDiscriminator_BDTVariableNames_eflowRec_1p1n_vs_1pXn(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Neutral_PID_BDTValues_BDTSort_2",
                    "Neutral_Ratio_EtOverEtAllConsts",
                    "Basic_NNeutralConsts",
                    "Neutral_HLV_SumM"]

class ModeDiscriminator_BDTVariableNames_eflowRec_3p0n_vs_3pXn(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Basic_NPi0NeutConsts",
                    "Neutral_PID_BDTValues_BDTSort_1",
                    "Charged_HLV_SumPt",
                    "Charged_Ratio_EtOverEtAllConsts",
                    "Neutral_Mean_DRToLeading_WrtEtAllConsts"]


### ---> CellBased BDT variables
class ModeDiscriminator_BDTVariableNames_CellBased_1p0n_vs_1p1n(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Neutral_PID_BDTValues_BDTSort_1",
                    "Neutral_Ratio_1stBDTEtOverEtAllConsts",
                    "Combined_DeltaR1stNeutralTo1stCharged",
                    "Charged_JetMoment_EtDRxTotalEt",
                    "Neutral_Shots_NPhotonsInSeed"]

class ModeDiscriminator_BDTVariableNames_CellBased_1p1n_vs_1pXn(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Neutral_PID_BDTValues_BDTSort_2",
                    "Neutral_HLV_SumM",
                    "Neutral_Ratio_EtOverEtAllConsts",
                    "Basic_NNeutralConsts",
                    "Neutral_Shots_NPhotonsInSeed"]

class ModeDiscriminator_BDTVariableNames_CellBased_3p0n_vs_3pXn(JobProperty):
    """  """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ["Neutral_Ratio_EtOverEtAllConsts",
                    "Neutral_PID_BDTValues_BDTSort_1",
                    "Charged_StdDev_Et_WrtEtAllConsts",
                    "Neutral_Shots_NPhotonsInSeed",
                    "Charged_HLV_SumM"]



##-----------------------------------------------------------------------------
## 2nd step
## Definition of the PanTau flag container
class Config_PanTau(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(Config_PanTau)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.Config_PanTau.add_JobProperty(UseDefaultCellBasedConfig)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_UsePionMass)
jobproperties.Config_PanTau.add_JobProperty(FeatureExtractor_UseEmptySeeds)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_PreselectionMinEnergy)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_eflowRec_UseMomentumAsEnergy)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_UseShrinkingCone)
jobproperties.Config_PanTau.add_JobProperty(eflowRec_Assoc_DeltaR)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Types_DeltaRCore)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_MaxEta)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_eflowRec_BDTThreshold_Pi0Neut)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R10X_CellBased)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R11X_CellBased)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R110_CellBased)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R1XX_CellBased)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R30X_CellBased)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R3XX_CellBased)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R10X_eflowRec)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R11X_eflowRec)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R110_eflowRec)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R1XX_eflowRec)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R30X_eflowRec)
jobproperties.Config_PanTau.add_JobProperty(DecayModeDeterminator_BDTCutValue_R3XX_eflowRec)
jobproperties.Config_PanTau.add_JobProperty(Name_TauRecContainer)
jobproperties.Config_PanTau.add_JobProperty(Name_eflowRecContainer)
jobproperties.Config_PanTau.add_JobProperty(Name_TrackParticleContainer)
jobproperties.Config_PanTau.add_JobProperty(Name_PanTauSeedsContainer)
#jobproperties.Config_PanTau.add_JobProperty(Name_PanTauDetailsContainer)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_ReaderOption)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_TMVAMethod)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_BinEdges_Eta)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_Neutral_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_Charged_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_OutNeut_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_OutChrg_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_NeutLowA_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(TauConstituents_Selection_NeutLowB_EtaBinned_EtCut)
jobproperties.Config_PanTau.add_JobProperty(eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_1prong)
jobproperties.Config_PanTau.add_JobProperty(eflowRec_Selection_Pi0Neut_EtaBinned_EtCut_3prong)
jobproperties.Config_PanTau.add_JobProperty(CellBased_BinEdges_Eta)
jobproperties.Config_PanTau.add_JobProperty(CellBased_EtaBinned_Pi0MVACut_1prong)
jobproperties.Config_PanTau.add_JobProperty(CellBased_EtaBinned_Pi0MVACut_3prong)
jobproperties.Config_PanTau.add_JobProperty(eflowRec_BinEdges_Eta)
jobproperties.Config_PanTau.add_JobProperty(eflowRec_EtaBinned_Pi0MVACut_1prong)
jobproperties.Config_PanTau.add_JobProperty(eflowRec_EtaBinned_Pi0MVACut_3prong)
jobproperties.Config_PanTau.add_JobProperty(Names_InputAlgorithms)
jobproperties.Config_PanTau.add_JobProperty(Names_ModeCases)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BinEdges_Pt)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BDTVariableNames_eflowRec_1p0n_vs_1p1n)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BDTVariableNames_eflowRec_1p1n_vs_1pXn)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BDTVariableNames_eflowRec_3p0n_vs_3pXn)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BDTVariableNames_CellBased_1p0n_vs_1p1n)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BDTVariableNames_CellBased_1p1n_vs_1pXn)
jobproperties.Config_PanTau.add_JobProperty(ModeDiscriminator_BDTVariableNames_CellBased_3p0n_vs_3pXn)



##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select PanTauFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from PanTauAlgs.PanTauFlags import panTauFlags
config_PanTau = jobproperties.Config_PanTau
