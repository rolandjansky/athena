# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_JetStream
##
## Author:      Ulla Blumenschein (Goettingen)
## Created:     August 2008
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
## 
## Description: Here, all necessary job flags for the Primary LargeMet DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all necessary job flags for the Primary Jet DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "Ulla Blumenschein <Ulla.Blumenschein@cern.ch>"



from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_LargeMetStream(JobPropertyContainer):
    """ The Primary LargeMet DPD flag/job property  container."""
    TauTriggerNames={"EF_tau29_loose":1,"EF_tau38_loose":1,"EF_tau12_loose_xe15_noMu":1,"EF_tauNoCut_hasTrk6_xe20_noMu":1,"EF_tauNoCut_hasTrk6_EFxe15":1,"EF_tauNoCut_hasTrk6_EFxe15_noMu":1,"EF_tau12_loose_EFxe12":1,"EF_tau12_loose_EFxe12_noMu":1}
    
    GlobalTriggerPrescale = 20
    
jobproperties.add_Container(PrimaryDPDFlags_LargeMetStream)
    
primLargeMetDPD=jobproperties.PrimaryDPDFlags_LargeMetStream



#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the LargeMet output DPD stream
##=============================================================================

## ===================
## General switches
## ===================

class WriteFullESD(JobProperty):
    """  Write everything """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WriteFullESD)

class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the LargeMet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the LargeMet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the slimming for the LargeMet output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplySlimming)


## ========================
##  MET Significance Filter
## =========================


class HighMetSig(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 5
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(HighMetSig)

class HighMetSigFilterPrescale(JobProperty):
    """  Apply the LargeMet Filter"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(HighMetSigFilterPrescale)


class LowMetSig(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 5
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(LowMetSig)

class LowMetSigFilterPrescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 100
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(LowMetSigFilterPrescale)


# W Filter


class WMetEt(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 25
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WMetEt)

class WMetFilterPrescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WMetFilterPrescale)




class WTauFilterPresel(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WTauFilterPresel)

class WTauFilterEt(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 25
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WTauFilterEt)

class WTauFilterPrescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WTauFilterPrescale)

class WTauFilterOptID(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(WTauFilterOptID)




# Trigger Skimming


class ApplyTriggerSkimming(JobProperty):
    """ Apply the thinning of the Tracks for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplyTriggerSkimming)



## ===================
##  Thinning options
## ===================


class ApplyPixelClusterThinning(JobProperty):
    """ Apply the thinning of the pixel clusters (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplyPixelClusterThinning)

class ApplySCTClusterThinning(JobProperty):
    """ Apply the thinning of the SCT clusters (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplySCTClusterThinning)

class ApplyTRTDriftCircleThinning(JobProperty):
    """ Apply the thinning of the TRT_DriftCircles (removing object) for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplyTRTDriftCircleThinning)

class ApplyTrackThinning(JobProperty):
    """ Apply the thinning of the Tracks for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplyTrackThinning)

class ApplyTriggerTowerThinning(JobProperty):
    """ Apply the thinning of the TriggerTower for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_LargeMetStream.add_JobProperty(ApplyTriggerTowerThinning)
