# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from MuonCondAlg.MuonTopCondAlgConfigRUN2 import TgcCondDbAlg
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if not athenaCommonFlags.isOnline:
    condSequence = AthSequencer("AthCondSeq")
    if not hasattr(condSequence,"TgcCondDbAlg"):
        condSequence += TgcCondDbAlg("TgcCondDbAlg")

