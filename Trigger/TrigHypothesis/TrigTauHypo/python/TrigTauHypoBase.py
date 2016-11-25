# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################################
#
#  Settings of Tau Slice HYPOs
# Author: Geng-Yuan  Jeng
######################################################################

from TriggerJobOpts.TriggerFlags import TriggerFlags
from TrigTauHypo.TrigTauHypoConf import *
from AthenaCommon.Logging import logging

# monitoring part, to disable set topOption TriggerFlags.enableMonitoring = []
from TrigTauHypo.TrigTauHypoMonitoring \
     import setTauMonTools, setTauT2IDMonTools, setTauT2TauMonTools, setHLTTrackPreselMonTools, setHLTCaloPreselMonTools

from TrigTauHypo.TrigTauHypoDefaults import *

## Parsing variable cut function
def setVarCut (obj, var, val):
    logging.getLogger("TrigTauHypoConfig").debug(obj.name)
    for i in range(len(var)):
        logging.getLogger("TrigTauHypoConfig").debug('Update variable cut: %s = %f'%(var[i],val[i]))
        obj.__setattr__(var[i], val[i])

## Calo-Based Pre-selection
class HLTCaloTauHypoBase(HLTCaloPreSelHypo):
    __slots__ = []
    def __init__(self, name):
        super(HLTCaloTauHypoBase, self).__init__(name)
        setHLTCaloPreselMonTools(self)
        #setTauMonTools(self)

class HLTCaloTauHypo (HLTCaloTauHypoBase):
    __slots__ = []
    def __init__(self, name = "HLTCaloTauHypo", var = [], val = []):
        super(HLTCaloTauHypo, self).__init__(name)
        #applyDefaults(self, T2Calo_defaults)
        setVarCut(self, var, val)

## Track-Based Pre-selection
class HLTTrackTauHypoBase(HLTTrackPreSelHypo):
    __slots__ = []
    def __init__(self, name):
        super(HLTTrackTauHypoBase, self).__init__(name)
        setHLTTrackPreselMonTools(self)
        #setTauMonTools(self)

class HLTTrackTauHypo_rejectNoTracks(HLTTrackTauHypoBase):
    __slots__ = []
    def __init__(self, name="HLTTrackTauHypo_rejectNoTracks"):
        super(HLTTrackTauHypo_rejectNoTracks, self).__init__(name)
        self.rejectNoTracks = True
        self.TracksInCoreCut = 999
        self.TracksInIsoCut = 999
        self.DeltaRLeadTrkRoI = 0.0
        self.TrackVariableOuter = 0.0
        self.TrackVariableCore = 0.0
        #setHLTTrackPreselMonTools(self)
        #setTauMonTools(self)

        if TriggerFlags.run2Config=='2016':
             self.relaxHighPt=False;

class HLTTrackTauHypo (HLTTrackTauHypoBase):
    __slots__ = []
    def __init__(self, name = "HLTTrackTauHypo", var = [], val = []):
        super(HLTTrackTauHypo, self).__init__(name)
        #applyDefaults(self, T2Calo_defaults)
        setVarCut(self, var, val)

## T2CaloTau
class T2CaloTauHypoBase (T2CaloTauHypo):
    __slots__ = []
    def __init__(self, name):
        super(T2CaloTauHypoBase, self).__init__(name)
        setTauMonTools(self)

class T2CaloTauHypo_tauNoCut (T2CaloTauHypoBase):
    __slots__ = []
    def __init__(self, name = "T2CaloTauHypo_tauNoCut"):
        super(T2CaloTauHypo_tauNoCut, self).__init__(name)        
        self.forceAccept = True

class T2CaloTauHypo (T2CaloTauHypoBase):
    __slots__ = []
    def __init__(self, name = "T2CaloTauHypo", var = [], val = []):
        super(T2CaloTauHypo, self).__init__(name)
        applyDefaults(self, T2Calo_defaults)
        setVarCut(self, var, val)        

## T2IDTau
class T2IDTauHypoBase (T2IDTauHypo):
    __slots__ = []
    def __init__(self, name):
        super(T2IDTauHypoBase, self).__init__(name)
        setTauT2IDMonTools(self)

class T2IDTauHypo_tauNoCut (T2IDTauHypoBase):
    __slots__ = []
    def __init__(self, name = "T2IDTauHypo_tauNoCut"):
        super(T2IDTauHypo_tauNoCut, self).__init__(name)
        self.NTrkMin = -1
        self.NTrkMax = 1000
        self.forceAccept = True

class T2IDTauHypo (T2IDTauHypoBase):
    __slots__ = []
    def __init__(self, name = "T2IDTauHypo", var = [], val = []):
        super(T2IDTauHypo, self).__init__(name)
        applyDefaults(self, T2ID_defaults)
        setVarCut(self, var, val)

## T2Tau
class T2TauHypoBase (T2TauHypo):
    __slots__ = []
    def __init__(self, name):
        super(T2TauHypoBase, self).__init__(name)
        setTauT2TauMonTools(self)

class T2TauHypo_tauNoCut (T2TauHypoBase):
    __slots__ = []
    def __init__(self, name = "T2TauHypo_tauNoCut"):
        super(T2TauHypo_tauNoCut, self).__init__(name)       
        self.forceAccept = True

class T2TauHypo (T2TauHypoBase):
    __slots__ = []
    def __init__(self, name = "T2TauHypo", var = [], val = []):
        super(T2TauHypo, self).__init__(name)
        applyDefaults(self, T2Tau_defaults)
        setVarCut(self, var, val)
