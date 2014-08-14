# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_MuonStream
##
## Authors:      David Lopez Mateos (Columbia University/Caltech)
## Created:     August 2008
## and  C.Gatti (LNF-INFN) since 2009 
## Description: Here, all neccessary job flags for the Primary muon DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Primary Muon DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "David Lopez Mateos <David.Lopez@cern.ch>"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units


#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_MuonStream(JobPropertyContainer):
    """ The Primary Muon DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_MuonStream)

#short-cut to get the PrimaryDPDFlags_PhotonJetStream container with this one line:
#'from PrimaryDPDMaker.PrimaryDPDFlags_PhotonJetStream import primPhotonJetDPD'
#Note that name has to be different to avoid problems with pickle
primDPDmu=jobproperties.PrimaryDPDFlags_MuonStream



#=====================================================================
# Now define each flag and add it to the container
#=====================================================================

##=============================================================================
## Define the content of the Muon output DPD stream
##=============================================================================
### General Switches
###
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplyThinning)

class ApplyCaloCellThinning(JobProperty):
    """  Apply the CaloCell thinning for the Muon output DPD stream. Not working yet"""
    statusOn     = False
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplyCaloCellThinning)


class ApplyPixelClusterThinning(JobProperty):
    """  Apply the Pixel Cluster thinning for the Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplyPixelClusterThinning)

class ApplySCTClusterThinning(JobProperty):
    """  Apply the SCT Cluster thinning for the Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplySCTClusterThinning)

class ApplyTRTDriftCircleThinning(JobProperty):
    """  Apply the  TRT Drift Circle thinning for the Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplyTRTDriftCircleThinning)

class ApplySlimming(JobProperty):
    """  Apply the slimming for the Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplySlimming)

##--------------------------------
## Sub-options for the Muon DPD output stream
##--------------------------------
class ApplyPrepRawDataThinning(JobProperty):
    """ Apply the thinning of the InDet PrepRawData (removing object) for the Muon output DPD streams."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(ApplyPrepRawDataThinning)

##--------------------------------
## Other stream content
##--------------------------------

class WriteIdPrepRawData(JobProperty):
    """ Write the ESD PrepRawData."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(WriteIdPrepRawData)

class WriteAllCaloContainer(JobProperty):
    """ Write the AllCalo Container"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(WriteAllCaloContainer)


#############################################

class UseESDContainer(JobProperty):
    """  Use ESD Muon Containers"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(UseESDContainer)



###############################################
## Single Muon switches
###############################################

class SelectSingleMuons(JobProperty):
    """  global switch for single muons"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SelectSingleMuons)


class SelectSingleIsoMuons(JobProperty):
    """  global switch for single isolated muons"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SelectSingleIsoMuons)


class IDActivity_standalone(JobProperty):
    """  require ID activity (>0tracks) for events with standalone muons only"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(IDActivity_standalone)


class minTotSiHits_tag(JobProperty):
    """  minimum number of silicon hits on tag tracks"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(minTotSiHits_tag)

class minTotTrigHits_standalone(JobProperty):
    """  minimum number of trigger hits on standalone tracks"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(minTotTrigHits_standalone)

class minPrecisionHits_standalone(JobProperty):
    """  minimum number of precision hits on standalone tracks"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(minPrecisionHits_standalone)



## Staco
##combined
class Skim_Staco_combined_MinPt(JobProperty):
    """  Skim Staco combined starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_combined_MinPt)

class Prescale_Staco_combined_MaxPt(JobProperty):
    """  Prescale Staco combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_combined_MaxPt)


class Prescale_Staco_combined_prescale(JobProperty):
    """  Prescale factor for Staco combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_combined_prescale)

##standalone
class Skim_Staco_standalone_MinPt(JobProperty):
    """  Skim Staco standalone starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_standalone_MinPt)

class Prescale_Staco_standalone_MaxPt(JobProperty):
    """  Prescale Staco standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_standalone_MaxPt)


class Prescale_Staco_standalone_prescale(JobProperty):
    """  Prescale factor for Staco standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_standalone_prescale)

##lowpt
class Skim_Staco_lowpt_MinPt(JobProperty):
    """  Skim Staco lowpt starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_lowpt_MinPt)

class Prescale_Staco_lowpt_MaxPt(JobProperty):
    """  Prescale Staco lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_lowpt_MaxPt)


class Prescale_Staco_lowpt_prescale(JobProperty):
    """  Prescale factor for Staco lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_lowpt_prescale)






## Muid
##combined
class Skim_Muid_combined_MinPt(JobProperty):
    """  Skim Muid combined starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_combined_MinPt)

class Prescale_Muid_combined_MaxPt(JobProperty):
    """  Prescale Muid combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_combined_MaxPt)


class Prescale_Muid_combined_prescale(JobProperty):
    """  Prescale factor for Muid combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_combined_prescale)

##standalone
class Skim_Muid_standalone_MinPt(JobProperty):
    """  Skim Muid standalone starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_standalone_MinPt)

class Prescale_Muid_standalone_MaxPt(JobProperty):
    """  Prescale Muid standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_standalone_MaxPt)


class Prescale_Muid_standalone_prescale(JobProperty):
    """  Prescale factor for Muid standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_standalone_prescale)

##lowpt
class Skim_Muid_lowpt_MinPt(JobProperty):
    """  Skim Muid lowpt starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_lowpt_MinPt)

class Prescale_Muid_lowpt_MaxPt(JobProperty):
    """  Prescale Muid lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_lowpt_MaxPt)


class Prescale_Muid_lowpt_prescale(JobProperty):
    """  Prescale factor for Muid lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_lowpt_prescale)





##calo
class Skim_Calo_MinPt(JobProperty):
    """  Skim Calo starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Calo_MinPt)

class Prescale_Calo_MaxPt(JobProperty):
    """  Prescale Calo up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Calo_MaxPt)


class Prescale_Calo_prescale(JobProperty):
    """  Prescale factor for Calo up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Calo_prescale)

##################################################
##### Isolated Single Muons ######################
##################################################

class PtTrackIsol(JobProperty):
    """  Isolation cut on Sumpt/pt """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.2
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(PtTrackIsol)



## Staco
##combined
class Skim_Staco_combined_MinPtIso(JobProperty):
    """  Skim Staco combined starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_combined_MinPtIso)

class Prescale_Staco_combined_MaxPtIso(JobProperty):
    """  Prescale Staco combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_combined_MaxPtIso)


class Prescale_Staco_combined_prescaleIso(JobProperty):
    """  Prescale factor for Staco combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_combined_prescaleIso)

##standalone
class Skim_Staco_standalone_MinPtIso(JobProperty):
    """  Skim Staco standalone starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_standalone_MinPtIso)

class Prescale_Staco_standalone_MaxPtIso(JobProperty):
    """  Prescale Staco standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_standalone_MaxPtIso)


class Prescale_Staco_standalone_prescaleIso(JobProperty):
    """  Prescale factor for Staco standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_standalone_prescaleIso)

##lowpt
class Skim_Staco_lowpt_MinPtIso(JobProperty):
    """  Skim Staco lowpt starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_lowpt_MinPtIso)

class Prescale_Staco_lowpt_MaxPtIso(JobProperty):
    """  Prescale Staco lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_lowpt_MaxPtIso)


class Prescale_Staco_lowpt_prescaleIso(JobProperty):
    """  Prescale factor for Staco lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_lowpt_prescaleIso)






## Muid
##combined
class Skim_Muid_combined_MinPtIso(JobProperty):
    """  Skim Muid combined starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_combined_MinPtIso)

class Prescale_Muid_combined_MaxPtIso(JobProperty):
    """  Prescale Muid combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_combined_MaxPtIso)


class Prescale_Muid_combined_prescaleIso(JobProperty):
    """  Prescale factor for Muid combined up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_combined_prescaleIso)

##standalone
class Skim_Muid_standalone_MinPtIso(JobProperty):
    """  Skim Muid standalone starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_standalone_MinPtIso)

class Prescale_Muid_standalone_MaxPtIso(JobProperty):
    """  Prescale Muid standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_standalone_MaxPtIso)


class Prescale_Muid_standalone_prescaleIso(JobProperty):
    """  Prescale factor for Muid standalone up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_standalone_prescaleIso)

##lowpt
class Skim_Muid_lowpt_MinPtIso(JobProperty):
    """  Skim Muid lowpt starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_lowpt_MinPtIso)

class Prescale_Muid_lowpt_MaxPtIso(JobProperty):
    """  Prescale Muid lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_lowpt_MaxPtIso)


class Prescale_Muid_lowpt_prescaleIso(JobProperty):
    """  Prescale factor for Muid lowpt up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_lowpt_prescaleIso)





##calo
class Skim_Calo_MinPtIso(JobProperty):
    """  Skim Calo starting from MinPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Calo_MinPtIso)

class Prescale_Calo_MaxPtIso(JobProperty):
    """  Prescale Calo up to MaxPt"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Calo_MaxPtIso)


class Prescale_Calo_prescaleIso(JobProperty):
    """  Prescale factor for Calo up to MaxPt"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Calo_prescaleIso)




########################################
## special methods for LowBetaCandidate

class SkimLowBeta(JobProperty):
    """  Skim LowBeta candididates"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SkimLowBeta)

class LowBetaPrescale(JobProperty):
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(LowBetaPrescale)





###########################################################
## DiMuon switches
##########################################################

class SelectDiMuons(JobProperty):
    """  global switch for di muons"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SelectDiMuons)

class UseOnlyCBTag(JobProperty):
    """  Skim DiMuon using only CB muons for the tag"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(UseOnlyCBTag)


class Skim_DiMuon_DeltaPhi(JobProperty):
    """  Skim DiMuon of DIFFERENT TYPE with minimum angular distance DeltaPhi (rad) """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_DiMuon_DeltaPhi)

class Skim_DiMuon_OppositeSign(JobProperty):
    """  Skim Opposite sign DiMuon """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_DiMuon_OppositeSign)


class Skim_DiMuon_MinPt(JobProperty):
    """  Skim DiMuon events with minimum muon pT"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_DiMuon_MinPt)


# isolation
class DiMuon_DoTagPtTrackIsol(JobProperty):
    """  do pT Isolation cut on DiMuon Tag"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(DiMuon_DoTagPtTrackIsol)


class DiMuon_DoProbePtTrackIsol(JobProperty):
    """  do pT Isolation cut on DiMuon Probe"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(DiMuon_DoProbePtTrackIsol)

class DiMuon_TagPtTrackIsol(JobProperty):
    """  Sum pt/pt isolation cut (0.4 cone by default) for Tag"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.2
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(DiMuon_TagPtTrackIsol)

class DiMuon_ProbePtTrackIsol(JobProperty):
    """  Sum pt/pt isolation cut (0.4 cone by default) for Probe"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.2
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(DiMuon_ProbePtTrackIsol)


## Staco
##combined
class Skim_Staco_combined_MinMass(JobProperty):
    """  Skim Staco combined starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_combined_MinMass)

class Prescale_Staco_combined_MaxMass(JobProperty):
    """  Prescale Staco combined up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_combined_MaxMass)


class Prescale_Staco_combined_halfprescale(JobProperty):
    """  Prescale factor for Staco combined up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_combined_halfprescale)

##standalone
class Skim_Staco_standalone_MinMass(JobProperty):
    """  Skim Staco standalone starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_standalone_MinMass)

class Prescale_Staco_standalone_MaxMass(JobProperty):
    """  Prescale Staco standalone up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_standalone_MaxMass)


class Prescale_Staco_standalone_halfprescale(JobProperty):
    """  Prescale factor for Staco standalone up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_standalone_halfprescale)

##lowpt
class Skim_Staco_lowpt_MinMass(JobProperty):
    """  Skim Staco lowpt starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Staco_lowpt_MinMass)

class Prescale_Staco_lowpt_MaxMass(JobProperty):
    """  Prescale Staco lowpt up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_lowpt_MaxMass)


class Prescale_Staco_lowpt_halfprescale(JobProperty):
    """  Prescale factor for Staco lowpt up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Staco_lowpt_halfprescale)





## Muid
##combined
class Skim_Muid_combined_MinMass(JobProperty):
    """  Skim Muid combined starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_combined_MinMass)

class Prescale_Muid_combined_MaxMass(JobProperty):
    """  Prescale Muid combined up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_combined_MaxMass)


class Prescale_Muid_combined_halfprescale(JobProperty):
    """  Prescale factor for Muid combined up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_combined_halfprescale)

##standalone
class Skim_Muid_standalone_MinMass(JobProperty):
    """  Skim Muid standalone starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_standalone_MinMass)

class Prescale_Muid_standalone_MaxMass(JobProperty):
    """  Prescale Muid standalone up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_standalone_MaxMass)


class Prescale_Muid_standalone_halfprescale(JobProperty):
    """  Prescale factor for Muid standalone up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_standalone_halfprescale)

##lowpt
class Skim_Muid_lowpt_MinMass(JobProperty):
    """  Skim Muid lowpt starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Muid_lowpt_MinMass)

class Prescale_Muid_lowpt_MaxMass(JobProperty):
    """  Prescale Muid lowpt up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_lowpt_MaxMass)


class Prescale_Muid_lowpt_halfprescale(JobProperty):
    """  Prescale factor for Muid lowpt up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Muid_lowpt_halfprescale)





##calo
class Skim_Calo_MinMass(JobProperty):
    """  Skim Calo starting from MinMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Skim_Calo_MinMass)

class Prescale_Calo_MaxMass(JobProperty):
    """  Prescale Calo up to MaxMass"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*Units.GeV
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Calo_MaxMass)


class Prescale_Calo_halfprescale(JobProperty):
    """  Prescale factor for Calo up to MaxMass"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(Prescale_Calo_halfprescale)


########BitWise Configuration
### if first bit from the right ==1, the bit configuration is turned on and previous settings will be overwritten
####################################################################

## bits for single muons

class CBWord(JobProperty):
    """  word for configuring the combined muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( ptmin bin )( ptmax bin )( prescale )] [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(CBWord)

class SAWord(JobProperty):
    """  word for configuring the standalone muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( ptmin bin )( ptmax bin )( prescale )] [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SAWord)

class TAGWord(JobProperty):
    """  word for configuring the tag muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( ptmin bin )( ptmax bin )( prescale )] [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(TAGWord)

class CALOWord(JobProperty):
    """  word for configuring the combined muon selection """
    """          4bits          4bits        4bits        4 bits     """
    """  0x  [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """               Calo word                                 """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(CALOWord)

## Bits for Isolated muons

class CBIsoWord(JobProperty):
    """  word for configuring the combined muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( ptmin bin )( ptmax bin )( prescale )] [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(CBIsoWord)

class SAIsoWord(JobProperty):
    """  word for configuring the standalone muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( ptmin bin )( ptmax bin )( prescale )] [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SAIsoWord)

class TAGIsoWord(JobProperty):
    """  word for configuring the tag muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( ptmin bin )( ptmax bin )( prescale )] [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(TAGIsoWord)

class CALOIsoWord(JobProperty):
    """  word for configuring the combined muon selection """
    """          4bits          4bits        4bits        4 bits     """
    """  0x  [( ptmin bin )( ptmax bin )( prescale )] [(1/0 ON/OFF)] """
    """               Calo word                                 """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(CALOIsoWord)


## Bits for Dimuons

class CBDiMuWord(JobProperty):
    """  word for configuring the combined muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( Mmin bin )( Mmax bin )( prescale )] [( Mmin bin )( Mmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(CBDiMuWord)

class SADiMuWord(JobProperty):
    """  word for configuring the standalone muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( Mmin bin )( Mmax bin )( prescale )] [( Mmin bin )( Mmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(SADiMuWord)

class TAGDiMuWord(JobProperty):
    """  word for configuring the tag muon selection """
    """        4bits          4bits        4bits         4bits          4bits        4bits        4 bits     """
    """  0x [( Mmin bin )( Mmax bin )( prescale )] [( Mmin bin )( Mmax bin )( prescale )] [(1/0 ON/OFF)] """
    """                    Muid word                             Staco Word                                  """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(TAGDiMuWord)

class CALODiMuWord(JobProperty):
    """  word for configuring the combined muon selection """
    """            4bits          4bits        4bits        4 bits     """
    """  0x  [( Mmin bin )( Mmax bin )( prescale )] [(1/0 ON/OFF)] """
    """               Calo word                                 """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0x0
    pass
jobproperties.PrimaryDPDFlags_MuonStream.add_JobProperty(CALODiMuWord)
