# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_Tracking
##
## Author:      Lorenzo Feligioni -->  Yoshikazu Nagai
## Created:     September 2008    -->  April 2012
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
## 
## Description: Here, all necessary job flags for the Primary Tracking DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all necessary job flags for the Primary Tracking DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "Lorenzo Feligioni <Lorenzo.Feligioni@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_Tracking(JobPropertyContainer):
    """ The Primary Tracking DPD flag/job property  container."""
#    L1SingleJetTriggerNames = {"L1_J5":1,"L1_J10":1500,"L1_J15":1,"L1_J20":1,"L1_J30":1,"L1_J40":1,"L1_J55":100,"L1_J70":1,"L1_J75":100,"L1_J95":20,"L1_J100":1,"L1_J115":20,"L1_J130":1,"L1_J150":1}
    L1SingleJetTriggerNames = {"L1_J10":1000,"L1_J15":1,"L1_J20":1,"L1_J30":1,"L1_J50":1,"L1_J75":100}
#    L1MuJetTriggerNames = {"L1_MU0_J10":-1,"L1_MU0_J40":-1,"L1_MU0_J70":-1}
    L1MuJetTriggerNames = {"L1_MU4_J10":-1,"L1_MU4_J15":-1,"L1_MU4_J30":-1,"L1_MU4_J50":-1,"L1_MU4_J75":-1}
#    L2MuJetTriggerNames = {"L2_mu4_L1J10_matched":-1,"L2_mu4_L1J15_matched":-1,"L2_mu4_L1J30_matched":20,"L2_mu4_L1J55_matched":-1}
    L2MuJetTriggerNames = {"L2_mu4T_j10_a4TTem":-1,"L2_mu4T_j15_a4TTem":-1,"L2_mu4T_j40_c4cchad":1,"L2_mu4T_j60_c4cchad":-1}
#    EFMuJetTriggerNames = {"EF_mu4_L1J10":-1,"EF_mu4_L1J20":-1,"EF_mu4_L1J10_matched":-1,"EF_mu4_L1J20_matched":-1,"EF_mu4_L1J40_matched":-1,"EF_mu4_L1J70_matched":-1,"EF_mu4_L1J5_matched":-1,"EF_mu4_L1J10_matched":-1,"EF_mu4_L1J15_matched":-1,"EF_mu4_L1J30_matched":-1,"EF_mu4_L1J55_matched":-1}
    EFMuJetTriggerNames = {"EF_mu4T_j15_a4tchad_matchedZ":-1,"EF_mu4T_j25_a4tchad_matchedZ":-1,"EF_mu4T_j35_a4tchad_matchedZ":-1,"EF_mu4T_j45_a4tchad_matchedZ":-1,"EF_mu4T_j55_a4tchad_matchedZ":-1,"EF_mu4T_j15_a4tchad_matched":-1,"EF_mu4T_j25_a4tchad_matched":-1,"EF_mu4T_j35_a4tchad_matched":-1,"EF_mu4T_j45_a4tchad_matched":-1,"EF_mu4T_j55_a4tchad_matched":-1,"EF_mu4T_j65_a4tchad_matched":-1,"EF_mu4T_j80_a4tchad_matched":-1,"EF_mu4T_j110_a4tchad_matched":-1,"EF_mu4T_j145_a4tchad_matched":-1,"EF_mu4T_j180_a4tchad_matched":-1,"EF_mu4T_j220_a4tchad_matched":-1,"EF_mu4T_j280_a4tchad_matched":-1}

#    BJetSliceTriggerNames = {"EF_b10_IDTrkNoCut":1} 
    BJetSliceTriggerNames = {"EF_b15_NoCut_j15_a4tchad":1} 
    GlobalTriggerPrescale = 1
jobproperties.add_Container(PrimaryDPDFlags_Tracking)

#short-cut to get the PrimaryDPDFlags_Tracking container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_Tracking import primTrackingDPD'
#Note that name has to be different to avoid problems with pickle
primTrackDPD=jobproperties.PrimaryDPDFlags_Tracking


#=====================================================================
# Now define each flag and add it to the container
#=====================================================================


##=============================================================================
## Define the content of the Tracking output DPD stream
##=============================================================================
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the Tracking output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Tracking output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the Tracking output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplySlimming)

##--------------------------------
## Sub-options for the Tracking DPD output stream
##--------------------------------

class ApplyJetSkimming(JobProperty):
    """ Apply the skimming based on jet pt for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyJetSkimming)

class ApplyMuJetSkimming(JobProperty):
    """ Apply the skimming based on the presence of a mu jet for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyMuJetSkimming)

class ApplyTriggerSkimming(JobProperty):
    """ Apply the skimming based on Trigger Decision Tool for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyTriggerSkimming)

class ApplyTriggerPrescales(JobProperty):
    """ Apply the prescales based on Trigger Decision Tool for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyTriggerPrescales)

class ApplyL1SingleJetTriggerSkimming(JobProperty):
    """ Apply the skimming based on L1 single jet Trigger terms for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyL1SingleJetTriggerSkimming)

class ApplyL1MuJetTriggerSkimming(JobProperty):
    """ Apply the skimming based on L1 mu jet Trigger terms for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyL1MuJetTriggerSkimming)

class ApplyL2MuJetTriggerSkimming(JobProperty):
    """ Apply the skimming based on L2 mu jet Trigger terms for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyL2MuJetTriggerSkimming)

class ApplyEFMuJetTriggerSkimming(JobProperty):
    """ Apply the skimming based on EF mu jet Trigger terms for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyEFMuJetTriggerSkimming)

class ApplyBJetSliceTriggerSkimming(JobProperty):
    """ Apply the skimming based on b10_IDTrkNoCut Trigger term for the output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(ApplyBJetSliceTriggerSkimming)

##--------------------------------
## Other stream content
##--------------------------------
class WriteIdPrepRawData(JobProperty):
    """ Write the ESD PrepRawData."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(WriteIdPrepRawData)


##--------------------------------
## Prescales
##--------------------------------
class JetEt0to30Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 200
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt0to30Prescale)

class JetEt30to60Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 200
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt30to60Prescale)

class JetEt60to100Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 250
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt60to100Prescale)

class JetEt100to150Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 100
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt100to150Prescale)

class JetEt150to225Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 25
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt150to225Prescale)

class JetEt225to375Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 10
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt225to375Prescale)

class JetEt375to1E9Prescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 10
    pass
jobproperties.PrimaryDPDFlags_Tracking.add_JobProperty(JetEt375to1E9Prescale)
