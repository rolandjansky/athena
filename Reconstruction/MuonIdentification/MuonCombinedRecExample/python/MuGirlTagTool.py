# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

### JobOptions to run MuGirlTag in xAOD

from AthenaCommon import CfgMgr
from AthenaCommon.CfgGetter import getPublicTool,getService

from RecExConfig.RecFlags import rec

from MuGirl.MuGirlRecoConfig import MuGirlRecoConfig
from MuonCombinedRecExample.MuonCombinedFitTools import CombinedMuonTrackBuilder,CombinedMuonTrackBuilderFit,MuidSegmentRegionRecoveryTool
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags

from MuonRecExample.MooreTools import MuonSeededSegmentFinder, MuonChamberHoleRecoveryTool
from MuonRecExample.MuonRecTools import DCMathSegmentMaker

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

def DCMathStauSegmentMaker( name="DCMathStauSegmentMaker", **kwargs ):
   kwargs.setdefault("MdtCreator", getPublicTool("MdtDriftCircleOnTrackCreatorStau") )   
   return DCMathSegmentMaker(name,**kwargs)

def MuonStauChamberHoleRecoveryTool(name="MuonStauChamberHoleRecoveryTool",**kwargs):
   kwargs.setdefault("MdtRotCreator", getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
   return MuonChamberHoleRecoveryTool(name,**kwargs)

def MuonStauSeededSegmentFinder( name="MuonStauSeededSegmentFinder", **kwargs ):
    kwargs.setdefault("MdtRotCreator", getPublicTool("MdtDriftCircleOnTrackCreatorStau") )
    kwargs.setdefault("SegmentMaker", getPublicTool("DCMathStauSegmentMaker") )
    kwargs.setdefault("SegmentMakerNoHoles", getPublicTool("DCMathStauSegmentMaker") )
    return MuonSeededSegmentFinder(name,**kwargs)

def MuonStauSegmentRegionRecoveryTool(name="MuonStauSegmentRegionRecoveryTool",**kwargs ):
   kwargs.setdefault("SeededSegmentFinder", getPublicTool("MuonStauSeededSegmentFinder") )
   kwargs.setdefault("ChamberHoleRecoveryTool", getPublicTool("MuonStauChamberHoleRecoveryTool") )
   kwargs.setdefault("Fitter",  getPublicTool("CombinedStauTrackBuilderFit") )
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
   return CfgMgr.MuonCombined__MuonInsideOutRecoTool(name,**kwargs )

def MuonStauRecoTool( name="MuonStauRecoTool", **kwargs ):
   kwargs.setdefault("ConsideredPDGs", [13,-13,1000015,-1000015])
   kwargs.setdefault("DoTruth", rec.doTruth() )
   kwargs.setdefault("DoSummary", muonCombinedRecFlags.printSummary() )
   kwargs.setdefault("MuonSegmentMaker", getPublicTool("DCMathStauSegmentMaker") )
   kwargs.setdefault("MuonInsideOutRecoTool", getPublicTool("MuonStauInsideOutRecoTool") )
   return CfgMgr.MuonCombined__MuonStauRecoTool(name,**kwargs )


        #ToolSvc +=  CfgMgr.Muon__MuonSystemExtensionTool("MuonSystemExtensionTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonPrepRawDataCollectionProviderTool("MuonPrepRawDataCollectionProviderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonLayerHashProviderTool("MuonLayerHashProviderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonPRDSelectionTool("MuonPRDSelectionTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonLayerSegmentFinderTool("MuonLayerSegmentFinderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__MuonClusterSegmentFinderTool("MuonClusterSegmentFinderTool",OutputLevel=VERBOSE)
        #ToolSvc +=  CfgMgr.Muon__VERBOSE)






