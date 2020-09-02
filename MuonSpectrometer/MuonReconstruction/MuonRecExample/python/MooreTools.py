# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of tools for Moore muon reconstruction"""

###############################################################
#
# Configuration for Moore
#
#==============================================================
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

import copy

from AthenaCommon.AppMgr import ServiceMgr,ToolSvc
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon import CfgMgr
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

from RecExConfig.RecFlags import rec

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

from AthenaCommon.ConfiguredFactory import getProperty
from IOVDbSvc.CondDB import conddb

from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber

from .MuonRecTools import MuonExtrapolator, MuonChi2TrackFitter, MdtDriftCircleOnTrackCreator, MuonRK_Propagator
from .MuonRecUtils import logMuon,ConfiguredBase,ExtraFlags


from .MuonRecFlags import muonRecFlags
from .MuonStandaloneFlags import muonStandaloneFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
#==============================================================

# call  setDefaults to update flags
muonRecFlags.setDefaults()
muonStandaloneFlags.setDefaults()

#
# Tools for Moore SegmentMaking
#
# segment makers

#  segment selection
def MuonPatternSegmentMaker(name="MuonPatternSegmentMaker",extraFlags=None,**kwargs):
    if extraFlags is None: extraFlags = ExtraFlags()    
    beamType       = extraFlags.setFlagDefault(beamFlags.beamType)
    doSegmentT0Fit = extraFlags.setFlagDefault(muonRecFlags.doSegmentT0Fit)

    if "MdtCreator" not in kwargs:
        # on data configure a MdtDriftCircleOnTrackCreator for the segment finding with reduced errors
        # when using the t0 refit enlarge the time window
        if globalflags.DataSource() == 'data' and beamFlags.beamType() == 'collisions':
            if doSegmentT0Fit:
                mdtCreator = getPublicToolClone( "MdtDriftCircleOnTrackCreatorSegmentFinding", "MdtDriftCircleOnTrackCreator", 
                                                 CreateTubeHit = False, TimeWindowSetting = mdtCalibWindowNumber('Collision_t0fit') )
            else:
                mdtCreator = getPublicToolClone( "MdtDriftCircleOnTrackCreatorSegmentFinding", "MdtDriftCircleOnTrackCreator", 
                                                 CreateTubeHit = False, TimeWindowSetting = mdtCalibWindowNumber('Collision_data') )
            kwargs["MdtCreator"] = mdtCreator

    if beamType == 'cosmics':
        kwargs.setdefault("AngleCutPhi", 1e9)
        kwargs.setdefault("DropDistance", 100000000.)

    return CfgMgr.Muon__MuonPatternSegmentMaker(name,**kwargs)
# end of factory function MuonPatternSegmentMaker


class MuonCurvedSegmentCombiner(CfgMgr.Muon__MuonCurvedSegmentCombiner,ConfiguredBase):
    __slots__ = ()
    def __init__(self,name="MuonCurvedSegmentCombiner",**kwargs):
        self.applyUserDefaults(kwargs,name)
        
        if (beamFlags.beamType() == 'singlebeam' or beamFlags.beamType() == 'cosmics'):
            kwargs.setdefault( "MissedHitsCut", 100 )
            kwargs.setdefault( "AddUnassociatedMiddleEndcapSegments", False )
        elif globalflags.DataSource() == 'data':  #collisions-data or simulation first data
            kwargs.setdefault( "MissedHitsCut", 100 )
        else:
            kwargs.setdefault( "MissedHitsCut", 4 )
        
        kwargs.setdefault("DoCosmics", muonStandaloneFlags.reconstructionMode() != 'collisions' )
        kwargs.setdefault( "AddAll2DCscs", False )
        kwargs.setdefault( "UseCscSegments", muonRecFlags.doCSCs() )
        kwargs.setdefault( "AddUnassociatedMiddleEndcapSegments", True )

        super(MuonCurvedSegmentCombiner,self).__init__(name,**kwargs)        

# end of class MuonCurvedSegmentCombiner

    

# the segment making supertool
class MooSegmentCombinationFinder(CfgMgr.Muon__MooSegmentCombinationFinder,ConfiguredBase):
    __slots__ = ()
    
    def __init__(self,name="MooSegmentFinder",**kwargs):

        kwargs.setdefault( "SegmentCombiner", "MuonCurvedSegmentCombiner" )
        kwargs.setdefault( "SegmentCombinationCleaner", "MuonSegmentCombinationCleanerTool" )
        if( muonStandaloneFlags.reconstructionMode() == 'collisions'): 
            kwargs.setdefault( "HoughPatternFinder", "MuonLayerHoughTool" )
        else:
            kwargs.setdefault( "HoughPatternFinder", "MuonHoughPatternFinderTool" )

        kwargs.setdefault( "MdtSegmentMaker", "MuonPatternSegmentMaker" )
        kwargs.setdefault( "DoSegmentCombinations", False )
        kwargs.setdefault( "DoSegmentCombinationCleaning", False )
        kwargs.setdefault( "DoCscSegments", muonRecFlags.doCSCs() )
        kwargs.setdefault( "DoMdtSegments", muonRecFlags.doMDTs() )
        if muonRecFlags.doCSCs():
            kwargs.setdefault( "Csc2dSegmentMaker","Csc2dSegmentMaker" )
            kwargs.setdefault( "Csc4dSegmentMaker", "Csc4dSegmentMaker" )
        else:
            kwargs.setdefault( "Csc2dSegmentMaker", "" )
            kwargs.setdefault( "Csc4dSegmentMaker", "" )
        if muonStandaloneFlags.printSummary():
            kwargs.setdefault( "DoSummary", True )

        self.applyUserDefaults(kwargs,name)
        # set some defaults

        # call the base class ctor
        super(MooSegmentCombinationFinder,self).__init__(name,**kwargs)

# end of class MooSegmentCombinationFinder


#
# Start of new Moore track finding tools
#

def MooCandidateMatchingTool(name,extraFlags=None,**kwargs):
    namePrefix =getattr(extraFlags,"namePrefix","")
    namePostfix=getattr(extraFlags,"namePostfix","")
    doSegmentPhiMatching = getattr(extraFlags,"doSegmentPhiMatching",None)
    useTrackSegmentMatching = getattr(extraFlags,"useTrackSegmentMatching",muonStandaloneFlags.useTrackSegmentMatching())
    # segment-segment matching
    if doSegmentPhiMatching is not None:
        if not (namePrefix or namePostfix):
            raise RuntimeError("extraFlags ERROR: namePrefix or namePostfix is required if doSegmentPhiMatching is set")

        if "SegmentMatchingTool" not in kwargs:
            kwargs["SegmentMatchingTool"] = getPublicToolClone(namePrefix+"MuonSegmentMatchingTool"+namePostfix,
                                                               "MuonSegmentMatchingTool",
                                                               doPhiMatching=doSegmentPhiMatching)

        if "SegmentMatchingToolTight" not in kwargs:
            kwargs["SegmentMatchingToolTight"] = getPublicToolClone(namePrefix+"MuonSegmentMatchingToolTight"+namePostfix,
                                                                    "MuonSegmentMatchingToolTight",
                                                                    doPhiMatching=doSegmentPhiMatching)
    # track-segment matching
    kwargs.setdefault("DoTrackSegmentMatching", useTrackSegmentMatching)

    kwargs.setdefault("RequireSameSide", muonStandaloneFlags.reconstructionMode() != "collisions")

    if muonRecFlags.useAlignmentCorrections():
        kwargs.setdefault("AlignmentErrorPosX", 5.0)
        kwargs.setdefault("AlignmentErrorPosY", 0.2)
        kwargs.setdefault("AlignmentErrorAngleX", 0.002)
        kwargs.setdefault("AlignmentErrorAngleY", 0.001)

    else: # no alignment corrections
        kwargs.setdefault("AlignmentErrorPosX", 10.0)
        kwargs.setdefault("AlignmentErrorPosY", 5.0)
        kwargs.setdefault("AlignmentErrorAngleX", 0.004)
        kwargs.setdefault("AlignmentErrorAngleY", 0.002)
 
    kwargs.setdefault("MuPatCandidateTool", getPublicTool("MuPatCandidateTool"))
    
    return CfgMgr.Muon__MooCandidateMatchingTool(name,**kwargs)



# track fitter tools
def MooTrackFitter(name="MooTrackFitter", extraFlags=None, **kwargs):

    namePrefix =getattr(extraFlags,"namePrefix","")
    namePostfix=getattr(extraFlags,"namePostfix","")

    kwargs.setdefault("Fitter",          "MCTBFitter")
    kwargs.setdefault("Propagator",      "MuonPropagator")
    kwargs.setdefault("SLFit" ,          not jobproperties.BField.allToroidOn())
    kwargs.setdefault("ReducedChi2Cut",  muonStandaloneFlags.Chi2NDofCut())
    kwargs.setdefault("SegmentMomentum", "MuonSegmentMomentumFromField")
    kwargs.setdefault("CleanPhiHits",              True)
    kwargs.setdefault("UsePreciseHits",            True)
    kwargs.setdefault("UsePrefit",                 False)
    kwargs.setdefault("SeedAtStartOfTrack",        False)

    if muonStandaloneFlags.reconstructionMode() == 'cosmics':
        kwargs.setdefault("SeedWithAvePhi",            True)
        kwargs.setdefault("SeedWithSegmentTheta",      False)
        kwargs.setdefault("Cosmics",                   True)
        kwargs.setdefault("PreCleaningReducedChi2Cut", 5000)
        kwargs.setdefault("SegmentMomentum",           "MuonSegmentMomentum")

    # make instance
    fitter = CfgMgr.Muon__MooTrackFitter(name,**kwargs)

    # make some clones if needed
    if namePrefix or namePostfix:
        oldFitterName = getProperty(fitter,"Fitter").getName()
        newFitterName = namePrefix + oldFitterName + namePostfix
        fitter.Fitter = getPublicToolClone(newFitterName,oldFitterName)

        oldFitterName = getProperty(fitter,"FitterPreFit").getName()
        newFitterName = namePrefix + oldFitterName + namePostfix
        fitter.FitterPreFit = getPublicToolClone(newFitterName,oldFitterName)
 
    return fitter 
    
# end of factory function MooTrackFitter



def MooTrackBuilder(name="MooTrackBuilderTemplate",
                    extraFlags=None,
                    **kwargs):

    namePrefix =getattr(extraFlags,"namePrefix","")
    namePostfix=getattr(extraFlags,"namePostfix","")
    optimiseMomentumResolutionUsingChi2=getattr(extraFlags,"optimiseMomentumResolutionUsingChi2",False)
    
    kwargs.setdefault("Fitter",   "MooTrackFitter")
    kwargs.setdefault("SLFitter", "MooSLTrackFitter")
    kwargs.setdefault("RecalibrateMDTHitsOnTrack", ( (not muonRecFlags.doSegmentT0Fit()) and muonStandaloneFlags.reconstructionMode() == 'collisions') )

    # hardcode some properties before passing on to base class constructors
    if optimiseMomentumResolutionUsingChi2:
        if "ErrorOptimisationTool" not in kwargs:
            if namePrefix or namePostfix:
                tool = getPublicToolClone(namePrefix+"MuonErrorOptimisationTool"+namePostfix,
                                          "MuonErrorOptimisationTool",
                                          extraFlags=extraFlags)
            else:
                tool = getPublicTool("MuonErrorOptimisationTool")
            kwargs["ErrorOptimisationTool"] = tool
        
    builder = CfgMgr.Muon__MooTrackBuilder(name,**kwargs)

    # make clones of some tools if namePrefix (e.g. for TrigMuonEF) or namePostfix (e.g. for FinalFit) is given
    if namePrefix or namePostfix:
        oldFitterName = getProperty(builder,"Fitter").getName()
        newFitterName = namePrefix+oldFitterName+namePostfix
        builder.Fitter = getPublicToolClone(newFitterName,oldFitterName,extraFlags=extraFlags)
        oldFitterName = getProperty(builder,"SLFitter").getName()
        newFitterName = namePrefix+oldFitterName+namePostfix
        builder.SLFitter = getPublicToolClone(newFitterName,oldFitterName,extraFlags=extraFlags)
        oldMatchingToolName = getProperty(builder,"CandidateMatchingTool").getName()
        newMatchingToolName = namePrefix+oldMatchingToolName+namePostfix
        builder.CandidateMatchingTool = getPublicToolClone(newMatchingToolName,oldMatchingToolName,extraFlags=extraFlags)

    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)
    
    return builder

# end of factory function MooTrackBuilder




#
# Tools for Moore track building
#
class MuonSegmentSelectionTool(CfgMgr.Muon__MuonSegmentSelectionTool,ConfiguredBase):
    __slots__ = ()
    def __init__(self,name="MuonSegmentSelectionTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        if globalflags.DataSource() == 'data':  #collisions-data or simulation first data
            kwargs.setdefault("GoodADCFractionCut",  0.5 )
            kwargs.setdefault("MinADCPerSegmentCut", 100 )
        super(MuonSegmentSelectionTool,self).__init__(name,**kwargs)
    

class MuonSegmentMatchingTool(CfgMgr.Muon__MuonSegmentMatchingTool):
    __slots__ = ()

    def __init__(self,name='MuonSegmentMatchingTool',**kwargs):
        kwargs.setdefault( "doThetaMatching", muonStandaloneFlags.useSegmentMatching() )
        kwargs.setdefault( "doPhiMatching", False )
        kwargs.setdefault( "ToroidOn", jobproperties.BField.allToroidOn() )
        if beamFlags.beamType() == 'cosmics':
            # switch off cut of phi hit pulls
            kwargs.setdefault("OverlapMatchAveragePhiHitPullCut", 200. )

        super(MuonSegmentMatchingTool,self).__init__(name,**kwargs)
        # add missing dependency in C++
        getPublicTool("ResidualPullCalculator")

# end of class MuonSegmentMatchingTool
    

def MCTBExtrapolator(name='MCTBExtrapolator',**kwargs):
    kwargs.setdefault("Propagators", ["MCTBPropagator"])
    kwargs.setdefault("ResolveMuonStation", False)
    return MuonExtrapolator(name,**kwargs)
# end of factory function MCTBExtrapolator


def MCTBFitter(name="MCTBFitter", **kwargs):
    kwargs.setdefault("ExtrapolationTool", "MCTBExtrapolator")
    kwargs.setdefault("GetMaterialFromTrack", True)
    return MuonChi2TrackFitter(name,**kwargs)
# end of MCTBFitter()

def MCTBSLFitter(name="MCTBSLFitter", **kwargs):
    kwargs["StraightLine"] = True # always set
    kwargs.setdefault("Momentum", muonStandaloneFlags.straightLineFitMomentum()) #only set if not yet set
    return MCTBFitter(name, **kwargs)

def MCTBSLFitterMaterialFromTrack(name="MCTBSLFitterMaterialFromTrack", **kwargs):
    kwargs["GetMaterialFromTrack"]=True
    kwargs.setdefault("ExtrapolationTool", "Trk::Extrapolator/MuonStraightLineExtrapolator")
    #kwargs.setdefault("PropagatorTool", "Trk::STEP_Propagator/MuonStraightLinePropagator") #will switch to this once it's ready
    kwargs["PropagatorTool"]=getPublicTool("MuonRK_Propagator")
    return MCTBSLFitter(name, **kwargs)

def MuonSeededSegmentFinder(name="MuonSeededSegmentFinder",**kwargs):
    
    if "SegmentMaker" not in kwargs or "SegmentMakerNoHoles" not in kwargs:
        if beamFlags.beamType() == 'collisions':
          
            segMaker = getPublicToolClone("MCTBDCMathSegmentMaker", "DCMathSegmentMaker", 
                                          MdtSegmentFinder = "MCTBMdtMathSegmentFinder",
                                          SinAngleCut = 0.04, DoGeometry = True )
        else: # cosmics or singlebeam
            segMaker = getPublicToolClone("MCTBDCMathSegmentMaker", "DCMathSegmentMaker", 
                                          MdtSegmentFinder = "MCTBMdtMathSegmentFinder",
                                          SinAngleCut = 0.1, DoGeometry = False, AddUnassociatedPhiHits= True )

        kwargs.setdefault("SegmentMaker", segMaker)
        kwargs.setdefault("SegmentMakerNoHoles", segMaker)

        if not MuonGeometryFlags.hasCSC(): kwargs.setdefault("CscPrepDataContainer","")
        if not MuonGeometryFlags.hasSTGC(): kwargs.setdefault("sTgcPrepDataContainer","")
        if not MuonGeometryFlags.hasMM(): kwargs.setdefault("MMPrepDataContainer","")
    
    return CfgMgr.Muon__MuonSeededSegmentFinder(name,**kwargs)

# end of factory function MuonSeededSegmentFinder


def MuonRefitTool(name,**kwargs):
    # To activate the tuning of meas. errors using alignment constants from DB
    # kwargs.setdefault("AlignmentErrorTool", getPublicTool("MuonAlignmentErrorTool"))
    # kwargs.setdefault("DeweightBEE", False)
    # kwargs.setdefault("DeweightEE", False)
    if conddb.dbdata == 'COMP200' or conddb.dbmc == 'COMP200' or 'HLT' in globalflags.ConditionsTag() or conddb.isOnline :
        kwargs["AlignmentErrorTool"] = None
    return CfgMgr.Muon__MuonRefitTool(name,**kwargs)


def MuonErrorOptimisationTool(name,extraFlags=None,**kwargs):
    namePrefix =getattr(extraFlags,"namePrefix","")
    namePostfix=getattr(extraFlags,"namePostfix","")
    cloneArgs = {}
    fitter=getattr(extraFlags,"Fitter",None)
    if fitter is not None:
        cloneArgs["Fitter"] = fitter
    if "RefitTool" not in kwargs:
        if namePrefix or namePostfix:
            cloneName = namePrefix+"MuonRefitTool"+namePostfix
            kwargs["RefitTool"] = getPublicToolClone(cloneName, "MuonRefitTool", **cloneArgs)

    return CfgMgr.Muon__MuonErrorOptimisationTool(name,**kwargs)

def MuonTrackCleaner(name,extraFlags=None,**kwargs):
  kwargs.setdefault("Chi2Cut", muonStandaloneFlags.Chi2NDofCut())
  kwargs.setdefault("MaxAvePullSumPerChamber", 6)
  kwargs.setdefault("Fitter",        getPrivateTool('MCTBFitterMaterialFromTrack') )
  kwargs.setdefault("SLFitter",      getPrivateTool('MCTBSLFitterMaterialFromTrack'))
  kwargs.setdefault("MdtRotCreator", getPrivateTool('MdtDriftCircleOnTrackCreator'))
  # kwargs.setdefault("CompRotCreator", getPrivateTool('TriggerChamberClusterOnTrackCreator')) Not in DB
  
  return CfgMgr.Muon__MuonTrackCleaner(name,**kwargs)

# class MuonTrackCleaner(CfgMgr.Muon__MuonTrackCleaner,ConfiguredBase):
#     __slots__ = ()
#
#     def __init__(self,name="MuonTrackCleaner",**kwargs):
#         self.applyUserDefaults(kwargs,name)
#         super(MuonTrackCleaner,self).__init__(name,**kwargs)
#
#         getPublicTool("ResidualPullCalculator")
#
# MuonTrackCleaner.setDefaultProperties( Chi2Cut = muonStandaloneFlags.Chi2NDofCut(),
#                                        MaxAvePullSumPerChamber = 6 )
# end of class MuonTrackCleaner


def MuonChamberHoleRecoveryTool(name="MuonChamberHoleRecoveryTool",extraFlags=None,**kwargs):
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit", muonRecFlags.doSegmentT0Fit())

    kwargs.setdefault("Extrapolator", "MuonExtrapolator")

    if doSegmentT0Fit:
        kwargs.setdefault("AddMeasurements", False)

    if muonRecFlags.doCSCs():
        if muonRecFlags.enableErrorTuning() or globalflags.DataSource() == 'data':
            kwargs.setdefault("CscRotCreator","CscBroadClusterOnTrackCreator")
        else:
            kwargs.setdefault("CscRotCreator","CscClusterOnTrackCreator")
    else: # no CSCs
        # switch off whatever is set
        kwargs["CscRotCreator"] = ""
        kwargs["CscPrepDataContainer"] = ""

    # add in missing C++ dependency. TODO: fix in C++
    getPublicTool("ResidualPullCalculator")

    if not MuonGeometryFlags.hasSTGC(): kwargs.setdefault("sTgcPrepDataContainer","")
    if not MuonGeometryFlags.hasMM(): kwargs.setdefault("MMPrepDataContainer","")

    #MDT conditions information not available online
    if(athenaCommonFlags.isOnline):
        kwargs.setdefault("MdtCondKey","")

    return CfgMgr.Muon__MuonChamberHoleRecoveryTool(name,**kwargs)
# end of factory function MuonChamberHoleRecoveryTool

def MuonTrackToSegmentTool(name="MuonTrackToSegmentTool",extraFlags=None,**kwargs):
    #MDT conditions information not available online
    if(athenaCommonFlags.isOnline):
        kwargs.setdefault("MdtCondKey","")
    return CfgMgr.Muon__MuonTrackToSegmentTool(name,**kwargs)

class MuonSegmentRegionRecoveryTool(CfgMgr.Muon__MuonSegmentRegionRecoveryTool,ConfiguredBase):
  __slots__ = ()

  def __init__(self,name="MuonSegmentRegionRecoveryTool",**kwargs):
      #MDT conditions information not available online
      if(athenaCommonFlags.isOnline):
          kwargs.setdefault("MdtCondKey","")
      from RegionSelector.RegSelToolConfig import makeRegSelTool_MDT, makeRegSelTool_RPC, makeRegSelTool_TGC
      kwargs.setdefault("MDTRegionSelector", makeRegSelTool_MDT())
      kwargs.setdefault("RPCRegionSelector", makeRegSelTool_RPC())
      kwargs.setdefault("TGCRegionSelector", makeRegSelTool_TGC())
      if MuonGeometryFlags.hasCSC():
          from RegionSelector.RegSelToolConfig import makeRegSelTool_CSC
          kwargs.setdefault("CSCRegionSelector", makeRegSelTool_CSC())
      else:
          kwargs.setdefault("CSCRegionSelector", "")
      if MuonGeometryFlags.hasSTGC():
          from RegionSelector.RegSelToolConfig import makeRegSelTool_sTGC
          kwargs.setdefault("STGCRegionSelector", makeRegSelTool_sTGC())
      else:
          kwargs.setdefault("STGCRegionSelector", "")
      if MuonGeometryFlags.hasMM():
          from RegionSelector.RegSelToolConfig import makeRegSelTool_MM
          kwargs.setdefault("MMRegionSelector", makeRegSelTool_MM())
      else:
          kwargs.setdefault("MMRegionSelector", "")

      self.applyUserDefaults(kwargs,name)
      super(MuonSegmentRegionRecoveryTool,self).__init__(name,**kwargs)



MuonSegmentRegionRecoveryTool.setDefaultProperties (
    Fitter = "MCTBFitter",
    TrackSegmentMatchingTool = "MooCandidateMatchingTool"
    )
# end of class MuonSegmentRegionRecoveryTool

class MuonTrackScoringTool(CfgMgr.Muon__MuonTrackScoringTool,ConfiguredBase):
    __slots__ = ()
    
    def __init__(self,name="MuonTrackScoringTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonTrackScoringTool,self).__init__(name,**kwargs)

MuonTrackScoringTool.setDefaultProperties( SumHelpTool = "MuonTrackSummaryTool" )
# end of class MuonTrackScoringTool

class MuonAmbiProcessor(CfgMgr.Trk__TrackSelectionProcessorTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonAmbiProcessor",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonAmbiProcessor,self).__init__(name,**kwargs)

MuonAmbiProcessor.setDefaultProperties(
    DropDouble         = False ,
    ScoringTool        = "MuonTrackScoringTool" ,
    SelectionTool      = "MuonAmbiSelectionTool" )
# end of class MuonAmbiProcessor

class MuonTrackSelectorTool(CfgMgr.Muon__MuonTrackSelectorTool,ConfiguredBase):
    __slots__ = ()
 
    def __init__(self,name="MuonTrackSelectorTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonTrackSelectorTool,self).__init__(name,**kwargs)

if beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' or globalflags.DataSource() == 'data' :
    MuonTrackSelectorTool.setDefaultProperties( UseRPCHoles = False,
                                                UseTGCHoles = False )

if beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' or globalflags.DataSource() == 'data':
    MuonTrackSelectorTool.setDefaultProperties(
        MaxMdtHolesOnTwoStationTrack = 10,
        MaxMdtHolesOnTrack = 30,
        HolesToHitsRatioCutPerStation = 2.,
        CountMDTOutlierAsHoles = False )
else: # collisions simulation
    MuonTrackSelectorTool.setDefaultProperties(
        MaxMdtHolesOnTwoStationTrack = 4,
        MaxMdtHolesOnTrack = 5,
        CountMDTOutlierAsHoles = True )

class MuonTrackExtrapolationTool(CfgMgr.Muon__MuonTrackExtrapolationTool,ConfiguredBase):
    __slots__ = ()
    
    def __init__(self,name="MuonTrackExtrapolationTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonTrackExtrapolationTool,self).__init__(name,**kwargs)

MuonTrackExtrapolationTool.setDefaultProperties( TrackingGeometrySvc=ServiceMgr.AtlasTrackingGeometrySvc )
if beamFlags.beamType() == 'cosmics':
    MuonTrackExtrapolationTool.setDefaultProperties( Cosmics = True )

# end of class MuonTrackExtrapolationTool


#
# Until all dependencies are explicit, instantiate some tools here
#
getPublicTool("MCTBFitter")
getPublicTool("MCTBSLFitter")
getPublicTool("MCTBFitterMaterialFromTrack")
getPublicTool("MuonSeededSegmentFinder")
mCHRT = getPublicTool("MuonChamberHoleRecoveryTool")
if not MuonGeometryFlags.hasCSC():
    mCHRT.CscRotCreator = ""
    mCHRT.CscPrepDataContainer = ""
getPublicTool("MuonTrackSelectorTool")
getPublicTool("MuonTrackExtrapolationTool")
getPublicTool("MuonTrackCleaner")
getPublicTool("FixedErrorMuonClusterOnTrackCreator")
getPublicTool("MuonSegmentSelectionTool")

