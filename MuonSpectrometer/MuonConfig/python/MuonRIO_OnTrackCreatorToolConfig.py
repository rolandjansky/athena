# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
from MuonConfig.MuonCalibrationConfig import MdtCalibrationToolCfg, MdtCalibrationDbToolCfg

### Simple function holding connecting names to the different calibration window options
# The window values themselves are defined in C++ in MdtCalibSvc/MdtCalibrationSvcSettings.h
# Author: Mike Flowerdew <michael.flowerdew@cern.ch>

__mdtCalibWindows = ['User',
                     'Default', ## 1000,2000
                     'Collision_G4', ## 20,30
                     'Collision_data', ## 10,30
                     'Collision_t0fit', ## 50,100
                     ]

def MdtCalibWindowNumber(name):
    """Returns index number corresponding to the calibration window name.
    This will throw a ValueError if name is not in the list.
    """
    return __mdtCalibWindows.index(name)

def TriggerChamberClusterOnTrackCreatorCfg(flags, name="TriggerChamberClusterOnTrackCreator", **kwargs):
    result=ComponentAccumulator()
    acc =  MuonClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("ClusterCreator", muon_cluster_creator)
    result.setPrivateTools(CompFactory.Muon.TriggerChamberClusterOnTrackCreator(name, **kwargs))
    return result

def CscClusterOnTrackCreatorCfg(flags,name="CscClusterOnTrackCreator", **kwargs):
    from MuonConfig.MuonSegmentFindingConfig import QratCscClusterFitterCfg, CscClusterUtilToolCfg, CalibCscStripFitterCfg

    result=ComponentAccumulator()    
    acc = QratCscClusterFitterCfg(flags)
    qrat = acc.popPrivateTools()
    result.merge(acc)
    kwargs.setdefault("CscClusterFitter", qrat )
    
    acc = CalibCscStripFitterCfg(flags)
    strip_fitter = acc.popPrivateTools()
    result.merge(acc)
    kwargs.setdefault("CscStripFitter", strip_fitter)
    
    acc = CscClusterUtilToolCfg(flags)
    cluster_util_tool = acc.popPrivateTools()
    kwargs.setdefault("CscClusterUtilTool", cluster_util_tool )
    result.merge(acc)
    
    if not flags.Input.isMC: # collisions real data or simulated first data
        # scale CSC and hit errors 
        kwargs.setdefault("ErrorScalerBeta", 0.070 )

    result.setPrivateTools(CompFactory.Muon.CscClusterOnTrackCreator(name,**kwargs))
    
    return result


def MdtDriftCircleOnTrackCreatorCfg(flags,name="MdtDriftCircleOnTrackCreator", **kwargs):
    result=ComponentAccumulator()
    
    # setup dependencies missing in C++. TODO: fix in C++
    #
    # acc = MdtCalibrationDbSvcCfg(flags)
    # result.merge(acc)
    
    kwargs.setdefault("CalibrationDbTool", result.popToolsAndMerge( MdtCalibrationDbToolCfg(flags)))
    kwargs.setdefault("CalibrationTool", result.popToolsAndMerge( MdtCalibrationToolCfg(flags)) )

    kwargs.setdefault("DoMagneticFieldCorrection", flags.Muon.Calib.correctMdtRtForBField)
    kwargs.setdefault("DoWireSag", flags.Muon.useWireSagCorrections)
    kwargs.setdefault("DoSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)

    if flags.Beam.Type in [BeamType.Cosmics, BeamType.SingleBeam]:
        kwargs.setdefault("DoTofCorrection", False)
        kwargs.setdefault("DoFixedError", True)
        kwargs.setdefault("TimingMode", 1)
        kwargs.setdefault("UseParametrisedError", True)

    else: # collisions simulation/data settings
        kwargs.setdefault("DoTofCorrection", True)
        kwargs.setdefault("DoFixedError", False)
        kwargs.setdefault("DoErrorScaling", False)
        kwargs.setdefault("TimeWindowSetting", MdtCalibWindowNumber('Collision_data'))  # MJW: should this be Collision_G4 ???
        kwargs.setdefault("UseParametrisedError", False)

        if not flags.Input.isMC : 
            kwargs.setdefault("CreateTubeHit", True)  # BroadErrors
            kwargs.setdefault("UseLooseErrors", flags.Muon.useLooseErrorTuning)  # LooseErrors on data                          
    
    kwargs.setdefault("IsMC", flags.Input.isMC)

    result.setPrivateTools(CompFactory.Muon.MdtDriftCircleOnTrackCreator(name, WasConfigured=True, **kwargs))
    return result
    
def MuonClusterOnTrackCreatorCfg(flags,name="MuonClusterOnTrackCreator", **kwargs):
    result=ComponentAccumulator()
    if not flags.Input.isMC: # collisions real data or simulated first data
        # scale TGC eta hit errors as long as TGC eta are not well aligned
        kwargs.setdefault("DoFixedErrorTgcEta", True)
        kwargs.setdefault("FixedErrorTgcEta", 15.)
    else:
        kwargs.setdefault("DoFixedErrorTgcEta", False) # This is ONLY to make the tool configured. Real solution is to use default name.
    
    muon_cluster_rot_creator = CompFactory.Muon.MuonClusterOnTrackCreator(name, **kwargs)
    result.setPrivateTools(muon_cluster_rot_creator)
    return result

def MMClusterOnTrackCreatorCfg(flags,name="MMClusterOnTrackCreator", **kwargs):
    result = ComponentAccumulator()
    from MuonConfig.MuonCalibrationConfig import NSWCalibToolCfg
    from MuonConfig.MuonRecToolsConfig import SimpleMMClusterBuilderToolCfg
    kwargs.setdefault("NSWCalibTool", result.popToolsAndMerge(NSWCalibToolCfg(flags)))
    kwargs.setdefault("SimpleMMClusterBuilder", result.popToolsAndMerge(SimpleMMClusterBuilderToolCfg(flags)))  
    the_tool = CompFactory.Muon.MMClusterOnTrackCreator(name, **kwargs)
    result.setPrivateTools(the_tool)
    return result