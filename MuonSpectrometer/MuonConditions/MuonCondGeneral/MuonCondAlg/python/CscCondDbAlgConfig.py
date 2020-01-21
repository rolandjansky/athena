# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from MuonCondAlg.MuonTopCondAlgConfigRUN2 import CscCondDbAlg
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

condSequence = AthSequencer("AthCondSeq")
if not hasattr(condSequence,"CscCondDbAlg"): 
    condSequence += CscCondDbAlg("CscCondDbAlg")

