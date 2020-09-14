# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber
from MuonRecExample.MuonRecUtils import logMuon,ConfiguredBase,uglyHackedInclude,ExtraFlags

from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.setDefaults()

from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags
muonStandaloneFlags.setDefaults()

from RecExConfig.RecFlags import rec


from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
     getService,getServiceClone,getAlgorithm,getAlgorithmClone

# temporarily for backwards compat. TO BE REMOVED
from AthenaCommon.CfgGetter import addTool,addToolClone,addService

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from TriggerJobOpts.TriggerFlags import TriggerFlags

#--------------------------------------------------------------------------------
# Hit-on-track creation tools
#--------------------------------------------------------------------------------
# set up default ROT creator
def MuonClusterOnTrackCreator(name="MuonClusterOnTrackCreator",**kwargs):
    if globalflags.DataSource() == 'data': # collisions real data or simulated first data
        # scale TGC eta hit errors as long as TGC eta are not well aligned
        kwargs.setdefault("DoFixedErrorTgcEta", True)
        kwargs.setdefault("FixedErrorTgcEta", 15.)

    return CfgMgr.Muon__MuonClusterOnTrackCreator(name,**kwargs)

def MMClusterOnTrackCreator(name="MMClusterOnTrackCreator",**kwargs):
    return CfgMgr.Muon__MMClusterOnTrackCreator(name,**kwargs)

def getMuonRIO_OnTrackErrorScalingCondAlg() :
    error_scaling_def=["CSCRIO_OnTrackErrorScaling:/MUON/TrkErrorScalingCSC"]
    return getRIO_OnTrackErrorScalingCondAlg( name                = "MuonRIO_OnTrackErrorScalingCondAlg",
                                              ReadKey             = "/MUON/TrkErrorScaling",
                                              CondDataAssociation = error_scaling_def)

def CscClusterOnTrackCreator(name="CscClusterOnTrackCreator",**kwargs):
    kwargs.setdefault("CscStripFitter", getPrivateTool("CalibCscStripFitter") )
    kwargs.setdefault("CscClusterFitter", getPrivateTool("QratCscClusterFitter") )
    kwargs.setdefault("CscClusterUtilTool", getPrivateTool("CscClusterUtilTool") )
    if False  : # enable CscClusterOnTrack error scaling :
        from InDetRecExample.TrackingCommon import getRIO_OnTrackErrorScalingCondAlg,createAndAddCondAlg
        createAndAddCondAlg(getMuonRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')

        kwargs.setdefault("CSCErrorScalingKey","/MUON/TrkErrorScalingCSC")

    if globalflags.DataSource() == 'data': # collisions real data or simulated first data
        # scale CSC and hit errors
        kwargs.setdefault("ErrorScalerBeta", 0.070 )

    return CfgMgr.Muon__CscClusterOnTrackCreator(name,**kwargs)

def CscBroadClusterOnTrackCreator(name="CscBroadClusterOnTrackCreator",**kwargs):
    kwargs["ErrorScalerBeta"] = 0.200
    return CscClusterOnTrackCreator(name,**kwargs)


def MdtDriftCircleOnTrackCreator(name="MdtDriftCircleOnTrackCreator",**kwargs):
    # setup dependencies missing in C++. TODO: fix in C++
    from MuonRecExample import MuonAlignConfig
    from MuonCnvExample import MuonCalibConfig
    MuonCalibConfig.setupMdtCondDB()
    from MuonCnvExample.MuonCalibFlags import mdtCalibFlags
    mdtCalibFlags.setDefaults()
    kwargs.setdefault("DoMagneticFieldCorrection", mdtCalibFlags.correctMdtRtForBField())
    kwargs.setdefault("DoWireSag", muonRecFlags.useWireSagCorrections())
    kwargs.setdefault("DoSlewingCorrection", mdtCalibFlags.correctMdtRtForTimeSlewing())

    if beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' :
        kwargs.setdefault("DoTofCorrection", False)
        kwargs.setdefault("DoFixedError", True)
        kwargs.setdefault("TimingMode", 1)
        kwargs.setdefault("UseParametrisedError", True)

    else: # collisions simulation/data settings
        kwargs.setdefault("DoTofCorrection", True)
        kwargs.setdefault("DoFixedError", False)
        kwargs.setdefault("DoErrorScaling", False)
        kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))  # MJW: should this be Collision_G4 ???
        kwargs.setdefault("UseParametrisedError", False)

        if globalflags.DataSource() == 'data': # collisions real data or simulated first data
            kwargs.setdefault("CreateTubeHit", True)  # BroadErrors
            kwargs.setdefault("UseLooseErrors", muonRecFlags.useLooseErrorTuning())  # LooseErrors on data

    if globalflags.DataSource() == 'data':
        kwargs.setdefault("IsMC", False)
    else:
        kwargs.setdefault("IsMC", True)

    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("doMDT", True)

    return CfgMgr.Muon__MdtDriftCircleOnTrackCreator(name,**kwargs)
# end of factory function MdtDriftCircleOnTrackCreator


def MdtTubeHitOnTrackCreator(name="MdtTubeHitOnTrackCreator",**kwargs):
    kwargs["CreateTubeHit"] = True
    return MdtDriftCircleOnTrackCreator(name,**kwargs)

def MdtDriftCircleOnTrackCreatorStau(name="MdtDriftCircleOnTrackCreatorStau",**kwargs ):
    kwargs.setdefault("TimingMode", 3 )
    kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_t0fit') )
    return MdtDriftCircleOnTrackCreator(name,**kwargs)


# For segment fitting with variable t0. Note separate instance names below for Moore and Muonboy
# DoTof must be consistent with the one in MdtDriftCircleOnTrackCreator
def AdjustableT0Tool(name="AdjustableT0Tool",**kwargs):
    # NB: the following 'ifs' are the same as in the MdtDriftCircleOnTrackCreator, so that the ToF setting is the same
    if muonStandaloneFlags.reconstructionMode() == 'cosmics':
        kwargs.setdefault("DoTof", 0)
    else: # collisions simulation final precise cuts
        kwargs.setdefault("DoTof", 1)

    from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import AdjT0__AdjustableT0Tool
    return AdjT0__AdjustableT0Tool(name,**kwargs)
# end of factory function AdjustableT0ToolMboy


def MdtDriftCircleOnTrackCreatorAdjustableT0(name="MdtDriftCircleOnTrackCreatorAdjustableT0",**kwargs):
    kwargs.setdefault("TimingMode", 3)
    kwargs.setdefault("DoTofCorrection", True)
    kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))
    return MdtDriftCircleOnTrackCreator(name,**kwargs)

# default RIO_OnTrackCreator for muons
# make a dedicated class to delay instantiation of the muon RIO_OnTrack creators members
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
class MuonRotCreator(Trk__RIO_OnTrackCreator,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonRotCreator",**kwargs):
        self.applyUserDefaults(kwargs,name)
        kwargs.setdefault("ToolMuonDriftCircle", "MdtDriftCircleOnTrackCreator")
        kwargs.setdefault("ToolMuonCluster", "MuonClusterOnTrackCreator")
        kwargs.setdefault("Mode", 'muon' )
        super(MuonRotCreator,self).__init__(name,**kwargs)
# end of class MuonRotCreator


# configure the pattern recognition

def MuonCombinePatternTool(name="MuonCombinePatternTool",**kwargs):
    kwargs.setdefault("UseTightAssociation", muonStandaloneFlags.reconstructionMode() == 'collisions')
    kwargs.setdefault("UseCosmics", muonStandaloneFlags.reconstructionMode() != 'collisions' )
    return CfgMgr.MuonCombinePatternTool(name,**kwargs)
# end of factory function MuonCombinePatternTool

def MuonHoughPatternTool(name="MuonHoughPatternTool",**kwargs):
    if muonStandaloneFlags.reconstructionMode() == 'collisions':
        kwargs.setdefault("UseCosmics", True)
        kwargs.setdefault("NumberOfMaximaPerIterations", 1)
    return CfgMgr.MuonHoughPatternTool(name,**kwargs)
# end of factory function MuonHoughPatternTool

def MuonHoughPatternFinderTool(name="MuonHoughPatternFinderTool",**kwargs):
    getPublicTool("MuonCombinePatternTool")
    getPublicTool("MuonHoughPatternTool")
    if muonStandaloneFlags.reconstructionMode() == 'collisions':
        kwargs.setdefault("MDT_TDC_cut", False)
    if muonStandaloneFlags.reconstructionMode() == 'collisions' or TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("RecordAll",False)
    return CfgMgr.Muon__MuonHoughPatternFinderTool(name,**kwargs)

#--------------------------------------------------------------------------------
# Tracking geometry
#--------------------------------------------------------------------------------

# combined tracking geometry service
def AtlasTrackingGeometrySvc(name="AtlasTrackingGeometrySvc",**kwargs):
    global ServiceMgr
    from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
    return ServiceMgr.AtlasTrackingGeometrySvc


# default muon navigator
def MuonNavigator(name = "MuonNavigator",**kwargs):
    kwargs.setdefault("TrackingGeometrySvc", "AtlasTrackingGeometrySvc")
    from TrkExTools.TrkExToolsConf import Trk__Navigator
    return Trk__Navigator(name,**kwargs)

# end of factory function MuonNavigator

# set the propagator
from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
class MuonRK_Propagator(Trk__RungeKuttaPropagator,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonRK_Propagator",**kwargs):
        self.applyUserDefaults(kwargs,name)
        kwargs.setdefault("AccuracyParameter", 0.0002 )
        super(MuonRK_Propagator,self).__init__(name,**kwargs)
# end of class MuonRK_Propagator

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
class MuonSTEP_Propagator(Trk__STEP_Propagator,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonSTEP_Propagator",**kwargs):
        kwargs.setdefault("Tolerance", 0.00001 )
        kwargs.setdefault("MaterialEffects", True  )
        kwargs.setdefault("IncludeBgradients", True  )
        self.applyUserDefaults(kwargs,name)
        super(MuonSTEP_Propagator,self).__init__(name,**kwargs)
# end of class MuonSTEP_Propagator


def MuonExtrapolator(name='MuonExtrapolator',**kwargs):
    kwargs.setdefault("Propagators", ["MuonPropagator"])
    kwargs.setdefault("MaterialEffectsUpdators", ["MuonMaterialEffectsUpdator"])
    kwargs.setdefault("Navigator", "MuonNavigator")
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("Tolerance", 0.0011)  # must be > 1um to avoid missing MTG intersections

    return CfgMgr.Trk__Extrapolator(name,**kwargs)
# end of factory function MuonExtrapolator

def MuonIdHelperSvc(name="MuonIdHelperSvc",**kwargs):
    from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperSvc
    kwargs.setdefault("HasCSC", MuonGeometryFlags.hasCSC())
    kwargs.setdefault("HasSTgc", MuonGeometryFlags.hasSTGC())
    kwargs.setdefault("HasMM", MuonGeometryFlags.hasMM())
    return Muon__MuonIdHelperSvc(name,**kwargs)

def MuonStraightLineExtrapolator(name="MuonStraightLineExtrapolator",**kwargs):
    kwargs.setdefault("Propagators",["Trk::STEP_Propagator/MuonStraightLinePropagator"])
    kwargs.setdefault("STEP_Propagator","Trk::STEP_Propagator/MuonStraightLinePropagator")
    return MuonExtrapolator(name,**kwargs)

def MuonEDMHelperSvc(name='MuonEDMHelperSvc',**kwargs):
    # configure some tools that are used but are not declared as properties (they should be!)
    getService("MuonIdHelperSvc")
    getPublicTool("AtlasExtrapolator")

    from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperSvc
    return Muon__MuonEDMHelperSvc(name,**kwargs)
# end of factory function MuonEDMHelperSvc

from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMPrinterTool
class MuonEDMPrinterTool(Muon__MuonEDMPrinterTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name='MuonEDMPrinterTool',**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonEDMPrinterTool,self).__init__(name,**kwargs)
        getService("MuonEDMHelperSvc")
# end of class MuonEDMPrinterTool


def MuonTrackSummaryHelperTool(name="MuonTrackSummaryHelperTool",**kwargs):
    kwargs.setdefault("CalculateCloseHits", True)

    from MuonTrackSummaryHelperTool.MuonTrackSummaryHelperToolConf import Muon__MuonTrackSummaryHelperTool
    return Muon__MuonTrackSummaryHelperTool(name,**kwargs)

# end of factory function MuonTrackSummaryHelper


from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
class MuonTrackSummaryTool(Trk__TrackSummaryTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonTrackSummaryTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        kwargs.setdefault("MuonSummaryHelperTool", "MuonTrackSummaryHelperTool" )
        kwargs.setdefault("doSharedHits", False )
        kwargs.setdefault("AddDetailedMuonSummary", True )
        super(MuonTrackSummaryTool,self).__init__(name,**kwargs)
# end of class MuonTrackSummaryTool


from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
class MuonParticleCreatorTool(Trk__TrackParticleCreatorTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonParticleCreatorTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        kwargs.setdefault("Extrapolator", "AtlasExtrapolator" )
        kwargs.setdefault("TrackSummaryTool", "MuonTrackSummaryTool" )
        kwargs.setdefault("KeepAllPerigee", True )
        kwargs.setdefault("UseMuonSummaryTool", True)
        kwargs.setdefault("PerigeeExpression", "Origin" )
        super(MuonParticleCreatorTool,self).__init__(name,**kwargs)
# end of class MuonParticleCreatorTool

def MuonChi2TrackFitter(name='MuonChi2TrackFitter',**kwargs):
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter

    kwargs.setdefault("ExtrapolationTool"    , "MuonExtrapolator")
    kwargs.setdefault("RotCreatorTool"       , "MuonRotCreator")
    kwargs.setdefault("MeasurementUpdateTool", "MuonMeasUpdator")
    kwargs.setdefault("StraightLine"         , False)
    kwargs.setdefault("OutlierCut"           , 3.0)
    kwargs.setdefault("GetMaterialFromTrack" , False)
    kwargs.setdefault("RejectLargeNScat"     , True)

    # take propagator and navigator from the extrapolator
    Extrapolator = getPublicTool(kwargs["ExtrapolationTool"])
    kwargs.setdefault("PropagatorTool",Extrapolator.Propagators[0].getName())
    kwargs.setdefault("NavigatorTool",Extrapolator.Navigator.getName())

    return Trk__GlobalChi2Fitter(name,**kwargs)


from MuonSegmentMomentum.MuonSegmentMomentumConf import MuonSegmentMomentumFromField as MuonSegMomField
class MuonSegmentMomentumFromField(MuonSegMomField,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonSegmentMomentumFromField",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonSegmentMomentumFromField,self).__init__(name,**kwargs)

MuonSegmentMomentumFromField.setDefaultProperties(
    PropagatorTool = "MuonPropagator",
    NavigatorTool  = "MuonNavigator"
    )


def MuonPhiHitSelector(name="MuonPhiHitSelector",**kwargs):
    kwargs.setdefault("MakeClusters", True)
    kwargs.setdefault("CompetingRios", True)
    kwargs.setdefault("DoCosmics", muonStandaloneFlags.reconstructionMode() != 'collisions')

    return CfgMgr.MuonPhiHitSelector(name,**kwargs)
# end of factory function MuonPhiHitSelector

def MuonSegmentMomentum(name="MuonSegmentMomentum",**kwargs):
    kwargs.setdefault("DoCosmics", muonStandaloneFlags.reconstructionMode() != 'collisions')
    return CfgMgr.MuonSegmentMomentum(name,**kwargs)
# end of factory function MuonSegmentMomentum

def MdtSegmentT0Fitter(name="MdtSegmentT0Fitter",**kwargs):
    # setup dependencies missing in C++. TODO: fix in C++
    from MuonRecExample import MuonAlignConfig
    from MuonCnvExample import MuonCalibConfig
    MuonCalibConfig.setupMdtCondDB()
    return CfgMgr.TrkDriftCircleMath__MdtSegmentT0Fitter(name,**kwargs)

def MdtMathSegmentFinder(name="MdtMathSegmentFinder",extraFlags=None,**kwargs):
    beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit",muonRecFlags.doSegmentT0Fit())
    enableCurvedSegmentFinding = getattr(extraFlags,"enableCurvedSegmentFinding", muonStandaloneFlags.enableCurvedSegmentFinding())

    if doSegmentT0Fit:
        kwargs.setdefault("AssociationRoadWidth", 3.)
        kwargs.setdefault("MDTAssocationPullcut", 3.)
        kwargs.setdefault("RecoverMdtOutliers", False)
        kwargs.setdefault("DCFitProvider", "MdtSegmentT0Fitter" )

    if beamType == 'singlebeam' or beamType == 'cosmics' or globalflags.DataSource() == 'data':
        kwargs.setdefault("AssociationRoadWidth", 2.)
        kwargs.setdefault("MDTAssocationPullcut", 4.)
        kwargs.setdefault("RecoverMdtOutliers", True )

    if enableCurvedSegmentFinding:
        kwargs.setdefault("DoCurvedSegmentFinder",True)

    return CfgMgr.Muon__MdtMathSegmentFinder(name,**kwargs)

def MdtMathT0FitSegmentFinder(name="MdtMathT0FitSegmentFinder",extraFlags=None,**kwargs):
    if extraFlags is None: extraFlags = ExtraFlags()
    extraFlags.setFlagDefault('doSegmentT0Fit',True)
    return MdtMathSegmentFinder(name,extraFlags,**kwargs)

def MuonClusterSegmentFinder(name="MuonClusterSegmentFinder", extraFlags=None,**kwargs):
    kwargs.setdefault("AmbiguityProcessor",getPublicTool("MuonAmbiProcessor"))
    return CfgMgr.Muon__MuonClusterSegmentFinder(name,**kwargs)

def MuonClusterSegmentFinderTool(name="MuonClusterSegmentFinderTool", extraFlags=None,**kwargs):
    kwargs.setdefault("SLFitter","Trk::GlobalChi2Fitter/MCTBSLFitterMaterialFromTrack")
    import MuonCombinedRecExample.CombinedMuonTrackSummary
    from AthenaCommon.AppMgr import ToolSvc
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("TrackSummaryTool", "MuonTrackSummaryTool" )
    else:
        kwargs.setdefault("TrackSummaryTool", ToolSvc.CombinedMuonTrackSummary)
    return CfgMgr.Muon__MuonClusterSegmentFinderTool(name,**kwargs)

def DCMathSegmentMaker(name='DCMathSegmentMaker',extraFlags=None,**kwargs):
    beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit", muonRecFlags.doSegmentT0Fit())
    updateSegmentSecondCoordinate = getattr(extraFlags,"updateSegmentSecondCoordinate", muonStandaloneFlags.updateSegmentSecondCoordinate())
    enableCurvedSegmentFinding = getattr(extraFlags,"enableCurvedSegmentFinding", muonStandaloneFlags.enableCurvedSegmentFinding())

    kwargs.setdefault("RefitSegment", True)
    kwargs.setdefault("AssumePointingPhi", beamType != 'cosmics')
    kwargs.setdefault("OutputFittedT0", True)
    kwargs.setdefault("DCFitProvider", "MdtSegmentT0Fitter")
    #kwargs.setdefault("CurvedErrorScaling", False)
    kwargs.setdefault("UsePreciseError", True)
    kwargs.setdefault("SinAngleCut", 0.4)

    #MDT conditions information not available online
    if(athenaCommonFlags.isOnline):
        kwargs.setdefault("MdtCondKey","")

    # MuonCompetingClustersCreator apparently just takes default
    kwargs.setdefault("MuonClusterCreator", getPrivateTool("MuonClusterOnTrackCreator") )

    if (beamType == 'singlebeam' or beamType == 'cosmics'):
        kwargs.setdefault("SinAngleCut", 0.9)
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)
        kwargs.setdefault("CurvedErrorScaling", False)
    elif globalflags.DataSource() == 'data': # collisions real data or simulation first data
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)

    if doSegmentT0Fit:
        kwargs.setdefault("MdtCreatorT0", getPrivateTool("MdtDriftCircleOnTrackCreatorAdjustableT0") )
        kwargs.setdefault("MdtSegmentFinder", getPrivateTool("MdtMathT0FitSegmentFinder") )
    else:
        kwargs.setdefault("MdtSegmentFinder", getPrivateTool("MdtMathSegmentFinder") )
    kwargs.setdefault("SegmentFitter", getPrivateTool("MuonSegmentFittingTool") )
    kwargs.setdefault("SegmentSelector", getPrivateTool("MuonSegmentSelectionTool") )

    if updateSegmentSecondCoordinate:
        kwargs.setdefault("UpdatePhiUsingPhiHits",True)

    if enableCurvedSegmentFinding:
        kwargs.setdefault("CurvedErrorScaling", False)
        kwargs.setdefault("PreciseErrorScale", 1)
        kwargs.setdefault("UsePreciseError", True)

    return CfgMgr.Muon__DCMathSegmentMaker(name,**kwargs)


def DCMathT0FitSegmentMaker(name='DCMathT0FitSegmentMaker',extraFlags=None,**kwargs):
    if extraFlags is None: extraFlags = ExtraFlags()
    extraFlags.setFlagDefault('doSegmentT0Fit',True)
    return DCMathSegmentMaker(name,extraFlags,**kwargs)

# end of factory function DCMathSegmentMaker

def MuonLayerHoughTool(name='MuonLayerHoughTool',extraFlags=None,**kwargs):
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("DoTruth", False)
    else:
        kwargs.setdefault("DoTruth", rec.doTruth())
    return CfgMgr.Muon__MuonLayerHoughTool(name,**kwargs)

def MuonSegmentFittingTool(name='MuonSegmentFittingTool',extraFlags=None,**kwargs):
    prop = getPublicTool('AtlasRungeKuttaPropagator')
    kwargs.setdefault("SLPropagator", getPrivateToolClone('SLPropagator',prop) )
    # Think I need to do it this way because AtlasRungeKuttaPropagator isn't known to ConfigurableFactory.
    # If I directly call getPrivateTool('AtlasRungeKuttaPropagator') then it fails with:
    # ConfigurationError: Private Tool <Trk::RungeKuttaPropagator/ToolSvc.AtlasRungeKuttaPropagator at 0x7f5811db3158> not found
    kwargs.setdefault("SLFitter",     getPrivateTool('MCTBSLFitter') )
    kwargs.setdefault("CurvedFitter", getPrivateTool('MCTBFitter') )
    kwargs.setdefault("TrackCleaner", getPrivateTool('MuonTrackCleaner')  )
    return CfgMgr.Muon__MuonSegmentFittingTool(name,**kwargs)

if DetFlags.detdescr.Muon_on() and rec.doMuon():
    # until all clients explicitly get their tools and services, load some explicitly
    getPublicTool("ResidualPullCalculator")
    getPublicTool("MuonHoughPatternTool")
    getPublicTool("MuonCombinePatternTool")
    getPublicTool("MuonPhiHitSelector")
    getPublicTool("MuonEDMPrinterTool")
    getPublicTool("MuonSegmentMomentum")
    getPublicTool("MuonClusterOnTrackCreator")
    if MuonGeometryFlags.hasCSC():
        getPrivateTool("CscClusterOnTrackCreator")
        getPrivateTool("CscBroadClusterOnTrackCreator")
    getPublicTool("MdtDriftCircleOnTrackCreator")
    getPublicTool("MdtTubeHitOnTrackCreator")

    #getService("SomeService")

else: # not (DetFlags.Muon_on() and rec.doMuon())
    logMuon.warning("Muon reconstruction tools only loaded on-demand because Muons")

def MuonLayerSegmentFinderTool(name='MuonLayerSegmentFinderTool',extraFlags=None,**kwargs):
    kwargs.setdefault("Csc2DSegmentMaker", getPublicTool("Csc2dSegmentMaker") if MuonGeometryFlags.hasCSC() else "")
    kwargs.setdefault("Csc4DSegmentMaker", getPublicTool("Csc4dSegmentMaker") if MuonGeometryFlags.hasCSC() else "")
    kwargs.setdefault("MuonClusterSegmentFinder",getPublicTool("MuonClusterSegmentFinder"))

    return CfgMgr.Muon__MuonLayerSegmentFinderTool(name,**kwargs)