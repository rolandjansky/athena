# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### JobOptions to run MuGirlTag in xAOD

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getService
from MuGirl.MuGirlRecoConfig import MuGirlRecoConfig

###logfile
from AthenaCommon.Logging import log

###############################################################################
### Configure MuGirlTag###
def MuGirlTagToolBase(name, configureForTrigger, doStau, **kwargs ):
   from AthenaCommon.AppMgr import ToolSvc
   toolName = "MuGirlRecoTool"
   if configureForTrigger:
      toolName = "TrigMuGirlRecoTool"
   RecoTool = MuGirlRecoConfig(name=toolName,configureForTrigger=configureForTrigger,doStau=doStau)
   ToolSvc += RecoTool
   kwargs.setdefault("MuGirlReconstruction",  RecoTool )
   return CfgMgr.MuonCombined__MuGirlTagTool(name,**kwargs)

def MuGirlTagTool( name='MuGirlTagTool', **kwargs ):
   return MuGirlTagToolBase(name=name,configureForTrigger=False,doStau=True,**kwargs)

def TrigMuGirlTagTool( name='TrigMuGirlTagTool', **kwargs ):
   return MuGirlTagToolBase(name=name,configureForTrigger=True,doStau=True,**kwargs)






