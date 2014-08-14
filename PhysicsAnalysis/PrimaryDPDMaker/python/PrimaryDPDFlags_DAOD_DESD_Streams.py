# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_DAOD_DESD_Streams
##
## Author:      Eric Feng (Chicago)
## Created:     September 2009
##
## Description: Here, all necessary job flags for the skimmed AOD and ESD streams
##              are defined.
##
## $Id$
##=============================================================================

__doc__ = """Here, all necessary job flags for the skimmed AOD and ESD streams
are defined.  """

__version__ = "0.0.2"
__author__  = "Eric Feng <Eric.Feng@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_DAOD_DESD_Streams(JobPropertyContainer):
    """ The DAOD_DESD_Streams flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_DAOD_DESD_Streams)

#short-cut to get the PrimaryDPDFlags container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_DAOD_DESD_Streams import primDAOD_DESD_Streams'
#Note that name has to be different to avoid problems with pickle
primDAOD_DESD_Streams = jobproperties.PrimaryDPDFlags_DAOD_DESD_Streams


#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the skimmed AOD streams output
##=============================================================================
class ApplySkimming(JobProperty):
    """ Apply the skimming (event selection) for the skimmed AOD output streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_DAOD_DESD_Streams.add_JobProperty(ApplySkimming)
