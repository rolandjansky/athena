# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDESDMFlags_TileMuStream
##
## Author:      Tomoe Kishimoto
## Description: Here, all neccessary job flags for the primary tile-muon trigger 
##              DESDM are defined.
##=============================================================================

__doc__     = "Here, all neccessary job flags for the Muon commissioning (Alignment adn Trigger) DPD are defined."
__version__ = "0.0.0"
__author__  = "Tomoe Kishimoto <tomoe.kishimoto@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDESDMFlags_TileMuStream(JobPropertyContainer):
    """ The Primary Tile-Muon Trigger DESDM flag/job property  container."""
jobproperties.add_Container(PrimaryDESDMFlags_TileMuStream)

primDESDMTileMu=jobproperties.PrimaryDESDMFlags_TileMuStream



#=====================================================================
# Now define each flag and add it to the container
#=====================================================================

##=============================================================================
## Define the content of the Tight Muon output DPD stream
##=============================================================================
## General switches
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for muon performance DESDM """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDESDMFlags_TileMuStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for muon performance DESDM """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDESDMFlags_TileMuStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for muon performance DESDM """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDESDMFlags_TileMuStream.add_JobProperty(ApplySlimming)


