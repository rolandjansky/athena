##-----------------------------------------------------------------------------
## Name:  DESDEventSkimmingFilters.py
##
## Author:   Karsten Koeneke <karsten.koeneke_at_cern.ch>
## Created:  January 2010
##
## Description:  Define the event skimming filters for all DESDs.
##
## $Id$
##-----------------------------------------------------------------------------
"""
# If this file is already included, don't include it again
include.block("PrimaryDPDMaker/DESDEventSkimmingFilters.py")


## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

## Include the job property flags for this package and from RecExCommon
from RecExConfig.RecFlags import rec

#define 'topSequence'
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Import needed utils
from PrimaryDPDMaker.LogicalFilterCombiner import LogicalFilterCombiner
from PrimaryDPDMaker.PrimaryDPDMakerConf   import PrimaryDPDPrescaler
from AssociationComps.AssociationCompsConf import DeltaRAssociationTool
from D2PDMaker.D2PDMakerConf import D2PDParticleCombiner
from D2PDMaker.D2PDMakerConf import D2PDElectronSelector
from D2PDMaker.D2PDMakerConf import D2PDPhotonSelector
from D2PDMaker.D2PDMakerConf import D2PDMuonSelector
from D2PDMaker.D2PDMakerConf import D2PDTauSelector

# ---- Load the egammaPID and egammaParameters information
# This is needed to always be up-to-date with the egamma
# IsEM selections and also the author selections
import PyCintex
PyCintex.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters

# Import also the muon parameters
#import PyCintex
#PyCintex.loadDictionary('muonEventDict')
#import ROOT
#from ROOT import MuonParameters


# Import all needed flags from the PrimaryDPDMaker
from PrimaryDPDMaker.PrimaryDPDFlags                      import primDPD
from PrimaryDPDMaker.PrimaryDPDFlags_EGammaStream         import primEGammaDPD
from PrimaryDPDMaker.PrimaryDPDFlags_JetStream            import primJetDPD
from PrimaryDPDMaker.PrimaryDPDFlags_LargeMetStream       import primLargeMetDPD
from PrimaryDPDMaker.PrimaryDPDFlags_MinBiasStream        import primMinBiasDPD
from PrimaryDPDMaker.PrimaryDPDFlags_MuonStream           import primDPDmu
from PrimaryDPDMaker.PrimaryDPDFlags_PhotonJetStream      import primPhotonJetDPD
from PrimaryDPDMaker.PrimaryDPDFlags_SingleElectronStream import primSingleElectronDPD
from PrimaryDPDMaker.PrimaryDPDFlags_SingleMuonStream     import primDPDsingleMu
from PrimaryDPDMaker.PrimaryDPDFlags_Tracking             import primTrackDPD

## Create the list that will hold all filter names
from PrimaryDPDMaker.PrimaryDPDFlags import desdEventSkimmingFilterNamesList


##====================================================================
## Setup TrigDecisionTool 
##====================================================================
include("PrimaryDPDMaker/SetupTrigDecisionTool.py")



"""
