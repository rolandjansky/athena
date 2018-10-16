# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import re
import sys

import LeptonTaggers.LeptonTaggersConfig as ConfigNew

#------------------------------------------------------------------------------
def ConfigureAntiKt4PV0TrackJets(privateSeq, name):
    return ConfigNew.ConfigureAntiKt4PV0TrackJets(privateSeq, name)

#------------------------------------------------------------------------------
def GetDecoratePromptLeptonAlgs(name="", addSpectators=False):
    return ConfigNew.GetDecoratePromptLeptonAlgs(name, addSpectators)

#------------------------------------------------------------------------------
def GetDecoratePromptTauAlgs():
    return ConfigNew.GetDecoratePromptTauAlgs()

#------------------------------------------------------------------------------
def GetExtraPromptVariablesForDxAOD(name="", addSpectators=False):
    return ConfigNew.GetExtraPromptVariablesForDxAOD(name, addSpectators)

#------------------------------------------------------------------------------
def GetExtraPromptTauVariablesForDxAOD():
    return ConfigNew.GetExtraPromptTauVariablesForDxAOD()

