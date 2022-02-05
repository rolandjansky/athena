#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# a small utility script to generate Run 3 navigation graphs
# usage:
# athena navGraphDump.py
# one may need to adjust file name below.
#
# Navigation graphs can be converted from the dot format to pdf using:
# dot -Tpdf input.dot -o output.pdf
# or for all the files: 


from Gaudi.Configuration import *

# Run Three Offline Trigger Checker

#File = "AOD.pool.root.1"
File = "outAOD.pool.root"
#Chain = "HLT_e26_lhtight_nod0_L1EM22VHI"
#Chain = "L2_e26_lhtight_nod0_L1_EM22VHIclhypo"
#Chain = "HLT_e26_lhtight_nod0"
Chain = ""
Level = DEBUG
Skip = 0
Max = 10

##################

import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import topSequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
from TrigEDMConfig.TriggerEDM import EDMLibraries

svcMgr.EventSelector.InputCollections  = [ File ]

cvgsvc = TrigConf__xAODConfigSvc('xAODConfigSvc')
svcMgr += cvgsvc

tdt = Trig__TrigDecisionTool('TrigDecisionTool')
tdt.TrigConfigSvc = cvgsvc
tdt.OutputLevel = Level
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
