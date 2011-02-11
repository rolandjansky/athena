# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Flags to steer the DQA for Muon Combined Reconstruction."""

from AthenaCommon.JobProperties import JobProperty,JobPropertyContainer,jobproperties
from MuonRecExample.MuonRecUtils import logMuon,RecConfigInfo,AutoLoadContainerJobProperty,\
     AutoLoadContainer,fillJobPropertyContainer,override_lock_and_set_Value
from AthenaCommon import BeamFlags
beamFlags = jobproperties.Beam

class Enabled(JobProperty):
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

    def _do_action(self):
        """Set all flags Off"""
        global _propertiesList
        for p in _propertiesList:
            if p is not self: p.set_On()

    def _undo_action(self):
        """Set all flags Off"""
        global _propertiesList
        for p in _propertiesList:
            if p is not self: p.set_Off()


class doMuonStandaloneMon(JobProperty):
    """Run DQA on the standalone muons MuidSA/Muonboy"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doMuonCombinedMon(JobProperty):
    """Run DQA on the combined track muons MuidCB/Staco"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doMuonMSTagMon(JobProperty):
    """Run DQA on the tagged muons MuTag/MuGirl/MuTagIMO"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doMuonCaloTagMon(JobProperty):
    """Run DQA on the calo tagged muons MuonCaloLR/MuonCaloTag"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class doMuonEfficiency(JobProperty):
    """Run DQA on the muon efficiency"""
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True



class MuonCombinedDQA(JobPropertyContainer):
    """The flags to steer muon combined reconstruction"""

    def setDefaults(self):
        global beamFlags
        if beamFlags.beamType()=='cosmics' or beamFlags.beamType()=='singlebeam':
            self.doMuonStandaloneMon = True
            self.doMuonCombinedMon   = True
            self.doMuonMSTagMon      = True
            self.doMuonCaloTagMon    = True
            self.doMuonEfficiency    = True
        else: # collisions
            self.doMuonStandaloneMon = True
            self.doMuonCombinedMon   = True
            self.doMuonMSTagMon      = True
            self.doMuonCaloTagMon    = True
            self.doMuonEfficiency    = True

    def sync_Flags(self):
        """Synchronise all flags to make them consistent"""
            

jobproperties.add_Container(MuonCombinedDQA)
muonCombinedDQAFlags = jobproperties.MuonCombinedDQA

# fill properties in container and store for later use (due to lack of iterator)
_propertiesList = fillJobPropertyContainer(muonCombinedDQAFlags,__name__)

muonCombinedDQAFlags.setDefaults()
