# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file PanTauAnalysis/python/PanTauFlags.py
## @purpose Python module to hold common flags to configure PanTau
##

""" PanTauFlags
    Python module to hold flags to configure the PanTau::JetBuilderAlg and its DiscriminantTools.

    From the python prompt:
    >>> from PanTauAnalysis.PanTauJetBuilderAlg_Flags import PanTauJetBuilderAlgFlags

"""

__author__ = "C.Limbach"
__version__= "$Revision: 500031 $"
__doc__    = "PanTauJetBuilderAlg_Flags"

__all__    = [ "PanTauJetBuilderAlg_Flags" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes


### ==========================================================
###
###          Flags for MVA part in JetBuilderAlg
###
### ==========================================================


class MVA_GeneralWeightFilePrefix(JobProperty):
    """Jet Building: Prefix of TMVA weight files"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "TrainModes"
#TrainModes_ET_15_30_1p0n_vs_1p1n1pXn3p0n3pXnFake_BDT


class MVA_VersionName(JobProperty):
    """Jet Building: Suffix of weight files (usefull to switch between different sets of weight files)"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ""


class MVA_PtBins(JobProperty):
    """Jet Building: Pt bins for the MVA"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ 15.* Units.GeV, 1000.* Units.GeV ]

class MVA_Method(JobProperty):
    """Jet Building: Name of the MVA method to be used"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "BDTBase"

### ### Prefixes for the different classes to be used

class MVA_ModeName1p0n(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "1p0n"

class MVA_ModeName1p1n(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "1p1n"

class MVA_ModeName1pXn(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "1pXn"

class MVA_ModeName3p0n(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "3p0n"

class MVA_ModeName3pXn(JobProperty):
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = "3pXn"


### ### List of variables to be used in the different classes

class MVA_UsedFeatures1p0n(JobProperty):
    """Jet Building: Feature Names in this class"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ "Eflow_QualChrg_Ratio_1stEtOverTypeEt", "Eflow_Pi0Neut_eflowPID_Pi0Val_1st_BDTSort", "Eflow_QualChrg_Ratio_EtOverEtQualEFOs", "Eflow_QualChrg_Mean_DRToLeading_WrtEtQualEFOs", "Eflow_QualChrg_StdDev_E_WrtEAll" ]

class MVA_UsedFeatures1p1n(JobProperty):
    """Jet Building: Feature Names in this class"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ "Eflow_Pi0Neut_eflowPID_Pi0Val_1st_BDTSort", "Eflow_QualChrg_Ratio_1stEtOverTypeEt", "Eflow_Pi0Neut_eflowPID_Pi0Val_2nd_BDTSort", "Basic_NQualChrgEFO", "Eflow_QualChrg_Mean_DRToLeading_WrtEtQualEFOs", "Eflow_QualChrg_Ratio_EtOverEtQualEFOs" ] # to be expanded

class MVA_UsedFeatures1pXn(JobProperty):
    """Jet Building: Feature Names in this class"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ "Eflow_QualChrg_Ratio_EtOverEtQualEFOs", "Eflow_Pi0Neut_eflowPID_Pi0Val_1st_BDTSort", "Eflow_QualChrg_Ratio_1stEtOverTypeEt", "Basic_NQualChrgEFO", "Eflow_Pi0Neut_eflowPID_Pi0Val_2nd_BDTSort" ]

class MVA_UsedFeatures3p0n(JobProperty):
    """Jet Building: Feature Names in this class"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ "Eflow_QualChrg_Mean_Et_WrtEtQualEFOs", "Eflow_QualChrg_Ratio_1stEtOverTypeEt", "Eflow_QualChrg_Ratio_EtOverEtQualEFOs", "Basic_NQualChrgEFO", "Eflow_QualChrg_Mean_DRToLeading_WrtEtQualEFOs", "Eflow_Pi0Neut_eflowPID_Pi0Val_1st_BDTSort", "Eflow_QualChrg_Ratio_1stEtOverEtAll" ]

class MVA_UsedFeatures3pXn(JobProperty):
    """Jet Building: Feature Names in this class"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ "Eflow_QualChrg_Ratio_1stEtOverTypeEt", "Eflow_QualChrg_Ratio_EtOverEtQualEFOs", "Basic_NQualChrgEFO", "Eflow_Pi0Neut_eflowPID_Pi0Val_1st_BDTSort", "Eflow_QualChrg_Mean_DRToLeading_WrtEtQualEFOs" ]




##-----------------------------------------------------------------------------
## 2nd step
## Definition of the PanTau flag container
class PanTauJetBuilderAlg_Flags(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(PanTauJetBuilderAlg_Flags)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_GeneralWeightFilePrefix)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_PtBins)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_Method)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_VersionName)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_ModeName1p0n)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_ModeName1p1n)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_ModeName1pXn)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_ModeName3p0n)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_ModeName3pXn)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_UsedFeatures1p0n)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_UsedFeatures1p1n)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_UsedFeatures1pXn)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_UsedFeatures3p0n)
jobproperties.PanTauJetBuilderAlg_Flags.add_JobProperty(MVA_UsedFeatures3pXn)



##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select PanTauFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from PanTauAnalysis.PanTauJetBuilderAlg_Flags import PanTauJetBuilderAlgFlags
PanTauJetBuilderAlgFlags = jobproperties.PanTauJetBuilderAlg_Flags
