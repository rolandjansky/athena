# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

from RecExConfig.RecFlags import rec

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

from AthenaCommon.ConfiguredFactory import getProperty

from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber

from MuonRecTools import MuonExtrapolator, MuonChi2TrackFitter, MdtDriftCircleOnTrackCreator
from MuonRecUtils import logMuon,ConfiguredBase,ExtraFlags


from MooreFlags import mooreFlags
from MuonRecFlags import muonRecFlags
from MuonStandaloneFlags import muonStandaloneFlags
#==============================================================

# call  setDefaults to update flags
muonRecFlags.setDefaults()
mooreFlags.setDefaults()

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
                print "CONFIGMuonPatternSegmentMaker: doT0fit?!?!"
            else:
                mdtCreator = getPublicToolClone( "MdtDriftCircleOnTrackCreatorSegmentFinding", "MdtDriftCircleOnTrackCreator", 
                                                 CreateTubeHit = False, TimeWindowSetting = mdtCalibWindowNumber('Collision_data') )
            kwargs["MdtCreator"] = mdtCreator

    if beamType == 'cosmics' or muonRecFlags.forceCollisionsMode():
        kwargs.setdefault("AngleCutPhi", 1e9)
        kwargs.setdefault("DropDistance", 100000000.)

    return CfgMgr.Muon__MuonPatternSegmentMaker(name,**kwargs)
# end of factory function MuonPatternSegmentMaker


class MuonCurvedSegmentCombiner(CfgMgr.Muon__MuonCurvedSegmentCombiner,ConfiguredBase):
    __slots__ = ()
    def __init__(self,name="MuonCurvedSegmentCombiner",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonCurvedSegmentCombiner,self).__init__(name,**kwargs)

MuonCurvedSegmentCombiner.setDefaultProperties(
    MissedHitsCut = 4,
    AddAll2DCscs = False,
    UseCscSegments = muonRecFlags.doCSCs())
if (beamFlags.beamType() == 'singlebeam' or beamFlags.beamType() == 'cosmics') and not muonRecFlags.forceCollisionsMode():
    MuonCurvedSegmentCombiner.setDefaultProperties(
        DoCosmics = True,
        MissedHitsCut = 100)
elif globalflags.DataSource() == 'data' or muonRecFlags.forceDataMode():  #collisions-data or simulation first data
    MuonCurvedSegmentCombiner.setDefaultProperties(
        DoCosmics = False,
        MissedHitsCut = 100)
        

# end of class MuonCurvedSegmentCombiner

if mooreFlags.addEndcapMiddleSingleStationTracks():
    MuonCurvedSegmentCombiner.setDefaultProperties(
        AddUnassociatedMiddleEndcapSegments = True )
    

# the segment making supertool
class MooSegmentCombinationFinder(CfgMgr.Muon__MooSegmentCombinationFinder,ConfiguredBase):
    __slots__ = ()
    
    def __init__(self,name="MooSegmentFinder",**kwargs):

        kwargs.setdefault( "SegmentCombiner", "MuonCurvedSegmentCombiner" )
        kwargs.setdefault( "SegmentCombinationCleaner", "MuonSegmentCombinationCleanerTool" )
        if( beamFlags.beamType() != 'cosmics' and beamFlags.beamType() != 'singlebeam'): 
            kwargs.setdefault( "HoughPatternFinder", "MuonLayerHoughTool" )
        else:
            kwargs.setdefault( "HoughPatternFinder", "MuonHoughPatternFinderTool" )

        kwargs.setdefault( "MdtSegmentMaker", "MuonPatternSegmentMaker" )
        kwargs.setdefault( "DoSegmentCombinationCleaning", True )
        kwargs.setdefault( "DoCscSegments", muonRecFlags.doCSCs() )
        kwargs.setdefault( "DoMdtSegments", muonRecFlags.doMDTs() )
        if muonRecFlags.doCSCs():
            kwargs.setdefault( "Csc2dSegmentMaker","Csc2dSegmentMaker" )
            kwargs.setdefault( "Csc4dSegmentMaker", "Csc4dSegmentMaker" )
        else:
            kwargs.setdefault( "Csc2dSegmentMaker", None )
            kwargs.setdefault( "Csc4dSegmentMaker", None )
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
    useTrackSegmentMatching = getattr(extraFlags,"useTrackSegmentMatching",mooreFlags.useTrackSegmentMatching())
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

    if (beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam') and not muonRecFlags.forceCollisionsMode():
        kwargs.setdefault("RequireSameSide", True)

    if globalflags.DataSource() == 'data' or muonRecFlags.forceDataMode():
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

    return CfgMgr.Muon__MooCandidateMatchingTool(name,**kwargs)



# track fitter tools
def MooTrackFitter(name="MooTrackFitter", extraFlags=None, **kwargs):

    namePrefix =getattr(extraFlags,"namePrefix","")
    namePostfix=getattr(extraFlags,"namePostfix","")
    materialSource=getattr(extraFlags,"materialSource",mooreFlags.materialSourcePatRec())

    kwargs.setdefault("Fitter",          "MCTBFitter")
    kwargs.setdefault("Propagator",      "MuonPropagator")
    kwargs.setdefault("SLFit" ,          not jobproperties.BField.allToroidOn())
    kwargs.setdefault("ReducedChi2Cut",  mooreFlags.Chi2NDofCut())
    kwargs.setdefault("FitEtaStrips",    True)
    kwargs.setdefault("SegmentMomentum", "MuonSegmentMomentumFromField")
    kwargs.setdefault("CleanPhiHits",              True)
    kwargs.setdefault("UsePreciseHits",            True)
    kwargs.setdefault("UsePrefit",                 False)
    kwargs.setdefault("SeedAtStartOfTrack",        False)

    if (beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam') and not muonRecFlags.forceCollisionsMode() :
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
        fitter.Fitter = getPublicToolClone(newFitterName,oldFitterName,materialSource=materialSource)

        oldFitterName = getProperty(fitter,"FitterPreFit").getName()
        newFitterName = namePrefix + oldFitterName + namePostfix
        fitter.FitterPreFit = getPublicToolClone(newFitterName,oldFitterName,materialSource=materialSource)
 
    return fitter 
    
# end of factory function MooTrackFitter



def MooTrackBuilder(name="MooTrackBuilderTemplate",
                    extraFlags=None,
                    **kwargs):

    namePrefix =getattr(extraFlags,"namePrefix","")
    namePostfix=getattr(extraFlags,"namePostfix","")
    optimiseMomentumResolutionUsingChi2=getattr(extraFlags,"optimiseMomentumResolutionUsingChi2",mooreFlags.optimiseMomentumResolutionUsingChi2())
    
    kwargs.setdefault("Fitter",   "MooTrackFitter")
    kwargs.setdefault("SLFitter", "MooSLTrackFitter")

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
    
    return builder

# end of factory function MooTrackBuilder


def MooTrackSteering(name="MooTrackSteering",
                     extraFlags=None,
                     **kwargs):
    
    # take defaults for extra args from MooreFlags
    namePrefix     = getattr(extraFlags,"namePrefix", "")
    beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit", muonRecFlags.doSegmentT0Fit())

    optimiseMomentumResolutionUsingChi2 = getattr(extraFlags,"optimiseMomentumResolutionUsingChi2",
                                                  mooreFlags.optimiseMomentumResolutionUsingChi2() or doSegmentT0Fit)
    materialSourcePatRec   = getattr(extraFlags,"materialSourcePatRec",   mooreFlags.materialSourcePatRec())
    materialSourceFinalFit = getattr(extraFlags,"materialSourceFinalFit", mooreFlags.materialSourceFinalFit())

    kwargs.setdefault("AmbiguityTool",          "MuonAmbiProcessor")
    kwargs.setdefault("EntryMatchingTool",      "MooCandidateMatchingTool")
    kwargs.setdefault("Propagator",             "MCTBPropagator")
    kwargs.setdefault("FindingMode",            2)
    kwargs.setdefault("UseAllPhiHits",          True)
    kwargs.setdefault("ExtrapolateToMuonEntry", True)
    kwargs.setdefault("ResolveAmbiguities",     True)
    kwargs.setdefault("CleanUpSeeds",           True)
    kwargs.setdefault("CleanUpCandidates",      True)
    kwargs.setdefault("HoleRecoveryTool",       "MuonEORecoveryTool")

    if mooreFlags.addEndcapMiddleSingleStationTracks():
        kwargs.setdefault("AddUnassociatedMiddleEndcapSegments", True)

    if mooreFlags.printSummary():
        kwargs.setdefault("DoSummary", True)

    if beamType == 'collisions' or muonRecFlags.forceCollisionsMode() :
        kwargs.setdefault("TrackExtrapolationTool",None)
    else: # cosmics or single beam
        kwargs.setdefault("TrackExtrapolationTool",             "MuonTrackExtrapolationTool")
        kwargs.setdefault("MaximumNumberOfAmbiguitiesPerCombi", 500)
        kwargs.setdefault("MaximumNumberOfCombisPerEvent",      5)
        kwargs.setdefault("MaximumNumberOfSeedsPerCombi",       10)
        kwargs.setdefault("MaximumNumberOfCandidatesPerSeed",   30)
        kwargs.setdefault("RetryAllChamberLayers",              True)
        kwargs.setdefault("SplitTracksCrossingCalo",            True)

    if doSegmentT0Fit:
        kwargs.setdefault("RecalibrateMDTHits", False)

    if extraFlags is not None:
        fitterFlags = copy.deepcopy(extraFlags)
    else:
        fitterFlags = ExtraFlags()
    # use separate fitter if material different
    if materialSourcePatRec != materialSourceFinalFit:
        if "TrackBuilder" not in kwargs:
            fitterFlags.materialSource = materialSourcePatRec
            fitter = getPublicToolClone(namePrefix+"MooTrackBuilder", "MooTrackBuilderTemplate",
                                        extraFlags=fitterFlags)
            kwargs["TrackBuilder"] = fitter

        if "TrackBuilderFinalFit" not in kwargs:
            fitterFlags.materialSource = materialSourceFinalFit
            fitterFlags.namePostfix="FinalFit"
            fitter = getPublicToolClone(namePrefix+"MooTrackBuilderFinalFit", "MooTrackBuilderTemplate",
                                        extraFlags=fitterFlags)

            kwargs["TrackBuilderFinalFit"] = fitter

    else: # same materialSource
        if "TrackBuilder" not in kwargs or "TrackBuilderFinalFit" not in kwargs:
            fitterFlags.materialSource = materialSourcePatRec
            fitter = getPublicToolClone(namePrefix+"MooTrackBuilder", "MooTrackBuilderTemplate",
                                        extraFlags=fitterFlags)
            
            kwargs.setdefault("TrackBuilder",         fitter)
            kwargs.setdefault("TrackBuilderFinalFit", fitter)

    return CfgMgr.Muon__MooTrackSteering(name,**kwargs)

# end of factory function MooTrackSteering


## Moore-only configuration version of MuonStandalone.MuonTrackSteering
def MooreTrackSteering(name="MooreTrackSteering", extraFlags=None, **kwargs):
    if extraFlags is None:
        extraFlags = ExtraFlags()
        
    extraFlags.setFlagDefault("doSegmentPhiMatching", True)
    extraFlags.setFlagDefault("materialSource",mooreFlags.materialSourcePatRec)
    extraFlags.setFlagDefault(mooreFlags.optimiseMomentumResolutionUsingChi2)
    extraFlags.setFlagDefault(mooreFlags.printSummary)
    extraFlags.setFlagDefault(mooreFlags.strategy)
                  
    kwargs.setdefault("StrategyList", extraFlags.strategy)  
    kwargs.setdefault("DoSummary", extraFlags.printSummary)

    kwargs.setdefault("OutputSingleStationTracks", True)

    if "TrackBuilderTool" not in kwargs:
        extraFlags.setFlagDefault('UseTrackingHistory',True)
        kwargs["TrackBuilderTool"] = getPublicToolClone("MooreTrackBuilder", "MooTrackBuilderTemplate",
                                                        extraFlags=extraFlags)
        if "TrackRefinementTool" not in kwargs:
            kwargs["TrackRefinementTool"] = getPublicToolClone("MooreTrackRefiner","MooTrackBuilderTemplate")


    kwargs.setdefault("SegSeedQCut", 2)
    kwargs.setdefault("Seg2ndQCut", 1)

    return CfgMgr.Muon__MuonTrackSteering(name,**kwargs)





#
# End of Moore track finding tools
#



#
# Tools for Moore track building
#
class MuonSegmentSelectionTool(CfgMgr.Muon__MuonSegmentSelectionTool,ConfiguredBase):
    __slots__ = ()
    def __init__(self,name="MuonSegmentSelectionTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonSegmentSelectionTool,self).__init__(name,**kwargs)

if globalflags.DataSource() == 'data':  #collisions-data or simulation first data
    MuonSegmentSelectionTool.setDefaultProperties( GoodADCFractionCut = 0.5, MinADCPerSegmentCut=100 )
    

class MuonSegmentMatchingTool(CfgMgr.Muon__MuonSegmentMatchingTool):
    __slots__ = ()

    def __init__(self,name='MuonSegmentMatchingTool',**kwargs):
        kwargs.setdefault( "doThetaMatching", mooreFlags.useSegmentMatching() )
        kwargs.setdefault( "doPhiMatching", False )
        if beamFlags.beamType() == 'cosmics' and not muonRecFlags.forceCollisionsMode():
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


def MCTBFitter(name="MCTBFitter", materialSource=None, **kwargs):
    if materialSource is None: materialSource = mooreFlags.materialSourcePatRec()

    kwargs.setdefault("ExtrapolationTool", "MCTBExtrapolator")
    kwargs.setdefault("GetMaterialFromTrack", True)
    
    # setup track fitters to be used during track finding
    if( materialSource == "MBoy" ):
        #kwargs.setdefault("MboyMat", True)
        #kwargs.setdefault("ExtrapolatorMaterial", False)
        raise RuntimeError("Muonboy material no longer supported")

    elif( materialSource == "TGMat" ):
        #kwargs.setdefault("MboyMat", False)
        #kwargs.setdefault("ExtrapolatorMaterial", True)
        pass

    elif( materialSource == "TGBlended" ):
        #kwargs.setdefault("MboyMat", False)
        #kwargs.setdefault("ExtrapolatorMaterial", True)
        kwargs.setdefault("ExtrapolationTool", "MCTBExtrapolatorBlendedMat")

    else:
        raise RuntimeError("Unknown materialSource %r for MCTBFitter(%r)" % (materialSource,name) )

    return MuonChi2TrackFitter(name,**kwargs)
# end of MCTBFitter()

def MCTBSLFitter(name="MCTBFitter", materialSource=None, **kwargs):
    kwargs["StraightLine"] = True # always set
    kwargs.setdefault("Momentum", mooreFlags.straightLineFitMomentum()) #only set if not yet set
    return MCTBFitter(name, materialSource=materialSource, **kwargs)


def MuonSeededSegmentFinder(name="MuonSeededSegmentFinder",**kwargs):
    if "SegmentMaker" not in kwargs or "SegmentMakerNoHoles" not in kwargs:
        if beamFlags.beamType() == 'collisions' or muonRecFlags.forceCollisionsMode():
            segMaker = getPublicToolClone("MCTBDCMathSegmentMaker", "DCMathSegmentMaker", 
                                          MdtSegmentFinder = "MCTBMdtMathSegmentFinder",
                                          SinAngleCut = 0.04, DoGeometry = True )
        else: # cosmics or singlebeam
            segMaker = getPublicToolClone("MCTBDCMathSegmentMaker", "DCMathSegmentMaker", 
                                          MdtSegmentFinder = "MCTBMdtMathSegmentFinder",
                                          SinAngleCut = 0.1, DoGeometry = False, AddUnassociatedPhiHits= True )

        kwargs.setdefault("SegmentMaker", segMaker)
        kwargs.setdefault("SegmentMakerNoHoles", segMaker)

    return CfgMgr.Muon__MuonSeededSegmentFinder(name,**kwargs)

# end of factory function MuonSeededSegmentFinder


def MuonRefitTool(name,**kwargs):
    if not muonRecFlags.doCSCs():
        kwargs["CscRotCreator"] = None	   
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


class MuonTrackCleaner(CfgMgr.Muon__MuonTrackCleaner,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonTrackCleaner",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonTrackCleaner,self).__init__(name,**kwargs)
        
        getPublicTool("ResidualPullCalculator")

MuonTrackCleaner.setDefaultProperties( Chi2Cut = mooreFlags.Chi2NDofCut(),
                                       MaxAvePullSumPerChamber = 6 )
# end of class MuonTrackCleaner


def MuonChamberHoleRecoveryTool(name="MuonChamberHoleRecoveryTool",extraFlags=None,**kwargs):
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit", muonRecFlags.doSegmentT0Fit())

    kwargs.setdefault("Extrapolator", "MuonExtrapolator")

    if doSegmentT0Fit:
        kwargs.setdefault("AddMeasurements", False)

    if muonRecFlags.doCSCs:
        if muonRecFlags.enableErrorTuning() or globalflags.DataSource() == 'data':
            kwargs.setdefault("CscRotCreator","CscBroadClusterOnTrackCreator")
        else:
            kwargs.setdefault("CscRotCreator","CscClusterOnTrackCreator")
    else: # no CSCs
        # switch off whatever is set
        kwargs["CscRotCreator"] = None

    # add in missing C++ dependency. TODO: fix in C++
    getPublicTool("ResidualPullCalculator")

    return CfgMgr.Muon__MuonChamberHoleRecoveryTool(name,**kwargs)
# end of factory function MuonChamberHoleRecoveryTool


class MuonSegmentRegionRecoveryTool(CfgMgr.Muon__MuonSegmentRegionRecoveryTool,ConfiguredBase):
  __slots__ = ()

  def __init__(self,name="MuonSegmentRegionRecoveryTool",**kwargs):
     self.applyUserDefaults(kwargs,name)
     super(MuonSegmentRegionRecoveryTool,self).__init__(name,**kwargs)
     global ServiceMgr
     from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
     SegRecoveryRegSelSvc = RegSelSvcDefault()
     SegRecoveryRegSelSvc.enableMuon = True
     ServiceMgr += SegRecoveryRegSelSvc
#     self.RegionSelector = SegRecoveryRegSelSvc

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
# end fo class MuonAmbiProcessor


class MuonTrackSelectorTool(CfgMgr.Muon__MuonTrackSelectorTool,ConfiguredBase):
    __slots__ = ()
 
    def __init__(self,name="MuonTrackSelectorTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonTrackSelectorTool,self).__init__(name,**kwargs)

if beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' or globalflags.DataSource() == 'data' \
       or muonRecFlags.forceDataMode():
    MuonTrackSelectorTool.setDefaultProperties( UseRPCHoles = False,
                                                UseTGCHoles = False )

if beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' or globalflags.DataSource() == 'data' \
       or muonRecFlags.forceDataMode():
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
if beamFlags.beamType() == 'cosmics' and not muonRecFlags.forceCollisionsMode():
    MuonTrackExtrapolationTool.setDefaultProperties( Cosmics = True )

# end of class MuonTrackExtrapolationTool


#
# Until all dependencies are explicit, instantiate some tools here
#
getPublicTool("MCTBFitter")
getPublicTool("MCTBSLFitter")
getPublicTool("MCTBFitterMaterialFromTrack")
getPublicTool("MCTBSLFitterMaterialFromTrack")
getPublicTool("MuonSeededSegmentFinder")
getPublicTool("MuonChamberHoleRecoveryTool")
getPublicTool("MuonTrackSelectorTool")
getPublicTool("MuonTrackExtrapolationTool")
getPublicTool("MuonSegmentRegionRecoveryTool")
getPublicTool("MuonTrackCleaner")
getPublicTool("FixedErrorMuonClusterOnTrackCreator")
getPublicTool("MuonSegmentSelectionTool")

