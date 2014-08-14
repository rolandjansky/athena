# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        PrimaryDPDFlags_SingleMuonStream
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
class PrimaryDPDFlags_SingleMuonStream(JobPropertyContainer):
    """ The Primary Muon DPD flag/job property  container."""
jobproperties.add_Container(PrimaryDPDFlags_SingleMuonStream)

primDPDsingleMu=jobproperties.PrimaryDPDFlags_SingleMuonStream



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
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(ApplySkimming)

class ApplyThinning(JobProperty):
    """  Apply the thinning for the Tight Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(ApplyThinning)

class ApplySlimming(JobProperty):
    """  Apply the thinning for the Tight Muon output DPD stream."""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(ApplySlimming)


class TauForZtautauFilter(JobProperty):
    """  Set the properties of the high muon with tau fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 25.0*Units.GeV
    MuonEtMax          = 45.0*Units.GeV
    RelativeMuonIso    = 0.06
    RelativeMuonEtIso  = 0.2
    TauNTrackMin       = 1
    TauNTrackMax       = 3
    TauEt              = 20.0*Units.GeV
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(TauForZtautauFilter)

class MuonTauLooseFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn              = True
    allowedTypes          = ['bool']
    StoredValue           = True
    MuonEt                = 22.0*Units.GeV
    MuonEtMax             = 40.0*Units.GeV
    RelativeMuonIso       = 0.06
    RelativeMuonEtIso     = 0.2
    TauNTrackMin          = 1
    TauNTrackMax          = 3
    TauEt                 = 15.0*Units.GeV
    Prescale              = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(MuonTauLooseFilter)

class ElectronMuonFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    ElectronEt         = 15.0*Units.GeV
    VetoElectronAuthor = [ egammaParameters.AuthorFrwd ]
    ElectronID         = egammaPID.ElectronIDMediumPP
    RemoveCrack        = False
    MuonEt             = 15.0*Units.GeV
    RelativeMuonIso    = 0.2
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(ElectronMuonFilter)

class SemiLepTTBarFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 25.0*Units.GeV
    RelativeMuonIso    = 0.2
    MissingEt          = 25.0*Units.GeV
    JetCollection      = "AntiKt4TopoEMJets"
    JetPt              = 25.0*Units.GeV
    JetEta             = 2.5
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(SemiLepTTBarFilter)

class ModifiedTTBarFilter(JobProperty):
    """  Set the properties of the modified ttbar filter."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 25.0*Units.GeV
    RelativeMuonIso    = 0.2
    MissingEt          = 40.0*Units.GeV
    JetCollection      = "AntiKt4LCTopoJets"
    JetPt              = 25.0*Units.GeV
    JetEta             = 2.5
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(ModifiedTTBarFilter)

class WmunuFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 25.0*Units.GeV
    RelativeMuonIso    = 0.2
    MissingEt          = 25.0*Units.GeV
    Prescale           = 10
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(WmunuFilter)

class WmunuHighPtFilter(JobProperty):
    """  Set the properties of the Wmunu+high pt object fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 20.0*Units.GeV
    ExtraObjectPt      = 60.0*Units.GeV
    RelativeMuonIso    = 0.2
    MissingEt          = 20.0*Units.GeV
    JetCollection      = "AntiKt4TopoEMJets"
#     JetPt              = 50.0*Units.GeV
    JetEta             = 2.5
    Prescale           = 2
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(WmunuHighPtFilter)

class DiIsolatedMuonFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 15.0*Units.GeV
    # RelativeMuonIso    = 0.2
    Prescale           = 3
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(DiIsolatedMuonFilter)

class ZmumuFilter(JobProperty):
    """  Set the properties of the Z->mumu fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    LeadingMuonPt      = 20.0*Units.GeV
    SubleadingMuonPt   = 15.0*Units.GeV
    # RelativeMuonIso    = 0.2
    MassMin            = 40.0*Units.GeV
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(ZmumuFilter)

class IsolatedMuonFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 25.0*Units.GeV
    RelativeMuonIso    = 0.2
    Prescale           = 15
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(IsolatedMuonFilter)

class HighPtMuonFilter(JobProperty):
    """  Set the properties of the high et muon fitler."""
    statusOn           = True
    allowedTypes       = ['bool']
    StoredValue        = True
    MuonEt             = 75.0*Units.GeV
    Prescale           = 1
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(HighPtMuonFilter)

class JpsiTriggerFilter(JobProperty):
    """  Set the properties of the J/psi trigger fitler."""
    statusOn         = True
    allowedTypes     = ['bool']
    StoredValue      = True
    TriggerNames     = [ [ "EF_2mu6_Jpsimumu",         2 ],
                         [ "EF_2mu4T_Jpsimumu",        2 ],
                         [ "EF_mu6_Jpsimumu_tight",         2 ] ]
    Prescale         = 15
    pass
jobproperties.PrimaryDPDFlags_SingleMuonStream.add_JobProperty(JpsiTriggerFilter)



