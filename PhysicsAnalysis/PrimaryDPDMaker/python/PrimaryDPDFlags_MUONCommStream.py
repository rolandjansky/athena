# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_MUONCommStream
##
## Author: Claudio Gatti (LNF)
## Email:  claudio.gatti@lnf.infn.it
##
## Description: Here, all necessary job flags for the MUON commissioning DPD
##              are defined.
##
## $Id: PrimaryDPDFlags_MUONCommStream.py,v 1.1 Fri Nov  6 18:01:00 CET 2009 cgatti $
##=============================================================================

__doc__ = """Here, all necessary job flags for the MUON commissioning DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "<claudio.gatti@lnf.infn.it>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_MUONCommStream(JobPropertyContainer):
    """ The Primary MUONComm DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_MUONCommStream)

#short-cut to get the PrimaryDPDFlags_MUONCommStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_MUONCommStream import primMUONCommDPD'
#Note that name has to be different to avoid problems with pickle
primMUONCommDPD=jobproperties.PrimaryDPDFlags_MUONCommStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the MUONComm output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the MUONComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the MUONComm output DPD stream."""
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the MUONComm output DPD stream."""
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(ApplySlimming)

##--------------------------------
## Sub-options for the MUONComm DPD output stream
##--------------------------------
class DoMuonBoy(JobProperty):
    """  Use MuonBoy tracks in selection for the MUONComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(DoMuonBoy)

class DoMoore(JobProperty):
    """  Use Moore tracks in selection for the MUONComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(DoMoore)

class DoMuTagIMO(JobProperty):
    """  Use MuTagIMO tracks in selection for the MUONComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(DoMuTagIMO)

class DoTrackID(JobProperty):
    """  Use ID tracks in selection for the MUONComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MUONCommStream.add_JobProperty(DoTrackID)





##--------------------------------
## Other stream content
##--------------------------------





