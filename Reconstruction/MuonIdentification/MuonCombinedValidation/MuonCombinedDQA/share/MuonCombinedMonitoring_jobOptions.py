#
# JobOptions to configure MuonCombined DQA monitoring
# MuonCombinedMonManager must be set up before calling these jobOptions
#

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from MuonCombinedDQA.MuonCombinedDQAFlags import muonCombinedDQAFlags

include("MuonCombinedTrackMon/MuonCombinedTrackValidation_jobOptions.py")
