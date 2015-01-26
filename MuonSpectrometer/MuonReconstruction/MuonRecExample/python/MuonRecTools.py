# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s", __name__)

from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam

from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber
from MuonRecUtils import logMuon,ConfiguredBase,uglyHackedInclude

from MuonRecFlags import muonRecFlags,mooreFlags
muonRecFlags.setDefaults()
mooreFlags.setDefaults()

from MuonCnvExample.MuonCalibFlags import mdtCalibFlags
mdtCalibFlags.setDefaults()

from RecExConfig.RecFlags import rec


from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
     getService,getServiceClone,getAlgorithm,getAlgorithmClone

# temporarily for backwards compat. TO BE REMOVED
from AthenaCommon.CfgGetter import addTool,addToolClone,addService


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


def CscClusterOnTrackCreator(name="CscClusterOnTrackCreator",**kwargs):
    kwargs.setdefault("CscStripFitter", getPublicTool("CalibCscStripFitter") )
    kwargs.setdefault("CscClusterFitter", getPublicTool("QratCscClusterFitter") )
    kwargs.setdefault("CscClusterUtilTool", getPublicTool("CscClusterUtilTool") )

    if globalflags.DataSource() == 'data': # collisions real data or simulated first data
        # scale CSC and hit errors 
        kwargs.setdefault("ErrorScalerBeta", 0.070 )

    return CfgMgr.Muon__CscClusterOnTrackCreator(name,**kwargs)

def CscBroadClusterOnTrackCreator(name="CscBroadClusterOnTrackCreator",**kwargs):
    kwargs["ErrorScalerBeta"] = 0.200
    return CscClusterOnTrackCreator(name,**kwargs)


def MdtDriftCircleOnTrackCreator(name="MdtDriftCircleOnTrackCreator",**kwargs):
    # setup dependencies missing in C++. TODO: fix in C++
    getService("MdtCalibrationSvc")
    getService("MdtCalibrationDbSvc")
    
    kwargs.setdefault("DoMagneticFieldCorrection", mdtCalibFlags.correctMdtRtForBField())
    kwargs.setdefault("DoWireSag", muonRecFlags.useWireSagCorrections())
    kwargs.setdefault("DoSlewingCorrection", mdtCalibFlags.correctMdtRtForTimeSlewing())

    if muonRecFlags.forceCollisionsMode(): #this setting uses loose collisions settings, with cosmics timing 
        #print "MdtDriftCircleOnTrackCreator::configuring forceCollisionsMode"
        kwargs.setdefault("DoTofCorrection", False)
        kwargs.setdefault("DoFixedError", True)
        kwargs.setdefault("MuonTofTool", None)
        kwargs.setdefault("TimingMode", 1)

    elif beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' :
        #print "MdtDriftCircleOnTrackCreator::configuring cosmics"
        kwargs.setdefault("DoTofCorrection", False)
        kwargs.setdefault("DoFixedError", True)
        kwargs.setdefault("TimingMode", 1)
        kwargs.setdefault("UseParametrisedError", True)

    elif globalflags.DataSource() == 'data': # collisions real data or simulated first data
        #print "MdtDriftCircleOnTrackCreator::configuring data"
        # set to use: Moore segment errors: 
        kwargs.setdefault("DoTofCorrection", True)
        kwargs.setdefault("DoFixedError", True)   # FixedErrors
        kwargs.setdefault("DoErrorScaling", True) # ScaledErrors
        kwargs.setdefault("CreateTubeHit", True)  # BroadErrors
        kwargs.setdefault("MuonTofTool", None) 
        kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))
        kwargs.setdefault("UseParametrisedError", False)

    else: # collisions simulation final precise cuts
        #print "MdtDriftCircleOnTrackCreator::configuring default"
        kwargs.setdefault("IsMC", True)
        kwargs.setdefault("DoFixedError", False)
        kwargs.setdefault("DoErrorScaling", False)
        kwargs.setdefault("DoTofCorrection", True)
        kwargs.setdefault("MuonTofTool", None)
        kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))  # MJW: should this be Collision_G4 ???

    from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import Muon__MdtDriftCircleOnTrackCreator
    return Muon__MdtDriftCircleOnTrackCreator(name,**kwargs)
# end of factory function MdtDriftCircleOnTrackCreator


def MdtTubeHitOnTrackCreator(name="MdtTubeHitOnTrackCreator",**kwargs):
    kwargs["CreateTubeHit"] = True
    return MdtDriftCircleOnTrackCreator(name,**kwargs)


# For segment fitting with variable t0. Note separate instance names below for Moore and Muonboy
# DoTof must be consistent with the one in MdtDriftCircleOnTrackCreator
def AdjustableT0Tool(name="AdjustableT0Tool",**kwargs):
    # NB: the following 'ifs' are the same as in the MdtDriftCircleOnTrackCreator, so that the ToF setting is the same
    if muonRecFlags.forceCollisionsMode(): #this setting uses loose collisions settings, with cosmics timing 
        kwargs.setdefault("DoTof", 0)
    elif beamFlags.beamType() == 'cosmics' or beamFlags.beamType() == 'singlebeam' :
        kwargs.setdefault("DoTof", 0)
    elif globalflags.DataSource() == 'data' or muonRecFlags.forceDataMode(): # collisions real data or simulated first data
        kwargs.setdefault("DoTof", 1)
    else: # collisions simulation final precise cuts
        kwargs.setdefault("DoTof", 1)
        
    from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import AdjT0__AdjustableT0Tool
    return AdjT0__AdjustableT0Tool(name,**kwargs)
# end of factory function AdjustableT0ToolMboy


def MdtDriftCircleOnTrackCreatorAdjustableT0(name="MdtDriftCircleOnTrackCreatorAdjustableT0",**kwargs):
    kwargs.setdefault("TimingMode", 3)
    kwargs.setdefault("DoTofCorrection", True)
    kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))
    kwargs.setdefault("MuonTofTool", "AdjustableT0Tool")

    return MdtDriftCircleOnTrackCreator(name,**kwargs)

def MdtDriftCircleOnTrackCreatorAdjustableT0Mboy(name="MdtDriftCircleOnTrackCreatorAdjustableT0Mboy",**kwargs):
    kwargs["MuonTofTool"] = getPublicToolClone("AdjustableT0ToolMboy","AdjustableT0Tool")
    return MdtDriftCircleOnTrackCreatorAdjustableT0(name,**kwargs)


def MdtDriftCircleOnTrackCreatorAdjustableT0Moore(name="MdtDriftCircleOnTrackCreatorAdjustableT0Moore",**kwargs):
    kwargs["MuonTofTool"] = getPublicToolClone("AdjustableT0ToolMoore","AdjustableT0Tool")
    return MdtDriftCircleOnTrackCreatorAdjustableT0(name,**kwargs)
    
    
# default RIO_OnTrackCreator for muons
# make a dedicated class to delay instantiation of the muon RIO_OnTrack creators members
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
class MuonRotCreator(Trk__RIO_OnTrackCreator,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonRotCreator",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonRotCreator,self).__init__(name,**kwargs)

MuonRotCreator.setDefaultProperties(
    ToolMuonDriftCircle = "MdtDriftCircleOnTrackCreator" ,
    ToolMuonCluster     = "MuonClusterOnTrackCreator"    ,
    Mode                = 'muon' )
# end of class MuonRotCreator


# configure the pattern recognition

def MuonCombinePatternTool(name="MuonCombinePatternTool",**kwargs):
    if beamFlags.beamType() == 'collisions' or muonRecFlags.forceCollisionsMode(): 
        kwargs.setdefault("UseTightAssociation", True)
    else:  #cosmics or singlebeam   
        kwargs.setdefault("UseCosmics", True)

    return CfgMgr.MuonCombinePatternTool(name,**kwargs)
# end of factory function MuonCombinePatternTool

def MuonHoughPatternTool(name="MuonHoughPatternTool",**kwargs):
    if beamFlags.beamType() != 'collisions' and not muonRecFlags.forceCollisionsMode(): 
        kwargs.setdefault("UseCosmics", True)
        kwargs.setdefault("NumberOfMaximaPerIterations", 1)

    return CfgMgr.MuonHoughPatternTool(name,**kwargs)
# end of factory function MuonHoughPatternTool

def MuonHoughPatternFinderTool(name="MuonHoughPatternFinderTool",**kwargs): 
    getPublicTool("MuonCombinePatternTool") 
    getPublicTool("MuonHoughPatternTool") 
    if beamFlags.beamType() != 'collisions' and not muonRecFlags.forceCollisionsMode():  
        kwargs.setdefault("MDT_TDC_cut", False)
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
        super(MuonRK_Propagator,self).__init__(name,**kwargs)

MuonRK_Propagator.setDefaultProperties( AccuracyParameter = 0.0002 )
# end of class MuonRK_Propagator

from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator
class MuonSTEP_Propagator(Trk__STEP_Propagator,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonSTEP_Propagator",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonSTEP_Propagator,self).__init__(name,**kwargs)

MuonSTEP_Propagator.setDefaultProperties(
    Tolerance         = 0.00001 ,
    MaterialEffects   = True   , 
    IncludeBgradients = True  )
# end of class MuonSTEP_Propagator


def MuonExtrapolator(name='MuonExtrapolator',**kwargs):
    kwargs.setdefault("Propagators", ["MuonPropagator"])
    kwargs.setdefault("MaterialEffectsUpdators", ["MuonMaterialEffectsUpdator"])
    kwargs.setdefault("Navigator", "MuonNavigator")
    kwargs.setdefault("ResolveMuonStation", True)
    kwargs.setdefault("Tolerance", 0.0011)  # must be > 1um to avoid missing MTG intersections

    return CfgMgr.Trk__Extrapolator(name,**kwargs)
# end of factory function MuonExtrapolator


def MuonRK_Extrapolator(name="MuonRK_Extrapolator",**kwargs):
    kwargs["Propagators"] = ["MuonRK_Propagator"]
    return MuonExtrapolator(name,**kwargs)


def MuonSTEP_Extrapolator(name="MuonSTEP_Extrapolator",**kwargs):
    kwargs["Propagators"] = ["MuonSTEP_Propagator"]
    return MuonExtrapolator(name,**kwargs)


def MuonStraightLineExtrapolator(name="MuonStraightLineExtrapolator",**kwargs):
    kwargs["Propagators"] = ["MuonStraightLinePropagator"]
    return MuonExtrapolator(name,**kwargs)


def MuonEDMHelperTool(name='MuonEDMHelperTool',**kwargs):
    # configure some tools that are used but are not declared as properties (they should be!)
    getPublicTool("MuonIdHelperTool")
    getPublicTool("MuonExtrapolator")
    getPublicTool("AtlasExtrapolator")

    from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMHelperTool
    return Muon__MuonEDMHelperTool(name,**kwargs)
# end of factory function MuonEDMHelperTool

from MuonRecHelperTools.MuonRecHelperToolsConf import Muon__MuonEDMPrinterTool
class MuonEDMPrinterTool(Muon__MuonEDMPrinterTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name='MuonEDMPrinterTool',**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonEDMPrinterTool,self).__init__(name,**kwargs)
        getPublicTool("MuonIdHelperTool")
        getPublicTool("MuonEDMHelperTool")
# end of class MuonEDMPrinterTool


from TrkKalmanFitter.TrkKalmanFitterConf import Trk__KalmanFitter
class MuonKalmanTrackFitter(Trk__KalmanFitter,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name='MuonKalmanTrackFitter',**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonKalmanTrackFitter,self).__init__(name,**kwargs)

MuonKalmanTrackFitter.setDefaultProperties(
    ExtrapolatorHandle             = "MuonExtrapolator" ,
    MeasurementUpdatorHandle       = "MuonMeasUpdator"  ,
    RIO_OnTrackCreatorHandle       = None ,
    DynamicNoiseAdjustorHandle     = None ,
    AlignableSurfaceProviderHandle = None )
##    InternalDAFHandle              = None )
# end of class MuonKalmanTrackFitter


#def MuonHolesOnTrackTool(name='MuonHolesOnTrackTool',**kwargs):
#    kwargs.setdefault("ExtrapolatorName"      , "MuonExtrapolator" )
#    kwargs.setdefault("RIO_OnTrackCreator"    , "MuonRotCreator"   )
#    kwargs.setdefault("DoHolesIdentification" , True )
#    kwargs.setdefault("DoParameterUpdate", True )
#    kwargs.setdefault("LowerTrackMomentumCut", 2000.0 )
#    AtlasTrackingGeometrySvc = getService("AtlasTrackingGeometrySvc")
#    kwargs.setdefault("TrackingGeometryName", AtlasTrackingGeometrySvc.TrackingGeometryName )

#    from MuonTGRecTools.MuonTGRecToolsConf import Muon__MuonHolesOnTrackTool
#    return Muon__MuonHolesOnTrackTool(name,**kwargs)
# end of factory function MuonHolesOnTrackTool


def MuonTrackSummaryHelper(name="MuonTrackSummaryHelper",**kwargs):
    AtlasTrackingGeometrySvc = getService("AtlasTrackingGeometrySvc")
    kwargs.setdefault("TrackingGeometryName", AtlasTrackingGeometrySvc.TrackingGeometryName)
    #kwargs.setdefault("HoleOnTrackTool", "MuonHolesOnTrackTool")
    kwargs.setdefault("DoHolesOnTrack", False)
    kwargs.setdefault("CalculateCloseHits", True)

    from MuonTrackSummaryHelperTool.MuonTrackSummaryHelperToolConf import Muon__MuonTrackSummaryHelperTool
    return Muon__MuonTrackSummaryHelperTool(name,**kwargs)

# end of factory function MuonTrackSummaryHelper


from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
class MuonTrackSummaryTool(Trk__TrackSummaryTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonTrackSummaryTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonTrackSummaryTool,self).__init__(name,**kwargs)

MuonTrackSummaryTool.setDefaultProperties(
    MuonSummaryHelperTool = "MuonTrackSummaryHelper" ,
    doSharedHits          = False,
    AddDetailedMuonSummary = True )
# end of class MuonTrackSummaryTool


from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
class MuonParticleCreatorTool(Trk__TrackParticleCreatorTool,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MuonParticleCreatorTool",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MuonParticleCreatorTool,self).__init__(name,**kwargs)

MuonParticleCreatorTool.setDefaultProperties(
    Extrapolator     = "AtlasExtrapolator"    ,
    TrackSummaryTool = "MuonTrackSummaryTool",
    KeepAllPerigee=True,
    UseMuonSummaryTool=True,
    PerigeeExpression="Origin")
# end of class MuonParticleCreatorTool

def MuonChi2TrackFitter(name='MuonChi2TrackFitter',**kwargs):
    from TrkGlobalChi2Fitter.TrkGlobalChi2FitterConf import Trk__GlobalChi2Fitter

    kwargs.setdefault("ExtrapolationTool"    , "MuonExtrapolator")
    kwargs.setdefault("RotCreatorTool"       , "MuonRotCreator")
    kwargs.setdefault("MeasurementUpdateTool", "MuonMeasUpdator")
    kwargs.setdefault("StraightLine"         , False)
    kwargs.setdefault("OutlierCut"           , 3.0)
    kwargs.setdefault("GetMaterialFromTrack" , False)

    # take propagator and navigator from the extrapolator
    Extrapolator = getPublicTool(kwargs["ExtrapolationTool"])
    kwargs["ExtrapolationTool"] = Extrapolator
    kwargs["PropagatorTool"]    = getPublicTool(Extrapolator.Propagators[0].getName())
    kwargs["NavigatorTool"]     = getPublicTool(Extrapolator.Navigator.getName())

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
    if beamFlags.beamType() != 'collisions' and not muonRecFlags.forceCollisionsMode() : 
        kwargs.setdefault("DoCosmics", True)

    return CfgMgr.MuonPhiHitSelector(name,**kwargs)
# end of factory function MuonPhiHitSelector

def MuonSegmentMomentum(name="MuonSegmentMomentum",**kwargs):
    if beamFlags.beamType() != 'collisions' and not muonRecFlags.forceCollisionsMode() : 
        kwargs.setdefault("DoCosmics", True)

    return CfgMgr.MuonSegmentMomentum(name,**kwargs)
# end of factory function MuonSegmentMomentum

def MdtSegmentT0Fitter(name="MdtSegmentT0Fitter",**kwargs):
    # setup dependencies missing in C++. TODO: fix in C++
    getService("MdtCalibrationDbSvc")
    return CfgMgr.TrkDriftCircleMath__MdtSegmentT0Fitter(name,**kwargs)

def MdtMathSegmentFinder(name="MdtMathSegmentFinder",extraFlags=None,**kwargs):
    beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit",muonRecFlags.doSegmentT0Fit())
    enableCurvedSegmentFinding = getattr(extraFlags,"enableCurvedSegmentFinding", mooreFlags.enableCurvedSegmentFinding())

    if beamType == 'singlebeam' or beamType == 'cosmics' or \
           globalflags.DataSource() == 'data' or muonRecFlags.forceDataMode():

        if doSegmentT0Fit:
            kwargs.setdefault("AssociationRoadWidth", 3.)
            kwargs.setdefault("MDTAssocationPullcut", 3.)
            kwargs.setdefault("RecoverMdtOutliers", False)
            kwargs.setdefault("DCFitProvider", "MdtSegmentT0Fitter" )
        else:
            kwargs.setdefault("AssociationRoadWidth", 2.)
            kwargs.setdefault("MDTAssocationPullcut", 4.)
            kwargs.setdefault("RecoverMdtOutliers", True )

    if enableCurvedSegmentFinding:
        kwargs.setdefault("DoCurvedSegmentFinder",True)

    return CfgMgr.Muon__MdtMathSegmentFinder(name,**kwargs)

def MuonClusterSegmentFinderTool(name="MuonClusterSegmentFinderTool", extraFlags=None,**kwargs):
    
    return CfgMgr.Muon__MuonClusterSegmentFinderTool(name,**kwargs)

def DCMathSegmentMaker(name='DCMathSegmentMaker',extraFlags=None,**kwargs):
    beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit", muonRecFlags.doSegmentT0Fit())
    updateSegmentSecondCoordinate = getattr(extraFlags,"updateSegmentSecondCoordinate", mooreFlags.updateSegmentSecondCoordinate())
    enableCurvedSegmentFinding = getattr(extraFlags,"enableCurvedSegmentFinding", mooreFlags.enableCurvedSegmentFinding())
        
    kwargs.setdefault("MdtSegmentFinder", "MdtMathSegmentFinder")
    kwargs.setdefault("RefitSegment", True)
    kwargs.setdefault("AssumePointingPhi", beamType != 'cosmics')
    kwargs.setdefault("OutputFittedT0", True)
    kwargs.setdefault("DCFitProvider", "MdtSegmentT0Fitter")
    #kwargs.setdefault("CurvedErrorScaling", False)

    if (beamType == 'singlebeam' or beamType == 'cosmics') and not muonRecFlags.forceCollisionsMode : 
        kwargs.setdefault("SinAngleCut", 0.9)
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)
        kwargs.setdefault("CurvedErrorScaling", False)
        kwargs.setdefault("UsePreciseError", True)
        kwargs.setdefault("PreciseErrorScale", 2.)
    elif globalflags.DataSource() == 'data' or muonRecFlags.forceDataMode: # collisions real data or simulation first data
        kwargs.setdefault("SinAngleCut", 0.4)
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)
        kwargs.setdefault("UsePreciseError", True)
        kwargs.setdefault("PreciseErrorScale", 2.)

    if doSegmentT0Fit:
        kwargs.setdefault("MdtCreatorT0", "MdtDriftCircleOnTrackCreatorAdjustableT0Moore")
        kwargs.setdefault("TofTool", "AdjustableT0ToolMoore")

    if updateSegmentSecondCoordinate:
        kwargs.setdefault("UpdatePhiUsingPhiHits",True)

    if enableCurvedSegmentFinding:
        kwargs.setdefault("CurvedErrorScaling", False)
        kwargs.setdefault("PreciseErrorScale", 1)
        kwargs.setdefault("UsePreciseError", True)

    return CfgMgr.Muon__DCMathSegmentMaker(name,**kwargs)

# end of factory function DCMathSegmentMaker

def MuonSegmentMerger(name='MuonSegmentMerger',extraFlags=None,**kwargs):
    kwargs.setdefault("TriggerHitAssociator", getPublicToolClone("TriggerHitAssociator", "DCMathSegmentMaker",Redo2DFit=False) )

    return CfgMgr.Muon__MuonSegmentMerger(name,**kwargs)

def MuonLayerHoughTool(name='MuonLayerHoughTool',extraFlags=None,**kwargs):
    kwargs.setdefault("DoTruth", rec.doTruth() )

    return CfgMgr.Muon__MuonLayerHoughTool(name,**kwargs)

  
    
#from TrkEventCnvTools import TrkEventCnvToolsConfig



if DetFlags.detdescr.Muon_on() and rec.doMuon():
    # until all clients explicitly get their tools and services, load some explicitly
    getPublicTool("ResidualPullCalculator")
    getPublicTool("MuonHoughPatternTool")
    getPublicTool("MuonCombinePatternTool")
    getPublicTool("MuonPhiHitSelector")
    getPublicTool("MuonEDMPrinterTool")
    getPublicTool("MuonSTEP_Extrapolator")
    getPublicTool("MuonSegmentMomentum")
    getPublicTool("MuonClusterOnTrackCreator")
    getPublicTool("CscClusterOnTrackCreator")
    getPublicTool("CscBroadClusterOnTrackCreator")
    getPublicTool("MdtDriftCircleOnTrackCreator")
    getPublicTool("MdtTubeHitOnTrackCreator")
        
    #getService("SomeService")

else: # not (DetFlags.Muon_on() and rec.doMuon())
    logMuon.warning("Muon reconstruction tools only loaded on-demand because Muons are off")
