# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_MinBiasStream
##
## Author:      Eric Feng (Chicago), Michiru Kaneda (Tokyo)
## Created:     August 2008
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
##
## Description: Here, all necessary job flags for the Primary Minimum Bias DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all necessary job flags for the Primary Minimum Bias DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "<Eric.Feng@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_MinBiasStream(JobPropertyContainer):
    """ The Primary Minimum Bias DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_MinBiasStream)

#short-cut to get the PrimaryDPDFlags_MinBiasStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_MinBiasStream import primMinBiasDPD'
#Note that name has to be different to avoid problems with pickle
primMinBiasDPD=jobproperties.PrimaryDPDFlags_MinBiasStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the Minimum Bias output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the Minimum Bias output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Minimum Bias output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the Minimum Bias output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(ApplySlimming)

##--------------------------------
## Sub-options for the Minimum Bias DPD output stream
##--------------------------------
class ApplyCaloCellThinning(JobProperty):
    """ Apply the thinning of the calorimeter cells (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(ApplyCaloCellThinning)

class ApplyTrackParticleThinning(JobProperty):
    """ Apply the thinning of the track particles (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(ApplyTrackParticleThinning)

class ApplyPixelClusterThinning(JobProperty):
    """ Apply the thinning of the pixel clusters (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(ApplyPixelClusterThinning)

##--------------------------------
## Other stream content
##--------------------------------
class WriteCaloCells(JobProperty):
    """ Write out the ESD calorimeter cells."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(WriteCaloCells)

class WriteIdPrepRawData(JobProperty):
    """ Write the ESD PrepRawData."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(WriteIdPrepRawData)

##-------------------------------
## Further Filter Configuration
##-------------------------------
class TrackFilter(JobProperty):
    """  Set the properties of the track filter."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
## SingleTracks
    UseEPFilter    = True
    TrackPtCut     = 2.0*Units.GeV
#    TrackPtCut     = 1.0*Units.GeV
#    TrackPtCut     = 0.5*Units.GeV
    TrackAtCaloEta          = 2.4
    TrackD0                 = 1.5
    TrackZ0                 = 1.5
    TrackNumberOfSCTHits    = 6
    TrackNumberOfPixelHits  = 1
    NumberOfPrimaryVertexes = 2
    trackIsoDR              = 0.4
## SingleTracks end
    TrackEtaCut    = 2.5
    SumPtRatioCut  = 0.1
    Chi2DOFCut     = 2.5
    NBLayerHitCut  = 0
    NHoleCut       = 0
    NSiHitCut      = 5
    TrackIsolCut   = True
    NTrackIsolCut  = 1
    CaloIsolCut    = False
    Prescale       = 1
    Enable         = True
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(TrackFilter)

class LowPtTrackFilter(JobProperty):
    """  Set the properties of the track filter."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
## SingleTracks
    UseEPFilter    = True
    TrackPtCut     = 0.5*Units.GeV
    TrackAtCaloEta          = 2.4
    TrackD0                 = 1.5
    TrackZ0                 = 1.5
    TrackNumberOfSCTHits    = 6
    TrackNumberOfPixelHits  = 1
    NumberOfPrimaryVertexes = 2
    trackIsoDR              = 0.4
## SingleTracks end
    TrackEtaCut    = 2.5
    SumPtRatioCut  = 0.1
    Chi2DOFCut     = 2.5
    NBLayerHitCut  = 0
    NHoleCut       = 0
    NSiHitCut      = 5
    TrackIsolCut   = True
    NTrackIsolCut  = 1
    CaloIsolCut    = False
    Prescale       = 20
    Enable         = True
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(LowPtTrackFilter)

class SimplePrescaleFilter(JobProperty):
    """  Set the properties of the simple prescale filter."""
    statusOn       = True
    allowedTypes   = ['bool']
    StoredValue    = True
    Prescale       = 100000000
    Offset         = -1
    pass
jobproperties.PrimaryDPDFlags_MinBiasStream.add_JobProperty(SimplePrescaleFilter)

