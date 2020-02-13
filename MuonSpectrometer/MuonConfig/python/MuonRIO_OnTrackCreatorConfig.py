# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MuonConfig.MuonCalibConfig import MdtCalibrationToolCfg, MdtCalibrationDbToolCfg
from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber # TODO should maybe shift this elsewhere?
Muon__MdtDriftCircleOnTrackCreator=CompFactory.Muon__MdtDriftCircleOnTrackCreator
Muon__CscClusterOnTrackCreator, Muon__MuonClusterOnTrackCreator=CompFactory.getComps("Muon__CscClusterOnTrackCreator","Muon__MuonClusterOnTrackCreator",)
Trk__RIO_OnTrackCreator=CompFactory.Trk__RIO_OnTrackCreator
Muon__TriggerChamberClusterOnTrackCreator=CompFactory.Muon__TriggerChamberClusterOnTrackCreator

def TriggerChamberClusterOnTrackCreatorCfg(flags, name="TriggerChamberClusterOnTrackCreator", **kwargs):
    result=ComponentAccumulator()
    acc =  MuonClusterOnTrackCreatorCfg(flags)
    muon_cluster_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("ClusterCreator", muon_cluster_creator)
    result.setPrivateTools(Muon__TriggerChamberClusterOnTrackCreator(name, **kwargs))
    return result

def CscClusterOnTrackCreatorCfg(flags,name="CscClusterOnTrackCreator", **kwargs):
    from MuonConfig.MuonSegmentFindingConfig import QratCscClusterFitterCfg, CscClusterUtilToolCfg, CalibCscStripFitterCfg

    result=ComponentAccumulator()    
    acc = QratCscClusterFitterCfg(flags)
    qrat = acc.getPrimary()
    result.addPublicTool(qrat)
    result.merge(acc)
    kwargs.setdefault("CscClusterFitter", qrat )
    
    acc = CalibCscStripFitterCfg(flags)
    strip_fitter = acc.getPrimary()
    result.addPublicTool(strip_fitter)
    result.merge(acc)
    kwargs.setdefault("CscStripFitter", strip_fitter)
    
    acc = CscClusterUtilToolCfg(flags)
    cluster_util_tool = acc.getPrimary()
    kwargs.setdefault("CscClusterUtilTool", cluster_util_tool )
    result.addPublicTool(cluster_util_tool)
    result.merge(acc)
    
    if not flags.Input.isMC: # collisions real data or simulated first data
        # scale CSC and hit errors 
        kwargs.setdefault("ErrorScalerBeta", 0.070 )

    csc_cluster_creator = Muon__CscClusterOnTrackCreator(name,**kwargs)
    result.addPublicTool(csc_cluster_creator, primary=True)
    
    return result


def MdtDriftCircleOnTrackCreatorCfg(flags,name="MdtDriftCircleOnTrackCreator", **kwargs):
    result=ComponentAccumulator()
    
    # setup dependencies missing in C++. TODO: fix in C++
    #
    # acc = MdtCalibrationDbSvcCfg(flags)
    # result.merge(acc)
    
    acc = MdtCalibrationDbToolCfg(flags)
    mdt_calibibration_db_tool = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CalibrationDbTool", mdt_calibibration_db_tool)

    acc = MdtCalibrationToolCfg(flags)
    mdt_calibibration_tool = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("CalibrationTool", mdt_calibibration_tool)

    kwargs.setdefault("DoMagneticFieldCorrection", flags.Muon.Calib.correctMdtRtForBField)
    kwargs.setdefault("DoWireSag", flags.Muon.useWireSagCorrections)
    kwargs.setdefault("DoSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)

    if flags.Beam.Type== 'cosmics' or flags.Beam.Type == 'singlebeam' :
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

        if not flags.Input.isMC : 
            kwargs.setdefault("CreateTubeHit", True)  # BroadErrors
            kwargs.setdefault("UseLooseErrors", flags.Muon.useLooseErrorTuning)  # LooseErrors on data                          
    
    kwargs.setdefault("IsMC", flags.Input.isMC)
                 
    result.addPublicTool(Muon__MdtDriftCircleOnTrackCreator(name, **kwargs),primary=True)
    return result
    
def MuonClusterOnTrackCreatorCfg(flags,name="MuonClusterOnTrackCreator", **kwargs):
    result=ComponentAccumulator()
    if not flags.Input.isMC: # collisions real data or simulated first data
        # scale TGC eta hit errors as long as TGC eta are not well aligned
        kwargs.setdefault("DoFixedErrorTgcEta", True)
        kwargs.setdefault("FixedErrorTgcEta", 15.)

    # TODO Fixme - the cxx class retrieves public MuonIdHelperTool ... should be private / service.
    
    muon_cluster_rot_creator = Muon__MuonClusterOnTrackCreator(name, **kwargs)
    result.addPublicTool(muon_cluster_rot_creator, primary=True)
    return result

    
# default RIO_OnTrackCreator for muons
# make a dedicated class to delay instantiation of the muon RIO_OnTrack creators members
# class MuonRotCreator(Trk__RIO_OnTrackCreator,ConfiguredBase):
#     __slots__ = ()
#
#     def __init__(self,name="MuonRotCreator",**kwargs):
#         self.applyUserDefaults(kwargs,name)
#         kwargs.setdefault("ToolMuonDriftCircle", "MdtDriftCircleOnTrackCreator")
#         kwargs.setdefault("ToolMuonCluster", "MuonClusterOnTrackCreator")
#         kwargs.setdefault("Mode", 'muon' )
#         super(MuonRotCreator,self).__init__(name,**kwargs)
# end of class MuonRotCreator

def MuonRotCreatorCfg(flags, name="MuonRotCreator", **kwargs):
    result=ComponentAccumulator()
    
    acc=MdtDriftCircleOnTrackCreatorCfg(flags)
    mdt_rot_creator=acc.getPrimary()
    result.merge(acc)

    acc = MuonClusterOnTrackCreatorCfg(flags)
    cluster_rot_creator=acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("ToolMuonDriftCircle", mdt_rot_creator)
    kwargs.setdefault("ToolMuonCluster", cluster_rot_creator)
    kwargs.setdefault("ToolPixelCluster", None)
    kwargs.setdefault("ToolSCT_Cluster", None)
    kwargs.setdefault("ToolTRT_DriftCircle", None)
    kwargs.setdefault("Mode", 'muon' )
    
    muon_rot_creator = Trk__RIO_OnTrackCreator(name, **kwargs)
    result.addPublicTool(muon_rot_creator,primary=True)
    return result
    
