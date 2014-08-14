# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_TtbarMuonStream
##
## Authors:      David Lopez Mateos (Columbia University/Caltech)
## Created:     August 2008
## and  C.Gatti (LNF-INFN) since 2009 
## Description: Here, all neccessary job flags for the Primary tight muon DPD
##              are defined.
##=============================================================================

__doc__ = """Here, all neccessary job flags for the Primary Muon DPD are defined.
"""

__version__ = "0.0.1"
__author__  = "David Lopez Mateos <David.Lopez@cern.ch>"

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
from ROOT import egammaPID
from ROOT import egammaParameters



#=====================================================================
# First define container for the PrimaryDPDMaker flags  
#=====================================================================
class PrimaryDPDFlags_TtbarMuonStream(JobPropertyContainer):
    """ The Primary Muon DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_TtbarMuonStream)

primDPDttbarMu=jobproperties.PrimaryDPDFlags_TtbarMuonStream



#=====================================================================
# Now define each flag and add it to the container
#=====================================================================

##=============================================================================
## Define the content of the Tight Muon output DPD stream
##=============================================================================
## General switches
class ApplySkimming(JobProperty):
    """  Apply the skimming (event selection) for the Tight Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    pass
jobproperties.PrimaryDPDFlags_TtbarMuonStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Tight Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_TtbarMuonStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the Tight Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_TtbarMuonStream.add_JobProperty(ApplySlimming)

class ModifiedTTBarFilter2(JobProperty):
    """  Set the properties of the modified ttbar filter."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 25.0*Units.GeV
    AbsoluteMuonIso    = 1.8*Units.GeV
    JetCollection      = "AntiKt4LCTopoJets"
    JetPt              = 30.0*Units.GeV
    JetEta             = 2.5
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_TtbarMuonStream.add_JobProperty(ModifiedTTBarFilter2)



