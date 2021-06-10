# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#---------------------------------------------------------
# Author      : B.Trocme (LPSC Grenoble) 
# Description : JO steering CaloBaselineMon
#
#----------------------------------------------------------

from CaloMonitoring.CaloMonitoringConf import CaloBaselineMon
 
from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaCommon.GlobalFlags  import globalflags
if 'rec' not in dir():
   from RecExConfig.RecFlags import rec

from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool

# BunchCrossing info
from LumiBlockComps.BunchCrossingCondAlgDefault import BunchCrossingCondAlgDefault
BunchCrossingCondAlgDefault()

tmp_CaloBaselineMon = {"useBadLBTool":FALSE,
                       "useReadyFilterTool":FALSE,
                       "useLArNoisyAlg":FALSE,
                       "useBeamBackgroundRemoval":FALSE,
                       "useLArCollisionFilter":FALSE,
                       "pedestalMon_BCIDmin":0,
                       "bcidtoolMon_BCIDmax":0}

if not (DQMonFlags.monManEnvironment == 'online' or globalflags.DataSource.get_Value() == 'geant4' or globalflags.DataSource.get_Value() == 'geant3'):
  tmp_CaloBaselineMon["useBadLBTool"]=TRUE
  tmp_CaloBaselineMon["useReadyFilterTool"]=TRUE
  tmp_CaloBaselineMon["useLArNoisyAlg"] = TRUE

if rec.triggerStream()=='CosmicCalo':
  tmp_CaloBaselineMon["useLArCollisionFilter"] = TRUE
  tmp_CaloBaselineMon["pedestalMon_BCIDmin"] = 40

if rec.triggerStream()=='ZeroBias':
  tmp_CaloBaselineMon["bcidtoolMon_BCIDmax"] = 144


CaloBaseline = CaloBaselineMon(
   name           = "CaloBaseline",
   
   useBadLBTool=tmp_CaloBaselineMon["useBadLBTool"],
   BadLBTool = GetLArBadLBFilterTool(),
   useReadyFilterTool = tmp_CaloBaselineMon["useReadyFilterTool"],
   ReadyFilterTool = GetAtlasReadyFilterTool(),
   useLArCollisionFilterTool = tmp_CaloBaselineMon["useLArCollisionFilter"],
   useLArNoisyAlg = tmp_CaloBaselineMon["useLArNoisyAlg"],
   useBeamBackgroundRemoval = tmp_CaloBaselineMon["useBeamBackgroundRemoval"],
   pedestalMon_BCIDmin = tmp_CaloBaselineMon["pedestalMon_BCIDmin"],
   bcidtoolMon_BCIDmax = tmp_CaloBaselineMon["bcidtoolMon_BCIDmax"],
)

#ToolSvc += CaloBaseline 
CaloMon.AthenaMonTools += [ CaloBaseline ]
