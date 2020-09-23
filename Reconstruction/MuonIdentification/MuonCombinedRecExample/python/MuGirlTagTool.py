# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

### JobOptions to run MuGirlTag in xAOD

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone,getService
from AthenaCommon.GlobalFlags import globalflags

from RecExConfig.RecFlags import rec

from MuonCombinedRecExample.MuonCombinedFitTools import CombinedMuonTrackBuilder,CombinedMuonTrackBuilderFit,MuidSegmentRegionRecoveryTool
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

from MuonRecExample.MooreTools import MuonSeededSegmentFinder, MuonChamberHoleRecoveryTool
from MuonRecExample.MuonRecTools import DCMathSegmentMaker

###logfile
from AthenaCommon.Logging import log

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

def MuonInsideOutRecoTool( name="MuonInsideOutRecoTool", **kwargs ):
   if TriggerFlags.MuonSlice.doTrigMuonConfig:
      kwargs.setdefault("VertexContainer", "")
   import MuonCombinedRecExample.CombinedMuonTrackSummary
   from AthenaCommon.AppMgr import ToolSvc
   kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)
   kwargs.setdefault("MuonLayerSegmentFinderTool", getPublicTool("MuonLayerSegmentFinderTool"))
   return CfgMgr.MuonCombined__MuonInsideOutRecoTool(name,**kwargs )

def MuonCandidateTrackBuilderTool( name="MuonCandidateTrackBuilderTool",**kwargs):
   return CfgMgr.Muon__MuonCandidateTrackBuilderTool(name,**kwargs)

def MuonLayerSegmentMatchingTool( name="MuonLayerSegmentMatchingTool",**kwargs):
   return CfgMgr.Muon__MuonLayerSegmentMatchingTool(name,**kwargs)

def MuonLayerAmbiguitySolverTool( name="MuonLayerAmbiguitySolverTool",**kwargs):
   return CfgMgr.Muon__MuonLayerAmbiguitySolverTool(name,**kwargs)

def MuonRecoValidationTool( name="MuonRecoValidationTool",**kwargs):
   if globalflags.DataSource() != 'data':
      kwargs.setdefault("isMC",True)
   return CfgMgr.Muon__MuonRecoValidationTool(name,**kwargs)

def DCMathStauSegmentMaker( name="DCMathStauSegmentMaker", **kwargs ):
   kwargs.setdefault("MdtCreator", getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
   return DCMathSegmentMaker(name,**kwargs)

def MuonStauChamberHoleRecoveryTool(name="MuonStauChamberHoleRecoveryTool",**kwargs):
   kwargs.setdefault("MdtRotCreator", getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
   if not MuonGeometryFlags.hasCSC():
      kwargs.setdefault("CscRotCreator", "" )
      kwargs.setdefault("CscPrepDataContainer", "" )
   return MuonChamberHoleRecoveryTool(name,**kwargs)

def MuonStauSeededSegmentFinder( name="MuonStauSeededSegmentFinder", **kwargs ):
    kwargs.setdefault("MdtRotCreator", getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
    kwargs.setdefault("SegmentMaker", getPublicTool("DCMathStauSegmentMaker") )
    kwargs.setdefault("SegmentMakerNoHoles", getPublicTool("DCMathStauSegmentMaker") )
    if not MuonGeometryFlags.hasCSC(): kwargs.setdefault("CscPrepDataContainer","")
    if not MuonGeometryFlags.hasSTGC(): kwargs.setdefault("sTgcPrepDataContainer","")
    if not MuonGeometryFlags.hasMM(): kwargs.setdefault("MMPrepDataContainer","")

    return MuonSeededSegmentFinder(name,**kwargs)

def MuonStauSegmentRegionRecoveryTool(name="MuonStauSegmentRegionRecoveryTool",**kwargs ):
   kwargs.setdefault("SeededSegmentFinder", getPublicTool("MuonStauSeededSegmentFinder") )
   kwargs.setdefault("ChamberHoleRecoveryTool", getPublicTool("MuonStauChamberHoleRecoveryTool") )
   kwargs.setdefault("Fitter",  getPublicTool("CombinedStauTrackBuilderFit") )
   if TriggerFlags.MuonSlice.doTrigMuonConfig and athenaCommonFlags.isOnline:
      kwargs.setdefault('MdtCondKey', "")
   return MuidSegmentRegionRecoveryTool(name,**kwargs)

def CombinedStauTrackBuilderFit( name='CombinedStauTrackBuilderFit', **kwargs ):
   kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
   return CombinedMuonTrackBuilderFit(name,**kwargs )

def CombinedStauTrackBuilder( name='CombinedStauTrackBuilder', **kwargs ):
   kwargs.setdefault("MdtRotCreator"                 , getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
   kwargs.setdefault("MuonHoleRecovery"              , getPublicTool("MuonStauSegmentRegionRecoveryTool") )
   return CombinedMuonTrackBuilder(name,**kwargs )

def MuonStauCandidateTrackBuilderTool( name="MuonStauCandidateTrackBuilderTool",**kwargs):

   kwargs.setdefault("MuonTrackBuilder",  getPublicTool("CombinedStauTrackBuilder") )
   return CfgMgr.Muon__MuonCandidateTrackBuilderTool(name,**kwargs)

def MuonStauInsideOutRecoTool( name="MuonStauInsideOutRecoTool", **kwargs ):
   kwargs.setdefault("MuonCandidateTrackBuilderTool", getPublicTool("MuonStauCandidateTrackBuilderTool") )
   if TriggerFlags.MuonSlice.doTrigMuonConfig:
      kwargs.setdefault("VertexContainer", "")
   import MuonCombinedRecExample.CombinedMuonTrackSummary
   from AthenaCommon.AppMgr import ToolSvc
   kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)
   return CfgMgr.MuonCombined__MuonInsideOutRecoTool(name,**kwargs )

def MuonStauRecoTool( name="MuonStauRecoTool", **kwargs ):
   kwargs.setdefault("ConsideredPDGs", [13,-13,1000015,-1000015])
   kwargs.setdefault("DoTruth", rec.doTruth() )
   kwargs.setdefault("DoSummary", muonCombinedRecFlags.printSummary() )
   kwargs.setdefault("MuonSegmentMaker", getPublicTool("DCMathStauSegmentMaker") )
   kwargs.setdefault("MuonInsideOutRecoTool", getPublicTool("MuonStauInsideOutRecoTool") )
   kwargs.setdefault("TrackAmbiguityProcessor", getPublicTool("MuonAmbiProcessor") )
   return CfgMgr.MuonCombined__MuonStauRecoTool(name,**kwargs )





