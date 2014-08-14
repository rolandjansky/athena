# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_SingleElectronStream
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Description: Here, all neccessary job flags for the Primary single electron DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Primary single electron DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties


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
# First define container for the Primary single electron DPD flags  
#=====================================================================
class PrimaryDPDFlags_SingleElectronStream(JobPropertyContainer):
    """ The PrimaryDPDMaker flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_SingleElectronStream)

#short-cut to get the PrimaryDPDFlags_SingleElectronStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_SingleElectronStream import primSingleElectronDPD'
#Note that name has to be different to avoid problems with pickle
primSingleElectronDPD = jobproperties.PrimaryDPDFlags_SingleElectronStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the Single Electron output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the SingleElectron output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the SingleElectron output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the SingleElectron output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ApplySlimming)


##--------------------------------
## Selections
##--------------------------------
class ElectronFilter(JobProperty):
    """  Set the properties of the low et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 25.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDMediumPP
    ElectronPtCone     = egammaParameters.ptcone20
    ElectronPtConeMax  = 0.1
    Prescale           = 80
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ElectronFilter)

class HighEtElectronFilter(JobProperty):
    """  Set the properties of the medium et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 150.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDLoosePP
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(HighEtElectronFilter)

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
    MinMass             = 30.0*Units.GeV
    Prescale            = 1
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ZeeFilter)

class WenuFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 25.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDMediumPP
    ElectronPtCone     = egammaParameters.ptcone20
    ElectronPtConeMax  = 0.1
    MissingEt          = 25.0*Units.GeV
    Prescale           = 20
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(WenuFilter)

class SemiLepTTBarFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 25.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDMediumPP
    ElectronPtCone     = egammaParameters.ptcone20
    ElectronPtConeMax  = 0.1
    MissingEt          = 25.0*Units.GeV
    JetCollection      = "AntiKt4TopoEMJets"
    JetPt              = 25.0*Units.GeV
    JetEta             = 2.5
    Prescale           = 10
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(SemiLepTTBarFilter)

class ElectronMuonFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 15.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDMediumPP
    RemoveCrack        = False
    MuonEt             = 15.0*Units.GeV
    RelativeMuonIso    = 0.2
    Prescale           = 5
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ElectronMuonFilter)

class ElectronTauFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 25.0*Units.GeV
    ElectronEtMax      = 45.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronIsEM       = egammaPID.ElectronIDTightPP
    ElectronPtCone     = egammaParameters.ptcone40
    ElectronPtConeMax  = 0.006
    #ElectronEtCone     = egammaParameters.etcone30
    ElectronEtCone     = egammaParameters.etcone30_ptcorrected
    ElectronEtConeMax  = 0.1
    TauNTrackMin       = 1
    TauNTrackMax       = 4
    TauEt              = 25.0*Units.GeV
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(ElectronTauFilter)

class DisappearingElectronFilter(JobProperty):
    """  Set the properties of the disappearing electron fitler."""
    statusOn          = True
    allowedTypes      = ['bool']
    StoredValue       = True
    ElectronEtCut     = 55.0*Units.GeV
    electronEtaCut    = 2.47
    MissingETCut      = 25.0*Units.GeV
    Prescale          = 1
    pass
jobproperties.PrimaryDPDFlags_SingleElectronStream.add_JobProperty(DisappearingElectronFilter)
