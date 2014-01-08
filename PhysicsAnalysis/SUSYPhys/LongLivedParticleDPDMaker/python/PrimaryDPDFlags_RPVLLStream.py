# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_RPVLL_Stream
## Authors:     Nick Barlow (Cambridge), C. Ohm (Stockholm)
## Created:     November 2009
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
##
## Description: Here, all necessary job flags for the RPV/LL ESD-based DPD
##              are defined.
##
##=============================================================================

__doc__ = """Here, all necessary job flags for the RPV/LL SUSY  DESD are defined."""

__version__ = "0.0.1"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_RPVLLStream(JobPropertyContainer):
    """ The Primary RPV/LL DPD flag/job property  container."""

jobproperties.add_Container(PrimaryDPDFlags_RPVLLStream)

primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

import SmpCaloIdFlags
import DVFlags
import VH_DVFlags
import SmpMsFlags
import HipsFlags
import NonPointingFlags
import KinkedTrackFlags
import StoppedFlags
import HVFlags
import QuirksFlags
import DiLepFlags
import StealthFlags

#===============================================================
# Set specific properties for each of the analysis selections
#===============================================================

class doStealth(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True 
    ForceToRun = False 
    TriggerStreams = ["JetTauEtmiss"]    
    pass
primRPVLLDESDM.add_JobProperty(doStealth)

class doDiLep(JobProperty):
    statusOn=True
    allowedTypes = ["bool"]
    StoredValue=True
    ForceToRun=False
    TriggerStreams=["Egamma"]
    pass
primRPVLLDESDM.add_JobProperty(doDiLep)
    
class doQuirks(JobProperty):
    statusOn=True
    allowedTypes = ["bool"]
    StoredValue=True
    ForceToRun=False
    TriggerStreams=["JetTauEtmiss"]
    pass
primRPVLLDESDM.add_JobProperty(doQuirks)

class doSmpCaloId(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    ForceToRun = False
    TriggerStreams = ["JetTauEtmiss", "Muons"]
    pass
primRPVLLDESDM.add_JobProperty(doSmpCaloId)

class doDV(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    ForceToRunMuonsSel=False
    ForceToRunEgammaSel=False
    ForceToRunJetTauEtmissSel=False    
    TriggerStreams = ["Egamma","JetTauEtmiss"] #### NB for late 2013 reprocessing don't rerun on Muons
    pass
primRPVLLDESDM.add_JobProperty(doDV)

class doVH_DV(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    ForceToRun = False
    TriggerStreams = ["Muons", "Egamma"]
    pass
primRPVLLDESDM.add_JobProperty(doVH_DV)

class doSmpMs(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False #### NB for late 2013 reprocessing don't rerun this
    ForceToRun = False
    TriggerStreams = ["Muons"]
    pass
primRPVLLDESDM.add_JobProperty(doSmpMs)

class doNonPointing(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False #### NB for late 2013 reprocessing don't rerun this
    ForceToRun = False
    TriggerStreams = ["Egamma"]
    pass
primRPVLLDESDM.add_JobProperty(doNonPointing)

class doKinkedTrack(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False #### NB for late 2013 reprocessing don't rerun this
    ForceToRun = False
    TriggerStreams = ["JetTauEtmiss"]
    pass
primRPVLLDESDM.add_JobProperty(doKinkedTrack)

class doHips(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False #### NB for late 2013 reprocessing don't rerun this
    ForceToRun = False
    TriggerStreams = ["Egamma"]
    pass
primRPVLLDESDM.add_JobProperty(doHips)

class doStopped(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False #### NB for late 2013 reprocessing don't rerun this
    ForceToRun = False
    TriggerStreams = ["JetTauEtmiss"]
    pass
primRPVLLDESDM.add_JobProperty(doStopped)

class doHV(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False #### NB for late 2013 reprocessing don't rerun this 
    ForceToRun = False
#    TriggerStreams = ["JetTauEtmiss", "Muons", "Egamma"]
#    TriggerStreams = ["JetTauEtmiss", "Muons"]    
    TriggerStreams = ["JetTauEtmiss"]    
    pass
primRPVLLDESDM.add_JobProperty(doHV)

# other switches (old - remove some?)

class UseAODTruth(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(UseAODTruth)

class prescaleFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue=False
    applyPrescale = False
    prescaleFactor=1.0
primRPVLLDESDM.add_JobProperty(prescaleFlags)

class doCaloCellThinning(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doCaloCellThinning)

