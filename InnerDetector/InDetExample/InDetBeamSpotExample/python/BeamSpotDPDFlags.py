# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        BeamSpotDPDFlags
##
## Author:      James Walder : based on PrimaryDPDFlags by Karsten Koeneke
## Created:     March 2010
##
## Description: Here, all neccessary job flags for the BeamSpot DPD
##              are defined.
##
##=============================================================================

__doc__ = """Here, all neccessary job flags for the BeamSpotDPD are defined."""
__version__ = "0.0.1"
__author__  = "James Walder <jwalder@cern.ch>"
__version__ = "$Id $"

from AthenaCommon.JobProperties import JobProperty
from AthenaCommon.JobProperties import jobproperties

# Filename stream
class WriteDESDM_BEAMSPOTStream(JobProperty):
    """ Produce the (DPD) DESDM for the BeamSpot """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    StreamName   = "StreamDESDM_BEAMSPOT"
    FileName     = ""
    pass
jobproperties.PrimaryDPDFlags.add_JobProperty(WriteDESDM_BEAMSPOTStream)
