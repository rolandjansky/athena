# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from Gaudi.Configuration import *

# Run Three Offline Trigger Checker

File = "tmp/AOD.pool.root"
Chain = "HLT_j80_L1J15"
Level = DEBUG
Skip = 0
Max = 1

##################

import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import topSequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool

svcMgr.EventSelector.InputCollections  = [ File ]

cvgsvc = TrigConf__xAODConfigSvc('xAODConfigSvc')
svcMgr += cvgsvc

tdt = Trig__TrigDecisionTool('TrigDecisionTool')
tdt.TrigConfigSvc = cvgsvc
tdt.NavigationFormat = "TrigComposite"
ToolSvc += tdt

from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
MessageSvc.defaultLimit = 9999999
MessageSvc.useColors = True
checker                                 = TrigEDMChecker()
checker.TriggerDecisionTool             = tdt
checker.OutputLevel                     = Level
checker.DumpNavigationForChain          = Chain
checker.doDumpAll                       = False
checker.doDumpTrigCompsiteNavigation    = True
checker.doDumpAllTrigComposite          = False
checker.doDumpStoreGate                 = False
checker.doTDTCheck                      = True
topSequence += checker

svcMgr.EventSelector.SkipEvents = Skip
theApp.EvtMax = Max
