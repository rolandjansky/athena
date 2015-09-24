# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_SCTCommStream
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## Author: Christian Schmitt
## Email:  Christian.Schmitt@cern.ch
##
## Description: Here, all necessary job flags for the SCT commissioning DPD
##              are defined.
##
## $Id: PrimaryDPDFlags_SCTCommStream.py,v 1.1 2008-11-13 15:39:31 kkoeneke Exp $
##=============================================================================

__doc__ = """Here, all necessary job flags for the SCT commissioning DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "<Karsten.Koeneke@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_SCTCommStream(JobPropertyContainer):
    """ The Primary SCTComm DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_SCTCommStream)

#short-cut to get the PrimaryDPDFlags_SCTCommStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_SCTCommStream import primSCTCommDPD'
#Note that name has to be different to avoid problems with pickle
primSCTCommDPD=jobproperties.PrimaryDPDFlags_SCTCommStream






#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the SCTComm output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the SCTComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_SCTCommStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the SCTComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_SCTCommStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the SCTComm output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_SCTCommStream.add_JobProperty(ApplySlimming)

##--------------------------------
## Sub-options for the SCTComm DPD output stream
##--------------------------------


##--------------------------------
## Other stream content
##--------------------------------





