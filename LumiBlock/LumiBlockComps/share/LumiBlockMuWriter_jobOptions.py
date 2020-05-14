""" Configuration for LumiBlockMuWriter to write mu into xAOD::EventInfo
"""

from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
condalg = LuminosityCondAlgDefault()

from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuWriter
muWriter = LumiBlockMuWriter("LumiBlockMuWriter",LumiDataKey=condalg.LuminosityOutputKey)

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condSeq += muWriter
