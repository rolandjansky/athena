# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDESDMFlags_PerfMSStream
##
## Author:      Saminder Dhaliwal
## Description: Here, all neccessary job flags for the Primary tight muon DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Muon commissioning (Alignment adn Trigger) DPD are defined.
"""
__version__ = "0.0.0"
__author__  = "Saminder Dhaliwal <s.dhaliwal@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyUtils.RootUtils as ru
ROOT = ru.import_root()
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
#from ROOT import egammaPID
#from ROOT import egammaParameters



#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDESDMFlags_PerfMSStream(JobPropertyContainer):
    """ The Primary Muon DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDESDMFlags_PerfMSStream)

primDPDAlignTrigMu=jobproperties.PrimaryDESDMFlags_PerfMSStream



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
jobproperties.PrimaryDESDMFlags_PerfMSStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for muon performance DESDM """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDESDMFlags_PerfMSStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for muon performance DESDM """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDESDMFlags_PerfMSStream.add_JobProperty(ApplySlimming)


