# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_PhotonJetStream
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Description: Here, all neccessary job flags for the Primary photon-jet DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Primary Photon-Jet DPD are defined.
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
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_PhotonJetStream(JobPropertyContainer):
    """ The Primary Photon-Jet DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_PhotonJetStream)

#short-cut to get the PrimaryDPDFlags_PhotonJetStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_PhotonJetStream import primPhotonJetDPD'
#Note that name has to be different to avoid problems with pickle
primPhotonJetDPD=jobproperties.PrimaryDPDFlags_PhotonJetStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the Photon-Jet output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the Photon-Jet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Photon-Jet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the Photon-Jet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(ApplySlimming)




##--------------------------------
## Selections
##--------------------------------
class TriggerFilter(JobProperty):
    """  Set the properties of the low et electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    Trigger1       = "EF_g20_loose"
    Trigger2       = "EF_g40_loose"
    Trigger3       = "EF_g60_loose"
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(TriggerFilter)

class LowEtPhotonFilter(JobProperty):
    """  Set the properties of the low et electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    PhotonEt       = 20.0*Units.GeV
    PhotonID       = egammaPID.PhotonIDTight
    Prescale       = 20
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(LowEtPhotonFilter)

class MediumEtPhotonFilter(JobProperty):
    """  Set the properties of the medium et electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    PhotonEt       = 40*Units.GeV
    PhotonID       = egammaPID.PhotonIDTight
    Prescale       = 5
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(MediumEtPhotonFilter)

class HighEtPhotonFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    PhotonEt       = 60*Units.GeV
    PhotonID       = egammaPID.PhotonIDTight
    Prescale       = 1
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(HighEtPhotonFilter)

class ZeeFilter(JobProperty):
    """  Set the properties of the high et electron fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 20.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronID         = egammaPID.ElectronIDMediumPP
    MinMass            = 30.0*Units.GeV
    #MassMax            = 120.0*Units.GeV
    MinPt              = 10.0*Units.GeV
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_PhotonJetStream.add_JobProperty(ZeeFilter)


