# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# This file configures the Muon segment finding. It is based on a few files in the old configuration system:
# Tools, which are configured here: 
# https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecTools.py
# https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py
# from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/CscTools.py
from AthenaConfiguration.ComponentFactory import CompFactory
#
# and algorithms which are defined in several places:
# 
#
# When porting it I have tried to remove anything redundant, and I have tried to simplify the number of configuration techniques 
# used, so for example I'm no longer using CfgGetter, and am minimising the use of CfgMgr,

# Core
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import Format

# Muon
# Csc2dSegmentMaker, Csc4dSegmentMaker=CompFactory.getComps("Csc2dSegmentMaker","Csc4dSegmentMaker",)

#Local
from MuonConfig.MuonCalibrationConfig import MdtCalibrationDbToolCfg
from MuonConfig.MuonRecToolsConfig import MCTBFitterCfg, MCTBSLFitterMaterialFromTrackCfg, MuonAmbiProcessorCfg, MuonStationIntersectSvcCfg, MuonTrackCleanerCfg, MuonTrackSummaryToolCfg, MuonEDMPrinterTool
from TrkConfig.AtlasExtrapolatorConfig import MuonStraightLineExtrapolatorCfg
from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtCalibWindowNumber

def MuonHoughPatternFinderTool(flags, **kwargs):
    # Taken from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecTools.py#L173

    Muon__MuonHoughPatternFinderTool=CompFactory.Muon.MuonHoughPatternFinderTool
    
    # TODO
    # getPublicTool("MuonCombinePatternTool")
    # getPublicTool("MuonHoughPatternTool")
    if flags.Beam.Type == 'collisions':
        kwargs.setdefault("MDT_TDC_cut", False)
        kwargs.setdefault("RecordAll",False)
    return Muon__MuonHoughPatternFinderTool("MuonHoughPatternFinderTool",**kwargs)

def MuonCurvedSegmentCombiner(flags, **kwargs):  
    # Taken from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L74
    # The original code seems very odd. The default MissedHitsCut is 100 by default, and the rest of it is a bit tortuous.
    # I've tried to clean it up, but might have made mistakes. 
    Muon__MuonCurvedSegmentCombiner=CompFactory.Muon.MuonCurvedSegmentCombiner
    if flags.Beam.Type!= 'collisions' :
        kwargs.setdefault( "AddUnassociatedMiddleEndcapSegments", False )
    elif flags.Input.isMC:
        kwargs.setdefault( "MissedHitsCut", 4 )

    kwargs.setdefault("DoCosmics", flags.Beam.Type != 'collisions' )
    kwargs.setdefault( "AddAll2DCscs", False )
    kwargs.setdefault( "UseCscSegments", flags.Muon.doCSCs )
    kwargs.setdefault( "AddUnassociatedMiddleEndcapSegments", True )
    return Muon__MuonCurvedSegmentCombiner("MuonCurvedSegmentCombiner", **kwargs)
    
# def MuonSegmentCombinationCleanerTool(flags):

def MdtDriftCircleOnTrackCreatorAdjustableT0Cfg(flags,**kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MuonClusterOnTrackCreatorCfg
    kwargs.setdefault("TimingMode", 3)
    kwargs.setdefault("DoTofCorrection", True)
    kwargs.setdefault("TimeWindowSetting", MdtCalibWindowNumber('Collision_data'))
    acc = MuonClusterOnTrackCreatorCfg(flags, **kwargs)  
    return acc

def AdjustableT0Tool(flags,**kwargs):
    # NB: the following 'ifs' are the same as in the MdtDriftCircleOnTrackCreator, so that the ToF setting is the same
    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("DoTof", 0)
    else: # collisions simulation final precise cuts
        kwargs.setdefault("DoTof", 1)
        
    AdjT0__AdjustableT0Tool=CompFactory.getComp("AdjT0::AdjustableT0Tool")
    return AdjT0__AdjustableT0Tool(**kwargs)

def MdtMathSegmentFinderCfg(flags,name="MdtMathSegmentFinder", **kwargs):
    # beamType       = getattr(extraFlags,"beamType", beamFlags.beamType())
    # doSegmentT0Fit = getattr(extraFlags,"doSegmentT0Fit",muonRecFlags.doSegmentT0Fit())
    # enableCurvedSegmentFinding = getattr(extraFlags,"enableCurvedSegmentFinding", muonStandaloneFlags.enableCurvedSegmentFinding())
    result = ComponentAccumulator()

    if flags.Muon.doSegmentT0Fit:
        kwargs.setdefault("AssociationRoadWidth", 3.)
        kwargs.setdefault("MDTAssocationPullcut", 3.)
        kwargs.setdefault("RecoverMdtOutliers", False)
        kwargs.setdefault("DCFitProvider", result.popToolsAndMerge(MdtSegmentT0FitterCfg(flags) ) )

    if flags.Beam.Type == 'singlebeam' or flags.Beam.Type == 'cosmics' or flags.Input.isMC is False:
        kwargs.setdefault("AssociationRoadWidth", 2.)
        kwargs.setdefault("MDTAssocationPullcut", 4.)
        kwargs.setdefault("RecoverMdtOutliers", True )

    if flags.Muon.enableCurvedSegmentFinding:
        kwargs.setdefault("DoCurvedSegmentFinder",True)
    result.setPrivateTools(CompFactory.Muon.MdtMathSegmentFinder(name=name,**kwargs))
    return result

def MuonSegmentFittingToolCfg(flags, **kwargs):
    # declareProperty("SLPropagator",   m_slPropagator);
    # declareProperty("SLFitter",       m_slTrackFitter);
    # declareProperty("CurvedFitter",   m_curvedTrackFitter);
    # declareProperty("TrackCleaner",   m_trackCleaner);
    # declareProperty("UpdatePrecisionCoordinate", m_updatePrecisionCoordinate = false );
    result=ComponentAccumulator()
    # FIXME! Add this.
    # acc, propagator = AtlasRungeKuttaPropagatorCfg(flags)
    # result.merge(acc)
    # kwargs.setdefault("SLPropagator", propagator)
    
    acc = MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True)
    slfitter = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("SLFitter", slfitter)
    
    acc  = MCTBFitterCfg(flags, name = "CurvedFitter")
    fitter = acc.getPrimary()
    
    result.merge(acc)
    kwargs.setdefault("CurvedFitter", fitter)
    
    acc = MuonTrackCleanerCfg(flags)
    cleaner = acc.getPrimary()
    
    result.merge(acc)
    kwargs.setdefault("TrackCleaner", cleaner)
    result.setPrivateTools(CompFactory.Muon.MuonSegmentFittingTool(**kwargs))    
    return result

def MdtSegmentT0FitterCfg(flags, name="MdtSegmentT0Fitter", **kwargs):
    result = MdtCalibrationDbToolCfg(flags) # Needed by MdtSegmentT0Fitter
    kwargs.setdefault("CalibrationDbTool", result.popPrivateTools())
    result.setPrivateTools(CompFactory.TrkDriftCircleMath.MdtSegmentT0Fitter(name, **kwargs))    
    return result

def DCMathSegmentMakerCfg(flags, **kwargs):    
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, MuonClusterOnTrackCreatorCfg, TriggerChamberClusterOnTrackCreatorCfg
    from MuonConfig.MuonCondAlgConfig import MdtCondDbAlgCfg
     
    # This in general is a pretty problematic piece of code. It seems to have a lot of potential issues, because it has loads of mutables / subtools etc
    # https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/DCMathSegmentMaker/src/DCMathSegmentMaker.h
    # ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreator;         //<! mdt rio ontrack creator
    # ToolHandle<IMdtDriftCircleOnTrackCreator> m_mdtCreatorT0;       //<! mdt rio ontrack creator
    # ToolHandle<IMuonClusterOnTrackCreator>    m_clusterCreator;     //<! cluster rio ontrack creator
    # ToolHandle<IMuonCompetingClustersOnTrackCreator> m_compClusterCreator;   //<! competing clusters rio ontrack creator
    # ToolHandle<MuonEDMPrinterTool>            m_printer;         //<! printer helper tool
    # ServiceHandle<IMuonEDMHelperSvc>          m_edmHelperSvc;          //<! printer helper tool
    # ToolHandle<IMdtSegmentFinder>             m_segmentFinder;   //<! segment finder tool MdtSegmentFinder
    # ToolHandle<IMuonSegmentFittingTool>       m_segmentFitter;   //<! segment fitting tool
    # ToolHandle<IMuonSegmentSelectionTool>     m_segmentSelectionTool; //<! segment selection tool
    # ToolHandle<IDCSLFitProvider>              m_dcslFitProvider;

    result=ComponentAccumulator()
    
    beamType       = flags.Beam.Type
    doSegmentT0Fit = flags.Muon.doSegmentT0Fit
    updateSegmentSecondCoordinate = flags.Muon.updateSegmentSecondCoordinate
    enableCurvedSegmentFinding = flags.Muon.enableCurvedSegmentFinding
        
    kwargs.setdefault("RefitSegment", True)
    kwargs.setdefault("AssumePointingPhi", beamType != 'cosmics')
    kwargs.setdefault("OutputFittedT0", True)

    kwargs.setdefault("DCFitProvider", result.popToolsAndMerge(MdtSegmentT0FitterCfg(flags)))
    #kwargs.setdefault("CurvedErrorScaling", False)
    kwargs.setdefault("UsePreciseError", True)
    kwargs.setdefault("SinAngleCut", 0.4)

    if (beamType == 'singlebeam' or beamType == 'cosmics'): 
        kwargs.setdefault("SinAngleCut", 0.9)
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)
        kwargs.setdefault("CurvedErrorScaling", False)
    elif not flags.Input.isMC: # collisions real data 
        kwargs.setdefault("AddUnassociatedPhiHits", True)
        kwargs.setdefault("RecoverBadRpcCabling", True)

    if doSegmentT0Fit:
        mdt_dcot_CA = MdtDriftCircleOnTrackCreatorCfg(flags, name="MdtDriftCircleOnTrackCreatorAdjustableT0", TimingMode=3, \
                   DoTofCorrection=True, TimeWindowSetting=MdtCalibWindowNumber('Collision_data'))
        kwargs.setdefault("MdtCreatorT0", result.getPrimaryAndMerge(mdt_dcot_CA)) # TODO - is this correct? 
        mdt_math_segment_finder = result.popToolsAndMerge(MdtMathSegmentFinderCfg(flags, doSegmentT0Fit=True))
    else:
        mdt_math_segment_finder = result.popToolsAndMerge(MdtMathSegmentFinderCfg(flags))
        
    result.addPublicTool(mdt_math_segment_finder)
    kwargs.setdefault("MdtSegmentFinder", mdt_math_segment_finder )

    if updateSegmentSecondCoordinate:
        kwargs.setdefault("UpdatePhiUsingPhiHits",True)

    if enableCurvedSegmentFinding:
        kwargs.setdefault("CurvedErrorScaling", False)
        kwargs.setdefault("PreciseErrorScale", 1)
        kwargs.setdefault("UsePreciseError", True)
    
    # Now stuff that wasn't explicitly configured before.
    
    acc = MuonStationIntersectSvcCfg(flags)
    muon_station_intersect_svc = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MuonStationIntersectSvc", muon_station_intersect_svc)

    acc=MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtCreator", result.getPrimaryAndMerge(acc))
    
    acc=MuonClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.addPublicTool(muon_cluster_creator)
    # FIXME - declare property  for muon_cluster_creator is missing from the tool.
    result.merge(acc)
    
    acc = TriggerChamberClusterOnTrackCreatorCfg(flags)
    muon_comp_cluster_creator =  acc.getPrimary()
    # result.addPublicTool(muon_comp_cluster_creator)
    result.merge(acc)
    
    kwargs.setdefault("MuonCompetingClustersCreator", muon_comp_cluster_creator)
    acc =  MuonSegmentFittingToolCfg(flags, name="MuonSegmentFittingTool")
    segment_fitter=acc.getPrimary()
    result.addPublicTool(segment_fitter)
    
    kwargs.setdefault("EDMPrinter", MuonEDMPrinterTool(flags) )

    result.merge(acc)    
    kwargs.setdefault("SegmentFitter", segment_fitter)
    
    segment_selector =  CompFactory.Muon.MuonSegmentSelectionTool()
    result.addPublicTool(segment_selector)
    kwargs.setdefault("SegmentSelector", segment_selector)
    
    # Needs MdtCondDbData
    acc = MdtCondDbAlgCfg(flags)
    result.merge(acc)
    
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    if flags.Common.isOnline:
        kwargs.setdefault('MdtCondKey', '')

    dc_segment_maker = CompFactory.Muon.DCMathSegmentMaker(**kwargs)
    result.setPrivateTools(dc_segment_maker)
    return result


def MuonPatternSegmentMakerCfg(flags, **kwargs):
    Muon__MuonPatternSegmentMaker=CompFactory.Muon.MuonPatternSegmentMaker
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MdtDriftCircleOnTrackCreatorCfg, MuonClusterOnTrackCreatorCfg
    # Taken from https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L49
    
    # Tool has the following subtools:
    # DCMathSegmentMaker, MdtDriftCircleOnTrackCreator, MuonClusterOnTrackCreator, MuonEDMPrinterTool
    result=ComponentAccumulator()

    if "MdtCreator" not in kwargs: 
        # on data configure a MdtDriftCircleOnTrackCreator for the segment finding with reduced errors
        # when using the t0 refit enlarge the time window
        if not flags.Input.isMC and flags.Beam.Type == 'collisions':
            if flags.Muon.doSegmentT0Fit:
                timeWindowSetting = MdtCalibWindowNumber('Collision_t0fit')
            else:
                timeWindowSetting = MdtCalibWindowNumber('Collision_data')
            acc = MdtDriftCircleOnTrackCreatorCfg(flags, name="MdtDriftCircleOnTrackCreator_NoTubeHits", CreateTubeHit = False, TimeWindowSetting = timeWindowSetting)   
        else:
            # I think we need to configure a 'default' version of the MdtDriftCircleOnTrackCreator here
            acc=MdtDriftCircleOnTrackCreatorCfg(flags)

        kwargs.setdefault('MdtCreator', result.getPrimaryAndMerge(acc))
    #else:
    # TODO work out what to do here
    
    acc = MuonClusterOnTrackCreatorCfg(flags)  
    kwargs.setdefault('ClusterCreator', acc.getPrimary())
    result.merge(acc)
    
    # Other dependencies:
    # EDM printer tool

    acc = DCMathSegmentMakerCfg(flags,name="DCMathSegmentMaker")
    segment_maker = acc.getPrimary()
    acc.addPublicTool(segment_maker)
    kwargs.setdefault('SegmentMaker', segment_maker)
    result.merge(acc)

    if flags.Beam.Type == 'cosmics':
        kwargs.setdefault("AngleCutPhi", 1e9)
        kwargs.setdefault("DropDistance", 100000000.)
    result.setPrivateTools(Muon__MuonPatternSegmentMaker("MuonPatternSegmentMaker", **kwargs))
    return result
    
def CscAlignmentTool(flags, **kwargs):

    CscAlignmentTool=CompFactory.CscAlignmentTool
    etaposAlignConsts = [ # 1st, 2nd, 3rd, 4th layer 
        0.0,   -0.0902347,   -0.0984321,    -0.141175, #sector -16
        0.0,    -0.166412,    -0.150399,     -0.18592, #sector -15
        0.0,   -0.0544449,     0.101448,   -0.0433321, #sector -14
        0.0,    -0.047769,    -0.156247,   -0.0854826, #sector -13
        0.0,   -0.0970101,    -0.227578,    -0.301079, #sector -12
        0.0,    -0.152912,    -0.365218,    -0.314042, #sector -11
        0.0,    0.0338696,    -0.264766,    -0.224567, #sector -10
        0.0,    -0.107372,     -0.16099,     -0.13508, #sector -9
        0.0,   -0.0663979,   -0.0312645,    -0.130711, #sector -8
        0.0,    -0.249974,    -0.269372,    -0.353648, #sector -7
        0.0,    -0.194175,    0.0238348,    -0.188433, #sector -6
        0.0,    0.0977346,    0.0492461,     0.150434, #sector -5
        0.0,    0.0444637,   -0.0871214,   -0.0627541, #sector -4
        0.0,    0.0347245,   -0.0226186,   -0.0826478, #sector -3
        0.0,  -0.00408879,   -0.0638005,    -0.206868, #sector -2
        0.0,    0.0331328,    0.0402103,     0.118611, #sector -1
        0.0,   -0.0634537,   -0.0516743,    0.0045364, #sector 1
        0.0,   -0.0957718,    -0.235246,    -0.439811, #sector 2
        0.0,    -0.163504,   -0.0129115,   0.00140143, #sector 3
        0.0,    -0.182903,   -0.0503858,    -0.207799, #sector 4
        0.0,   -0.0970927,            0,     0.124744, #sector 5
        0.0,   -0.0169602,   -0.0527447,    0.0319154, #sector 6
        0.0,    -0.176499,    -0.196542,   -0.0846979, #sector 7
        0.0,    0.0589994,    0.0131431,    0.0461769, #sector 8
        0.0,   -0.0237507,   -0.0307316,     0.144429, #sector 9
        0.0,   -0.0144483,   -0.0328234,  -0.00553684, #sector 10
        0.0,     0.113188,    -0.176182,    0.0635706, #sector 11
        0.0,    -0.109776,    -0.727705,    -0.756824, #sector 12
        0.0,   -0.0553061,    -0.056162,   -0.0336955, #sector 13
        0.0,    -0.070077,    -0.127203,    -0.140082, #sector 14
        0.0,   -0.0844368,   -0.0735331,    -0.016667, #sector 15
        0.0,    0.0426443,    -0.153659,    -0.129711, #sector 16
    ]

    phiposAlignConsts = [ # 1st, 2nd, 3rd, 4th layer 
        0.0, 0.0, 0.0, 0.0, #-16
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-14
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-12
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-10
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-8
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-6
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, #-4
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #-2
        0.0, 0.0, 0.0, 0.0, #-1  
        0.0, 0.0, 0.0, 0.0, # sector =1 and above...
        0.0, 0.0, 0.0, 0.0, #2
        0.0, 0.0, 0.0, 0.0, #3
        0.0, 0.0, 0.0, 0.0, #4
        0.0, 0.0, 0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, #6
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #8
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #10
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #12
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0, #14
        0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 0.0  #16
    ]
    kwargs.setdefault("phiposAlignConsts", phiposAlignConsts)
    kwargs.setdefault("etaposAlignConsts", etaposAlignConsts)
    kwargs.setdefault("useAlignment", flags.Input.isMC is False and flags.Muon.Align.UseILines is False  )

    return CscAlignmentTool(**kwargs)
    

def QratCscClusterFitterCfg(flags, **kwargs):
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/CscTools.py#L151
    QratCscClusterFitter=CompFactory.QratCscClusterFitter
    result=ComponentAccumulator()
    qratcor_css_eta = [
        0.000000, 0.000000, 0.000000, 0.000000, 0.0699381, 0.178291, 0.271303, 0.345611, 
        0.400738, 0.452451, 0.493772, 0.526792, 0.553845, 0.580111, 0.603337, 0.624749, 
        0.646065, 0.663924, 0.681897, 0.701345, 0.716067, 0.730324, 0.743168, 0.760598, 
        0.773341, 0.786406, 0.798358, 0.811339, 0.820949, 0.832676, 0.843917, 0.85412, 
        0.863914, 0.873229, 0.88251, 0.892856, 0.903296, 0.910067, 0.917892, 0.928028, 
        0.93461, 0.943679, 0.951617, 0.958326, 0.964428, 0.97341, 0.978204, 0.987075, 
        0.994789, 1.000000
        ]
    qratcor_csl_eta = [
        0.000000, 0.000000, 0.000000, 0.000000, 0.0290177, 0.0834867, 0.188683, 0.269967, 
        0.334887, 0.393036, 0.440317, 0.481884, 0.51725, 0.545107, 0.56959, 0.592327, 
        0.614087, 0.635344, 0.655252, 0.672766, 0.690929, 0.707293, 0.722742, 0.738323, 
        0.753562, 0.76749, 0.780983, 0.79354, 0.806521, 0.818085, 0.829987, 0.841443, 
        0.853723, 0.863393, 0.87431, 0.883139, 0.892962, 0.902803, 0.911276, 0.921417, 
        0.929387, 0.938106, 0.947072, 0.954406, 0.961692, 0.97055, 0.978517, 0.985133, 
        0.992257, 1.000000
        ]
    kwargs.setdefault("qratcor_csl_eta",qratcor_csl_eta)
    kwargs.setdefault("qratcor_css_eta",qratcor_css_eta)
    csc_align_tool = CscAlignmentTool(flags)
    kwargs.setdefault("CscAlignmentTool", csc_align_tool )
    result.setPrivateTools(QratCscClusterFitter(**kwargs))
    
    return result

def CalibCscStripFitterCfg(flags, name = "CalibCscStripFitter",**kwargs):
    CalibCscStripFitter=CompFactory.CalibCscStripFitter
    from MuonConfig.MuonCalibrationConfig import CscCalibToolCfg
    result = CscCalibToolCfg(flags)
    kwargs.setdefault("cscCalibTool", result.popPrivateTools() )
    result.setPrivateTools(CalibCscStripFitter(name=name,**kwargs))
    return result

def CscClusterUtilToolCfg(flags, name='CscClusterUtilTool', **kwargs):
    CscClusterUtilTool=CompFactory.CscClusterUtilTool
    
    result = CalibCscStripFitterCfg(flags)
    kwargs.setdefault("strip_fitter", result.popPrivateTools() )
    
    acc = QratCscClusterFitterCfg(flags)
    kwargs.setdefault("precision_fitter", acc.popPrivateTools() )
    result.setPrivateTools(CscClusterUtilTool(name=name, **kwargs))
    result.merge(acc)
    return result

def CscSegmentUtilToolCfg(flags, name='CscSegmentUtilTool', **kwargs):
    CscSegmentUtilTool=CompFactory.CscSegmentUtilTool
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import CscClusterOnTrackCreatorCfg
    
    result=CscClusterOnTrackCreatorCfg(flags)
    csc_cluster_creator = result.popPrivateTools()
    kwargs.setdefault("rot_creator", csc_cluster_creator )
    
    result.setPrivateTools(CscSegmentUtilTool( name=name, **kwargs))
    return result

def Csc2dSegmentMakerCfg(flags, name= "Csc2dSegmentMaker", **kwargs):
    Csc2dSegmentMaker=CompFactory.Csc2dSegmentMaker
    result=ComponentAccumulator()
    if 'segmentTool' not in kwargs:
        acc  = CscSegmentUtilToolCfg(flags)
        csc_segment_util_tool = acc.getPrimary()
        result.addPublicTool(csc_segment_util_tool)
        kwargs.setdefault('segmentTool', csc_segment_util_tool)
        result.merge(acc)
    
    kwargs.setdefault("printerTool", MuonEDMPrinterTool(flags) )

    csc_segment_maker = Csc2dSegmentMaker(name=name, **kwargs)
    result.setPrivateTools(csc_segment_maker)
    
    return result

def Csc4dSegmentMakerCfg(flags, name= "Csc4dSegmentMaker", **kwargs):
    Csc4dSegmentMaker=CompFactory.Csc4dSegmentMaker
    
    result=ComponentAccumulator()
    acc= CscSegmentUtilToolCfg(flags)
    csc_segment_util_tool = acc.getPrimary()
    result.addPublicTool(csc_segment_util_tool)
    
    result.merge(acc)
    csc_segment_maker = Csc4dSegmentMaker(name=name, 
           segmentTool=csc_segment_util_tool) 
    result.setPrivateTools(csc_segment_maker)
    
    return result


def MooSegmentFinderCfg(flags, name='MooSegmentFinder', **kwargs):
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MooreTools.py#L99 
    
    Muon__MuonLayerHoughTool=CompFactory.Muon.MuonLayerHoughTool
    Muon__MuonSegmentCombinationCleanerTool=CompFactory.Muon.MuonSegmentCombinationCleanerTool
    Muon__MooSegmentCombinationFinder=CompFactory.Muon.MooSegmentCombinationFinder

    result=ComponentAccumulator()

    muon_curved_segment_combiner_tool = MuonCurvedSegmentCombiner(flags)
    result.addPublicTool(muon_curved_segment_combiner_tool)
    
    muon_segment_combination_cleaner_tool = Muon__MuonSegmentCombinationCleanerTool()
    result.addPublicTool(muon_segment_combination_cleaner_tool)
    
    # Use the MuonLayerHoughTool for collisions, MuonHoughPatternFinderTool for everything else.
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonRecTools.py#L428
    muon_pattern_finder_tool = Muon__MuonLayerHoughTool("MuonLayerHoughTool", DoTruth=False if flags.Muon.MuonTrigger else flags.Input.isMC) \
                              if flags.Beam.Type=="collisions" else MuonHoughPatternFinderTool(flags)
    result.addPublicTool(muon_pattern_finder_tool)
    
    acc  = MuonPatternSegmentMakerCfg(flags)
    muon_pattern_segment_maker = acc.getPrimary()
    result.merge(acc)
    result.addPublicTool(muon_pattern_segment_maker)
    
    if flags.Muon.doCSCs:
        acc = Csc2dSegmentMakerCfg(flags)
        csc_2d_segment_maker = acc.getPrimary()        
        result.merge(acc)
    
        acc = Csc4dSegmentMakerCfg(flags)
        csc_4d_segment_maker = acc.getPrimary()
        result.merge(acc)
    else:
        csc_2d_segment_maker=None
        csc_4d_segment_maker=None
    
    kwargs.setdefault('SegmentCombiner', muon_curved_segment_combiner_tool)
    kwargs.setdefault('SegmentCombinationCleaner', muon_segment_combination_cleaner_tool)
    kwargs.setdefault('HoughPatternFinder', muon_pattern_finder_tool)
    kwargs.setdefault('MdtSegmentMaker', muon_pattern_segment_maker)
    kwargs.setdefault('DoSegmentCombinations', False)
    kwargs.setdefault('DoSegmentCombinationCleaning', False)
    kwargs.setdefault('DoCscSegments', flags.Muon.doCSCs)
    kwargs.setdefault('DoMdtSegments', flags.Muon.doMDTs)
    kwargs.setdefault('Csc2dSegmentMaker', csc_2d_segment_maker)
    kwargs.setdefault('Csc4dSegmentMaker', csc_4d_segment_maker)
    kwargs.setdefault('DoSummary', flags.Muon.printSummary)

    segment_finder_tool = Muon__MooSegmentCombinationFinder(name=name, **kwargs)
    
    result.setPrivateTools(segment_finder_tool)
    return result

def MuonClusterSegmentFinderToolCfg(flags, **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonTrackToSegmentToolCfg
    result=ComponentAccumulator()
    # Won't explicitly configure MuonIdHelperSvc
    # Won't explicitly configure MuonEDMHelperSvc
    kwargs.setdefault('SegmentAmbiguityTool', result.popToolsAndMerge( MuonAmbiProcessorCfg(flags) ) ) 
    kwargs.setdefault('SLFitter', result.popToolsAndMerge( MCTBSLFitterMaterialFromTrackCfg(flags) ) ) 
    kwargs.setdefault("TrackToSegmentTool", result.popToolsAndMerge( MuonTrackToSegmentToolCfg(flags) ) )
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )
    kwargs.setdefault('TrackCleaner', result.popToolsAndMerge( MuonTrackCleanerCfg(flags) ) ) 
    kwargs.setdefault('TrackSummaryTool', result.popToolsAndMerge( MuonTrackSummaryToolCfg(flags) ) ) 

    result.setPrivateTools(CompFactory.Muon.MuonClusterSegmentFinderTool(**kwargs))
    return result

def MuonPRDSelectionToolCfg( flags, name="MuonPRDSelectionTool", **kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MuonClusterOnTrackCreatorCfg, MdtDriftCircleOnTrackCreatorCfg
    # Won't explicitly configure MuonIdHelperSvc
    result=MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtDriftCircleOnTrackCreator", result.getPrimary())
    acc = MuonClusterOnTrackCreatorCfg(flags)
    kwargs.setdefault("MuonClusterOnTrackCreator", acc.getPrimary() )
    result.merge(acc)
    # Won't explicitly configure MuonRecoValidationTool (but it would need configuration if used)
    result.setPrivateTools(CompFactory.Muon.MuonPRDSelectionTool(**kwargs))
    return result

def MuonClusterSegmentFinderCfg(flags, **kwargs):
    from MuonConfig.MuonRecToolsConfig import MuonTrackToSegmentToolCfg
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MuonClusterOnTrackCreatorCfg

    result=ComponentAccumulator()

    # Won't explicitly configure MuonIdHelperSvc
    kwargs.setdefault("MuonEDMPrinterTool", MuonEDMPrinterTool(flags) )
    # Won't explicitly configure MuonLayerHashProviderTool
    kwargs.setdefault("MuonPRDSelectionTool", result.popToolsAndMerge( MuonPRDSelectionToolCfg(flags) ) )
    kwargs.setdefault('MdtSegmentMaker', result.popToolsAndMerge( DCMathSegmentMakerCfg(flags,name="DCMathSegmentMaker") ) ) 
    # Won't explicitly configure MuonClusterizationTool
    acc = MuonClusterOnTrackCreatorCfg(flags) # This only sets a public tool at the moment, so need to it like this. FIXME
    kwargs.setdefault("MuonClusterOnTrackCreator", acc.getPrimary() )
    result.merge(acc)
    kwargs.setdefault("TrackToSegmentTool", result.popToolsAndMerge( MuonTrackToSegmentToolCfg(flags) ) )
    kwargs.setdefault('SLFitter', result.popToolsAndMerge( MCTBFitterCfg(flags, name = "SLFitter", StraightLine=True) ) ) 
    kwargs.setdefault('AmbiguityProcessor', result.popToolsAndMerge( MuonAmbiProcessorCfg(flags) ) ) 
    kwargs.setdefault('TrackCleaner', result.popToolsAndMerge( MuonTrackCleanerCfg(flags) ) ) 
    # Won't explicitly configure MuonSegmentOverlapRemovalTool (though it possibly needs it)

    from MuonConfig.MuonRecToolsConfig import MuonTrackToSegmentToolCfg
    acc = MuonTrackToSegmentToolCfg(flags)
    kwargs.setdefault( "TrackToSegmentTool", result.popToolsAndMerge(acc))

    result.setPrivateTools(CompFactory.Muon.MuonClusterSegmentFinder(**kwargs))
    return result

def MooSegmentFinderAlgCfg(flags, name = "MuonSegmentMaker",  **kwargs):
    # This is based on https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonReconstruction/MuonRecExample/python/MuonStandalone.py#L113
    MooSegmentFinderAlg=CompFactory.MooSegmentFinderAlg
    result=ComponentAccumulator()
    
    if 'SegmentFinder' not in kwargs:
        # Let's not call all of this twice if we don't have too...
        acc  = MooSegmentFinderCfg(flags)
        segment_finder_tool=(acc.popPrivateTools())
        result.addPublicTool(segment_finder_tool)
        result.merge(acc)
        kwargs.setdefault('SegmentFinder', segment_finder_tool)

    if 'MuonClusterSegmentFinderTool' not in kwargs:
        # Let's not call all of this twice if we don't have too...
        acc = MuonClusterSegmentFinderCfg(flags)
        muon_cluster_segment_finder=acc.getPrimary()
        result.merge(acc)
        kwargs.setdefault('MuonClusterSegmentFinderTool', muon_cluster_segment_finder)
        
    kwargs.setdefault('UseCSC', flags.Muon.doCSCs)
    kwargs.setdefault('UseMDT', flags.Muon.doMDTs)
    kwargs.setdefault('UseRPC', flags.Muon.doRPCs)
    kwargs.setdefault('UseTGC', flags.Muon.doTGCs)
    kwargs.setdefault('UseTGCPriorBC', flags.Muon.doTGCs and flags.Muon.useTGCPriorNextBC)
    kwargs.setdefault('UseTGCNextBC', flags.Muon.doTGCs and flags.Muon.useTGCPriorNextBC)
    kwargs.setdefault('doTGCClust', flags.Muon.doTGCClusterSegmentFinding)
    kwargs.setdefault('doRPCClust', flags.Muon.doRPCClusterSegmentFinding)
    # When reading ESDs, where prior/next BC TGCs are merged, just retrieve that.
    # FIXME - this really shouldn't be set here! 
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
        
    kwargs.setdefault('MuonSegmentOutputLocation', "ThirdChainSegments" if flags.Muon.segmentOrigin=="TruthTracking" else "TrackMuonSegments")
    if flags.Beam.Type != 'collisions':
        kwargs.setdefault("Key_MuonLayerHoughToolHoughDataPerSectorVec", "")

    moo_segment_finder_alg = MooSegmentFinderAlg( name=name, **kwargs )
    moo_segment_finder_alg.Cardinality=10
    result.addEventAlgo( moo_segment_finder_alg )
        
    return result

def MooSegmentFinderAlg_NCBCfg(flags, name = "MuonSegmentMaker_NCB", **kwargs):
    result = ComponentAccumulator()

    # Configure NCB MooSegmentFinder
    if flags.Detector.GeometryCSC:
        acc = CscSegmentUtilToolCfg(flags, name='CscSegmentUtilTool_NCB', TightenChi2 = False, IPconstraint=False)
        csc_segment_util_tool = acc.getPrimary()
        result.merge(acc)
        
        acc = Csc2dSegmentMakerCfg(flags, name='Csc2dSegmentMaker_NCB', segmentTool=csc_segment_util_tool)
        csc_2d_segment_maker = acc.getPrimary() 
        result.merge(acc)
        
        acc = Csc4dSegmentMakerCfg(flags, name='Csc4dSegmentMaker_NCB', segmentTool=csc_segment_util_tool)
        csc_4d_segment_maker = acc.getPrimary()
        result.merge(acc)
    else:
        csc_2d_segment_maker = ""
        csc_4d_segment_maker = ""

    acc  = MooSegmentFinderCfg(flags, name='MooSegmentFinder_NCB', Csc2dSegmentMaker=csc_2d_segment_maker, 
                               Csc4dSegmentMaker=csc_4d_segment_maker, 
                               DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False)
    segment_finder_tool=(acc.popPrivateTools())
    result.addPublicTool(segment_finder_tool)
    result.merge(acc)

    kwargs.setdefault('SegmentFinder', segment_finder_tool)

    # Now set other NCB properties
    kwargs.setdefault('MuonPatternCombinationLocation', "NCB_MuonHoughPatternCombinations" )
    kwargs.setdefault('MuonSegmentOutputLocation', "NCB_TrackMuonSegments" )
    kwargs.setdefault('Key_MuonLayerHoughToolHoughDataPerSectorVec', '')
    kwargs.setdefault('UseCSC', flags.Muon.doCSCs)
    kwargs.setdefault('UseMDT', False)
    kwargs.setdefault('UseRPC', False)
    kwargs.setdefault('UseTGC', False)
    kwargs.setdefault('UseTGCPriorBC', False)
    kwargs.setdefault('UseTGCNextBC', False)
    kwargs.setdefault('doTGCClust', False)
    kwargs.setdefault('doRPCClust', False)

    acc = MooSegmentFinderAlgCfg(flags, name=name, **kwargs)
    result.merge(acc)
    return result

def MuonLayerHoughToolCfg(flags, name = "MuonLayerHoughTool" , **kwargs):
    result = ComponentAccumulator()
    if flags.Muon.MuonTrigger:
        kwargs.setdefault("DoTruth", False)
    else:
        kwargs.setdefault("DoTruth", flags.Input.isMC)
    layer_hough_tool = CompFactory.Muon.MuonLayerHoughTool(name, **kwargs)
    result.addPublicTool(layer_hough_tool,primary=True)
    return result

def MuonLayerHoughAlgCfg(flags, name = "MuonLayerHoughAlg", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    kwargs.setdefault("CscPrepDataContainer", "CSC_Clusters" if flags.Muon.doCSCs else "")
    kwargs.setdefault("sTgcPrepDataContainer", "STGC_Measurements" if flags.Muon.dosTGCs else "")
    kwargs.setdefault('TgcPrepDataContainer', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')
    kwargs.setdefault("MMPrepDataContainer", "MM_Measurements" if flags.Muon.doMicromegas else "")
    kwargs.setdefault("PrintSummary", flags.Muon.printSummary)
    acc = MuonLayerHoughToolCfg(flags,name = "MuonLayerHoughTool")
    hough_tool = acc.getPrimary()
    kwargs.setdefault("MuonLayerScanTool", hough_tool)
    result.merge(acc)
    the_alg = CompFactory.MuonLayerHoughAlg(name = name,
                                           **kwargs)
    result.addEventAlgo( the_alg )         
    return result


def MuonPatternCalibrationCfg(flags, name="MuonPatternCalibration", **kwargs):
    from MuonConfig.MuonRIO_OnTrackCreatorConfig import MuonClusterOnTrackCreatorCfg, MdtDriftCircleOnTrackCreatorCfg
    result = MdtDriftCircleOnTrackCreatorCfg(flags)
    kwargs.setdefault("MdtCreator", result.popPrivateTools())
    kwargs.setdefault('ClusterCreator', result.popToolsAndMerge(MuonClusterOnTrackCreatorCfg(flags)))
    kwargs.setdefault("Printer", MuonEDMPrinterTool(flags) )
    # Won't explicitly configure MuonIdHelperSvc
    result.setPrivateTools( CompFactory.Muon.MuonPatternCalibration(name, **kwargs) )
    return result

def MuonSegmentFinderAlgCfg(flags, name="MuonSegmentMaker", **kwargs):
    result = ComponentAccumulator()
    
    acc = MuonStraightLineExtrapolatorCfg(flags)
    extrapolator = acc.getPrimary()
    result.addPublicTool(extrapolator)
    result.merge(acc)

    acc = MCTBSLFitterMaterialFromTrackCfg(flags)
    slfitter = acc.getPrimary()
    result.merge(acc)

    acc = MuonTrackCleanerCfg(flags, "MuonTrackCleaner_seg",
                              PullCut = 3,
                              PullCutPhi = 3,
                              UseSLFit = True,
                              Extrapolator = extrapolator,
                              Fitter = slfitter)
   
    Cleaner = acc.getPrimary()
    result.merge(acc)
    
    acc = MuonClusterSegmentFinderToolCfg(flags, name ="MuonClusterSegmentFinderTool", 
                                          TrackCleaner = Cleaner)
    SegmentFinder = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MuonClusterSegmentFinderTool", SegmentFinder)
  
    acc = MuonClusterSegmentFinderCfg(flags, name = "MuonClusterSegmentFinder")
    segment_finder = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MuonClusterSegmentFinder", segment_finder)

    acc = DCMathSegmentMakerCfg(flags,name="DCMathSegmentMaker")
    segment_maker = acc.getPrimary()
    acc.addPublicTool(segment_maker)
    kwargs.setdefault('SegmentMaker', segment_maker)
    result.merge(acc)
    
    kwargs.setdefault("PrintSummary", flags.Muon.printSummary)
    kwargs.setdefault("SegmentCollectionName",  "TrackMuonSegments" if flags.Muon.segmentOrigin != "TruthTracking" else "ThirdChainSegments")
    
    acc  = MuonPatternSegmentMakerCfg(flags)
    muon_pattern_segment_maker = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MuonPatternSegmentMaker",muon_pattern_segment_maker)
    
    ### we check whether the layout contains any CSC chamber and if yes, we check that the user also wants to use the CSCs in reconstruction    
    if flags.Muon.doCSCs:
        acc = Csc2dSegmentMakerCfg(flags)
        csc_2d_segment_maker = acc.getPrimary()
        result.merge(acc)
        kwargs.setdefault("Csc2dSegmentMaker", csc_2d_segment_maker)

        acc = Csc4dSegmentMakerCfg(flags)
        csc_4d_segment_maker = acc.getPrimary()
        result.merge(acc)
        kwargs.setdefault("Csc4dSegmentMaker", csc_4d_segment_maker)
    else:
        kwargs.setdefault("Csc2dSegmentMaker", "")
        kwargs.setdefault("Csc4dSegmentMaker", "")
        kwargs.setdefault("CSC_clusterkey", "")
    kwargs.setdefault('TGC_PRDs', 'TGC_MeasurementsAllBCs' if not flags.Muon.useTGCPriorNextBC else 'TGC_Measurements')

    # for test purposes allow parallel running of truth segment finding and new segment finder
    kwargs.setdefault('MuonPatternCalibration', result.popToolsAndMerge( MuonPatternCalibrationCfg(flags) ) )
    the_alg = CompFactory.MuonSegmentFinderAlg( name,
                                               **kwargs)
                                                       
    result.addEventAlgo(the_alg)
    return result    

def MuonSegmentFilterAlgCfg(flags, name="MuonSegmentFilterAlg", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("SegmentCollectionName", "TrackMuonSegments")
    ## The output key of this alg os per default FilteredMuonSegments
    kwargs.setdefault("FilteredCollectionName", "TrackMuonSegmentsEMEO")
    the_alg =  CompFactory.MuonSegmentFilterAlg(name, **kwargs)
    result.addEventAlgo(the_alg)
    return result

def MuonSegmentFindingCfg(flags, cardinality=1):
    # Set up some general stuff needed by muon reconstruction
    
    result = ComponentAccumulator()
    
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg 
    result.merge( MuonGeoModelCfg(flags) )

    Muon__MuonEDMHelperSvc=CompFactory.Muon.MuonEDMHelperSvc
    muon_edm_helper_svc = Muon__MuonEDMHelperSvc("MuonEDMHelperSvc")
    result.addService( muon_edm_helper_svc )
    
    if flags.Input.Format is Format.BS:
        from MuonConfig.MuonBytestreamDecodeConfig import MuonByteStreamDecodersCfg
        result.merge( MuonByteStreamDecodersCfg(flags) )

    if flags.Input.Format is Format.BS or 'StreamRDO' in flags.Input.ProcessingTags:
        from MuonConfig.MuonRdoDecodeConfig import MuonRDOtoPRDConvertorsCfg
        result.merge( MuonRDOtoPRDConvertorsCfg(flags) )

    # We need to add two algorithms - one for normal collisions, one for NCB
    # result.merge( MooSegmentFinderAlgCfg(flags, Cardinality=cardinality) )
    ### For the moment to not use the run 3 segment maker algorithm as we need
    ### to migrate the TgcPrepData first. In any case, let's keep the next two lines
    ### commented for the moment!!!
    result.merge(MuonLayerHoughAlgCfg(flags))
    result.merge(MuonSegmentFinderAlgCfg(flags, name="MuonSegmentFinderAlg"))
  
    result.merge(MooSegmentFinderAlg_NCBCfg(flags, Cardinality=cardinality))
    
    if flags.Muon.runCommissioningChain:
        result.merge(MuonSegmentFilterAlgCfg(flags))

    result.addEventAlgo(CompFactory.xAODMaker.MuonSegmentCnvAlg("MuonSegmentCnvAlg"))

    return result

if __name__=="__main__":
    # To run this, do e.g. 
    # python -m MuonConfig.MuonSegmentFindingConfig --run --threads=1
    from MuonConfig.MuonConfigUtils import SetupMuonStandaloneArguments, SetupMuonStandaloneConfigFlags, SetupMuonStandaloneOutput, SetupMuonStandaloneCA

    args = SetupMuonStandaloneArguments()
    ConfigFlags = SetupMuonStandaloneConfigFlags(args)
    cfg = SetupMuonStandaloneCA(args,ConfigFlags)

    # Run the actual test.
    acc = MuonSegmentFindingCfg(ConfigFlags, cardinality=args.threads)
    cfg.merge(acc)
    
    if args.threads>1 and args.forceclone:
        from AthenaCommon.Logging import log
        log.info('Forcing segment finding cardinality to be equal to '+str(args.threads))
        # We want to force the algorithms to run in parallel (eventually the algorithm will be marked as cloneable in the source code)
        AlgResourcePool=CompFactory.AlgResourcePool
        cfg.addService(AlgResourcePool( OverrideUnClonable=True ) )
        segment_finder = acc.getPrimary()
        segment_finder.Cardinality=args.threads

    # This is a temporary fix - it should go someplace central as it replaces the functionality of addInputRename from here:
    # https://gitlab.cern.ch/atlas/athena/blob/master/Control/SGComps/python/AddressRemappingSvc.py
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    pps = ProxyProviderSvc()
    ars=AddressRemappingSvc()
    pps.ProviderNames += [ 'AddressRemappingSvc' ]
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % ("Trk::SegmentCollection", "TrackMuonSegments", "TrackMuonSegments_old") ]
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % ("Trk::SegmentCollection", "NCB_TrackMuonSegments", "NCB_TrackMuonSegments_old") ]

    cfg.addService(pps)
    cfg.addService(ars)

    itemsToRecord = ["Trk::SegmentCollection#TrackMuonSegments", "Trk::SegmentCollection#NCB_TrackMuonSegments"]
    SetupMuonStandaloneOutput(cfg, ConfigFlags, itemsToRecord)

    # cfg.getService("StoreGateSvc").Dump = True
    cfg.printConfig()
    f=open("MuonSegmentFinding.pkl","wb")
    cfg.store(f)
    f.close()

    if args.run:
        sc = cfg.run(20)
        if not sc.isSuccess():
            import sys
            sys.exit("Execution failed")
