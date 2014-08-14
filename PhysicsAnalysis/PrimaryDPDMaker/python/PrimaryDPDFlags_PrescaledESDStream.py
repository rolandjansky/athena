# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_PrescaledESDStream
##
## Author:      Eric Feng (Chicago)
## Created:     August 2008
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
##
## Description: Here, all necessary job flags for the Primary PrescaledESD DPD
##              are defined.
##
## $Id: PrimaryDPDFlags_PrescaledESDStream.py,v 1.2 2009-02-11 11:01:36 cote Exp $
##=============================================================================

__doc__ = """Here, all necessary job flags for the Primary PrescaledESD DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "<Eric.Feng@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_PrescaledESDStream(JobPropertyContainer):
    """ The Primary PrescaledESD DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_PrescaledESDStream)

#short-cut to get the PrimaryDPDFlags_PrescaledESDStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_PrescaledESDStream import primPrescaledESDDPD'
#Note that name has to be different to avoid problems with pickle
primPrescaledESDDPD=jobproperties.PrimaryDPDFlags_PrescaledESDStream

class PrescaleFactor(JobProperty):
    """  Apply the skimming (event selection) for the PrescaledESD output DPD stream."""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 2
    pass
jobproperties.PrimaryDPDFlags_PrescaledESDStream.add_JobProperty(PrescaleFactor)






