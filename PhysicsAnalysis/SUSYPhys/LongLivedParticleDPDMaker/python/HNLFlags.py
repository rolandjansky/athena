
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        HNLFlags.py
## Authors:     Susumu Oda
## Email:       Susumu.Oda@cern.ch
## Created:     July 2019
##
## Description: Here, flags for displaced HNL filters are defined.
##
##=============================================================================

__doc__ = """job flags for the RPV/LL HNL DRAW"""
__version__ = "0.0.1"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units
from xAODPrimitives.xAODIso import xAODIso
import ROOT

from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLLStream import jobproperties
primRPVLLDESDM=jobproperties.PrimaryDPDFlags_RPVLLStream

class doHnlElMu(JobProperty):
    # prompt electron + displace muon
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doHnlElMu)

class doHnlElEl(JobProperty):
    # prompt electron + displace electron
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doHnlElEl)

class doHnlMuEl(JobProperty):
    # prompt muon + displace electron
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = True
    pass
primRPVLLDESDM.add_JobProperty(doHnlMuEl)

class promptMuonPtThreshHnl(JobProperty):
    # pT threshold for prompt muons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 28.*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(promptMuonPtThreshHnl)

class promptElectronPtThreshHnl(JobProperty):
    # pT threshold for prompt electrons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 28.*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(promptElectronPtThreshHnl)

class displacedMuonPtThreshHnl(JobProperty):
    # pT threshold for displaced muons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 5.*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(displacedMuonPtThreshHnl)

class displacedElectronPtThreshHnl(JobProperty):
    # pT threshold for displaced electrons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 5.*Units.GeV
    pass
primRPVLLDESDM.add_JobProperty(displacedElectronPtThreshHnl)

class promptMuonIsoThreshHnl(JobProperty):
    # isolation threshold for prompt muons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 0.05
    pass
primRPVLLDESDM.add_JobProperty(promptMuonIsoThreshHnl)

class promptElectronIsoThreshHnl(JobProperty):
    # isolation threshold for prompt electrons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 0.05
    pass
primRPVLLDESDM.add_JobProperty(promptElectronIsoThreshHnl)

class displacedMuonIsoThreshHnl(JobProperty):
    # isolation threshold for displaced muons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 9999.
    pass
primRPVLLDESDM.add_JobProperty(displacedMuonIsoThreshHnl)

class displacedElectronIsoThreshHnl(JobProperty):
    # isolation threshold for displaced electrons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 9999.
    pass
primRPVLLDESDM.add_JobProperty(displacedElectronIsoThreshHnl)

class promptElectronIsoTypeHnl(JobProperty):
    # isolation type for prompt electrons
    statusOn = True
    allowedTypes = ["int"]
    StoredValue = xAODIso.ptcone30
    pass
primRPVLLDESDM.add_JobProperty(promptElectronIsoTypeHnl)

class displacedElectronIsoTypeHnl(JobProperty):
    # isolation type for displaced electrons
    statusOn = True
    allowedTypes = ["int"]
    StoredValue = xAODIso.ptcone30
    pass
primRPVLLDESDM.add_JobProperty(displacedElectronIsoTypeHnl)

class promptMuonTypesHnl(JobProperty):
    # allowed types of prompt muons
    statusOn = True
    allowedTypes = ["list"]
    StoredValue = [ROOT.xAOD.Muon.Combined]
    pass
primRPVLLDESDM.add_JobProperty(promptMuonTypesHnl)

class displacedMuonTypesHnl(JobProperty):
    # allowed types of displaced muons
    statusOn = True
    allowedTypes = ["list"]
    StoredValue = [ROOT.xAOD.Muon.Combined,
                   ROOT.xAOD.Muon.MuonStandAlone]
    pass
primRPVLLDESDM.add_JobProperty(displacedMuonTypesHnl)

class promptElectronIDHnl(JobProperty):
    # electron identification working point for prompt electrons
    statusOn = True
    allowedTypes = ["str"]
    StoredValue = "LHLoose"
    pass
primRPVLLDESDM.add_JobProperty(promptElectronIDHnl)

class displacedMuonD0ThreshHnl(JobProperty):
    # minimum |d0| threshold for displaced muons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 0.1*Units.mm
    pass
primRPVLLDESDM.add_JobProperty(displacedMuonD0ThreshHnl)

class displacedElectronD0ThreshHnl(JobProperty):
    # minimum |d0| threshold for displaced electrons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 1.0*Units.mm
    pass
primRPVLLDESDM.add_JobProperty(displacedElectronD0ThreshHnl)

class doHnlMultiLeptonTriggers(JobProperty):
    # use multi-lepton triggers in addition to single lepton triggers
    statusOn = True
    allowedTypes = ["bool"]
    StoredValue = False
    pass
primRPVLLDESDM.add_JobProperty(doHnlMultiLeptonTriggers)

class leptonDphiThreshHnl(JobProperty):
    # minimum threshold for |dPhi| between prompt and displaced leptons
    statusOn = True
    allowedTypes = ["float"]
    StoredValue = 0.0*Units.radian
    pass
primRPVLLDESDM.add_JobProperty(leptonDphiThreshHnl)
