# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Activate MboyView. When this module is imported,
MboyView is setup unconditionally (it does not check the
muonboyFlags.MboyView flag)."""

__author__ = "M. Woudtsra, J.F. Laporte"

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr
from RecExConfig.RecFlags import rec as recFlags

topSequence = AlgSequence()

from MboyView.MboyViewConf import MboyView
topSequence += MboyView( SwitchOff = 0 )
# for later reference
theMboyView = topSequence.MboyView

if recFlags.readESD():
    theMboyView.NameOfMboyViewOutputFile   = "Out.MboyView_ESD" 
    theMboyView.ViewMETContainer           = 1
    theMboyView.ViewL1RoIContainer         = 1
elif recFlags.readAOD():
    theMboyView.NameOfMboyViewOutputFile   = "Out.MboyView_AOD"
    theMboyView.ViewDigits                 = 0
    theMboyView.ViewTrackRecordCollection  = 0
    theMboyView.ViewCombinedMuonContainer  = 0
    theMboyView.ViewTrackParticleContainer = 1
    theMboyView.ViewTrkTrackCollection     = 0
    theMboyView.ViewTrkSegmentCollection   = 0
    theMboyView.ViewMcEventCollection      = 0
    theMboyView.ViewEgammaContainer        = 0
    theMboyView.ViewJetContainer           = 1
    theMboyView.ViewMETContainer           = 0
    theMboyView.ViewL1RoIContainer         = 1

# configure MboySvc
if not hasattr(ServiceMgr,'MboySvc'):
  from MboySvc.MboySvcConf import MboySvc
  ServiceMgr += MboySvc()
ServiceMgr.MboySvc.GiveRsLineToDigit = 1

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ToolSvc
from MboyView.MboyViewConf import MboyViewEventInfoTool
theMboyViewEventInfoTool = MboyViewEventInfoTool("ConfiguredMboyViewEventInfoTool")
if globalflags.DataSource() == 'data' :
    theMboyViewEventInfoTool.SIM = 0
else:
    theMboyViewEventInfoTool.SIM = 1
ToolSvc += theMboyViewEventInfoTool
