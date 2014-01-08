# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_RPVLL_CosmicCalo
## Authors:     Paul Jackson (SLAC) Nick Barlow (Cambridge), C. Ohm (Stockholm)
## Created:     Februaury 2010
##
## Based on:    PrimaryDPDFlags_PhotonJetStream by Karsten Koeneke (DESY)
##
## Description: Here, all necessary job flags for the RPV/LL ESD-based DPD
##              are defined.
##
##=============================================================================

__doc__ = """Here, all necessary job flags for the RPV/LL SUSY jet DPD are defined."""

__version__ = "0.0.1"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

#=====================================================================
# First define container for the flags  
#=====================================================================
class PrimaryDPDFlags_RPVLL_CosmicCaloStream(JobPropertyContainer):
    """ The Primary RPV/LL CosmicCalo DESDM flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_RPVLL_CosmicCaloStream)

primRPVLLCosmicCaloDESDM=jobproperties.PrimaryDPDFlags_RPVLL_CosmicCaloStream

#===============================================================
# Set specific properties
#==============================================================

class triggerFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False
    TriggerNames = ["L1_J10_EMPTY","L1_TAU5_EMPTY"]
    ##TriggerNames = ["L1_J10","L1_TAU5"]    
    pass
primRPVLLCosmicCaloDESDM.add_JobProperty(triggerFilterFlags)

class jetFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin = 20*Units.GeV
    cutEtMax = 1000*Units.GeV
    cutEtaMax=2.0
    nJetCut  = 1
    jetCollectionName = "AntiKt4TowerJets"
    goodJetCollectionName="RPVLL_CosmicCaloStream_AntiKt4TowerJets10Empty"
    pass
primRPVLLCosmicCaloDESDM.add_JobProperty(jetFilterFlags)

class muonSegmentFilterFlags(JobProperty):
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    cutNsegMin = 0
    cutNsegMax = 0
    addPrescaledLooseFilter = False
    cutNsegMinLoose = 0
    cutNsegMaxLoose = 20
    prescaleFactorForLooseFilter = 10
    muonSegmentCollectionName = "MooreSegments"
    pass
primRPVLLCosmicCaloDESDM.add_JobProperty(muonSegmentFilterFlags)
