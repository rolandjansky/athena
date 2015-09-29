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

def MuonInsideOutRecoTool( name="MuonInsideOutRecoTool", **kwargs ):
   return CfgMgr.MuonCombined__MuonInsideOutRecoTool(name,**kwargs )

def MuonCandidateTrackBuilderTool( name="MuonCandidateTrackBuilderTool",**kwargs):
   return CfgMgr.Muon__MuonCandidateTrackBuilderTool(name,**kwargs)

def MuonLayerSegmentMatchingTool( name="MuonLayerSegmentMatchingTool",**kwargs):
   return CfgMgr.Muon__MuonLayerSegmentMatchingTool(name,**kwargs)

def MuonLayerAmbiguitySolverTool( name="MuonLayerAmbiguitySolverTool",**kwargs):
   return CfgMgr.Muon__MuonLayerAmbiguitySolverTool(name,**kwargs)

def MuonRecoValidationTool( name="MuonRecoValidationTool",**kwargs):
   return CfgMgr.Muon__MuonRecoValidationTool(name,**kwargs)

def MuonStauRecoTool( name="MuonStauRecoTool", **kwargs ):
   kwargs.setdefault("MuonSegmentMaker", getPublicTool("DCMathT0FitSegmentMaker") )
   return CfgMgr.MuonCombined__MuonStauRecoTool(name,**kwargs )

        #ToolSvc +=  CfgMgr.Muon__MuonSystemExtensionTool("MuonSystemExtensionTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonPrepRawDataCollectionProviderTool("MuonPrepRawDataCollectionProviderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonLayerHashProviderTool("MuonLayerHashProviderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonPRDSelectionTool("MuonPRDSelectionTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonLayerSegmentFinderTool("MuonLayerSegmentFinderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonClusterSegmentFinderTool("MuonClusterSegmentFinderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__VERBOSE)






