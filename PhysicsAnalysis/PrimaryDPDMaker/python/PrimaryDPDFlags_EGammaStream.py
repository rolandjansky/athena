# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_EGammaStream
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Description: Here, all neccessary job flags for the Primary EGamma DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Primary EGamma DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units


# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters



#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_EGammaStream(JobPropertyContainer):
    """ The Primary EGamma DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_EGammaStream)

#short-cut to get the PrimaryDPDFlags container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_EGammaStream import primEGammaDPD'
#Note that name has to be different to avoid problems with pickle
primEGammaDPD = jobproperties.PrimaryDPDFlags_EGammaStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the EGamma output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """ Apply the skimming (event selection) for the EGamma output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """ Apply the thinning for the EGamma output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """ Apply the skimming (event selection) for the EGamma output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplySlimming)

##--------------------------------
## Sub-options for the EGamma DPD output stream
##--------------------------------
class ApplyCaloCellThinning(JobProperty):
    """ Apply the thinning of the calorimeter cells (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyCaloCellThinning)

class ApplyPixelClusterThinning(JobProperty):
    """ Apply the thinning of the pixel clusters (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyPixelClusterThinning)

class ApplySCTClusterThinning(JobProperty):
    """ Apply the thinning of the SCT clusters (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplySCTClusterThinning)

class ApplyTRTDriftCircleThinning(JobProperty):
    """ Apply the thinning of the TRT_DriftCircles (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyTRTDriftCircleThinning)

class ApplyEGammaTrackThinning(JobProperty):
    """ Apply the thinning of the Tracks (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyEGammaTrackThinning)

class ApplyMuonTrackThinning(JobProperty):
    """ Apply the thinning of the Tracks (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyMuonTrackThinning)

class ApplyTauTrackThinning(JobProperty):
    """ Apply the thinning of the Tracks (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyTauTrackThinning)

class ApplyTriggerTowerThinning(JobProperty):
    """ Apply the thinning of L1Calo Trigger Towers for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ApplyTriggerTowerThinning)


##--------------------------------
## Other stream content
##--------------------------------
class WriteCaloCells(JobProperty):
    """ Write out the ESD calorimeter cells."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(WriteCaloCells)

class WriteIdPrepRawData(JobProperty):
    """ Write the ESD PrepRawData."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(WriteIdPrepRawData)

class UseAODTruth(JobProperty):
    """ Write the AOD Truth containers instead of the ESD ones."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(UseAODTruth)


##--------------------------------
## Selections
##--------------------------------

# The trigger filters
class JpsiTriggerFilter(JobProperty):
    """  Set the properties of the J/psi trigger fitler."""
    statusOn         = True
    allowedTypes     = ['bool']
    StoredValue      = True
    TriggerNames     = [ 
                         [ "EF_e5_tight1_e4_etcut_Jpsi",       1 ],  # 1.3 Hz @10^33
                         [ "EF_e5_tight1_e9_etcut_Jpsi",       1 ],  # 1.6 Hz @10^33
                         [ "EF_e5_tight1_e14_etcut_Jpsi",      1 ],
                         [ "EF_e9_tight1_e4_etcut_Jpsi",       1 ],
                         [ "EF_e14_tight1_e4_etcut_Jpsi",      1 ], 
                         [ "EF_2e5_tight1_Jpsi",               1 ] ]  # 0.2 Hz @10^33
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(JpsiTriggerFilter)

class ZeeFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn            = True
    allowedTypes        = ['bool']
    StoredValue         = False
    Electron1Et         = 15.0*Units.GeV
    VetoElectron1Author = [ egammaParameters.AuthorFrwd ]
    Electron1IsEM       = egammaPID.ElectronIDLoosePP
    Electron2Et         = 15.0*Units.GeV
    VetoElectron2Author = [ egammaParameters.AuthorFrwd ]
    Electron2IsEM       = egammaPID.ElectronIDLoosePP
    #MinMass             = 30.0*Units.GeV
    Prescale            = 1
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(ZeeFilter)

class TagAndProbeElectronFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn                = True
    allowedTypes            = ['bool']
    StoredValue             = True
    TagElectronEt           = 25.0*Units.GeV
    TagVetoElectronAuthor   = [ egammaParameters.AuthorFrwd ]
    TagElectronIsEM         = egammaPID.ElectronIDTightPP
    TagElectronPtCone       = egammaParameters.ptcone30
    TagElectronPtConeMax    = 0.1
    ProbeElectronEt         = 15.0*Units.GeV
    ProbeElectronAuthor     = [ egammaParameters.AuthorFrwd ]
    #ProbeElectronIsEM       = egammaPID.ElectronIDTight
    MinMass                 = 60.0*Units.GeV
    Prescale                = 1
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(TagAndProbeElectronFilter)

class DiPhotonFilter(JobProperty):
    """  Set the properties of the electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = False
    PhotonEt       = 20.0*Units.GeV
    PhotonEta      = 1000.0
    PhotonAuthor   = 0
    PhotonIsEM     = egammaPID.PhotonIDLoose
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(DiPhotonFilter)

class PhotonElectronFilter(JobProperty):
    """  Set the properties of the electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    ElectronEt         = 20.0*Units.GeV
    ElectronEta        = 1000.0
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDLoosePP
    PhotonEt           = 20.0*Units.GeV
    PhotonEta          = 1000.0
    PhotonAuthor       = 0
    #    PhotonAuthor       = egammaParameters.AuthorPhoton
    PhotonIsEM         = egammaPID.PhotonIDLoose
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(PhotonElectronFilter)

class DiElectronFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = False
    ElectronEt         = 20.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDTightPP
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_EGammaStream.add_JobProperty(DiElectronFilter)


