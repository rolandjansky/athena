# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_DiPhotonStream
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
class PrimaryDPDFlags_DiPhotonStream(JobPropertyContainer):
    """ The PrimaryDPDMaker flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_DiPhotonStream)

#short-cut to get the PrimaryDPDFlags_DiPhotonStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_DiPhotonStream import primDiPhotonDPD'
#Note that name has to be different to avoid problems with pickle
primDiPhotonDPD = jobproperties.PrimaryDPDFlags_DiPhotonStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the Single Electron output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the DiPhoton output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the DiPhoton output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the DiPhoton output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(ApplySlimming)


##--------------------------------
## Selections for D2ESD
##--------------------------------
class DiPhotonFilter(JobProperty):
    """  Set the properties of the electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    PhotonEt       = 20.0*Units.GeV
    PhotonEta      = 1000.0
    PhotonAuthor   = 0
    PhotonID       = egammaPID.PhotonIDLoose
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(DiPhotonFilter)

class PhotonElectronFilter(JobProperty):
    """  Set the properties of the electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 20.0*Units.GeV
    ElectronEta        = 1000.0
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronID         = egammaPID.ElectronIDTightPP
    PhotonEt           = 20.0*Units.GeV
    PhotonEta          = 1000.0
    PhotonAuthor       = 0
    #    PhotonAuthor       = egammaParameters.AuthorPhoton
    PhotonID           = egammaPID.PhotonIDLoose
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(PhotonElectronFilter)

class DiElectronFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 20.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronID         = egammaPID.ElectronIDTightPP
    RemoveCrack        = False
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(DiElectronFilter)




##--------------------------------
## Selections for D2AOD
##--------------------------------
class D2AODDiPhotonFilter(JobProperty):
    """  Set the properties of the electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    PhotonEt       = 20.0*Units.GeV
    PhotonEta      = 1000.0
    PhotonAuthor   = 0
    PhotonID       = egammaPID.PhotonIDLoose
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(D2AODDiPhotonFilter)

class D2AODPhotonElectronFilter(JobProperty):
    """  Set the properties of the electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 20.0*Units.GeV
    ElectronEta        = 1000.0
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronID         = egammaPID.ElectronIDMediumPP
    PhotonEt           = 20.0*Units.GeV
    PhotonEta          = 1000.0
    PhotonAuthor       = 0
    #PhotonAuthor       = egammaParameters.AuthorPhoton
    PhotonID           = egammaPID.PhotonIDLoose
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(D2AODPhotonElectronFilter)

class D2AODDiElectronFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 20.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronID         = egammaPID.ElectronIDMediumPP
    RemoveCrack        = False
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_DiPhotonStream.add_JobProperty(D2AODDiElectronFilter)
