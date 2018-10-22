# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MuonConfig.MuonCalibConfig import MdtCalibrationSvcCfg, MdtCalibrationDbSvcCfg
from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber # TODO should maybe shift this elsewhere?
from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import Muon__MdtDriftCircleOnTrackCreator
from MuonClusterOnTrackCreator.MuonClusterOnTrackCreatorConf import Muon__CscClusterOnTrackCreator, Muon__MuonClusterOnTrackCreator
from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator

def CscClusterOnTrackCreatorCfg(flags,**kwargs):
    from  MuonConfig.MuonSegmentFindingConfig import QratCscClusterFitterCfg
    
    result=ComponentAccumulator()
    
    acc, qrat = MuonConfig.MuonSegmentFindingConfig.QratCscClusterFitterCfg(flags)
    result.addPublicTool(qrat)
    result.merge(acc)
    
    # TODO fix this
    # kwargs.setdefault("CscStripFitter", getPublicTool("CalibCscStripFitter") )
    kwargs.setdefault("CscClusterFitter", qrat )
    # kwargs.setdefault("CscClusterUtilTool", getPublicTool("CscClusterUtilTool") )
    if False  : # enable CscClusterOnTrack error scaling :
        from InDetRecExample.TrackingCommon import getRIO_OnTrackErrorScalingCondAlg,createAndAddCondAlg
        createAndAddCondAlg(getMuonRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')

        kwargs.setdefault("CSCErrorScalingKey","/MUON/TrkErrorScalingCSC")

    if not flags.Input.isMC: # collisions real data or simulated first data
        # scale CSC and hit errors 
        kwargs.setdefault("ErrorScalerBeta", 0.070 )

    csc_cluster_creator = Muon__CscClusterOnTrackCreator(**kwargs)
    
    return result, csc_cluster_creator


def MdtDriftCircleOnTrackCreatorCfg(flags,**kwargs):
    result=ComponentAccumulator()
    
    # setup dependencies missing in C++. TODO: fix in C++
    acc, mdt_calib_svc  = MdtCalibrationSvcCfg(flags)
    result.merge(acc)
    
    acc,mdt_calib_db_svc = MdtCalibrationDbSvcCfg(flags)
    result.merge(acc)

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
        kwargs.setdefault("MuonTofTool", None)
        kwargs.setdefault("TimeWindowSetting", mdtCalibWindowNumber('Collision_data'))  # MJW: should this be Collision_G4 ???
        kwargs.setdefault("UseParametrisedError", False)

        if not flags.Input.isMC : 
            kwargs.setdefault("CreateTubeHit", True)  # BroadErrors
            kwargs.setdefault("UseLooseErrors", flags.Muon.useLooseErrorTuning)  # LooseErrors on data                          
    
    kwargs.setdefault("IsMC", flags.Input.isMC)
                  
    return result,Muon__MdtDriftCircleOnTrackCreator(**kwargs)
    
def MuonClusterOnTrackCreatorCfg(flags,**kwargs):
    result=ComponentAccumulator()
    if not flags.Input.isMC: # collisions real data or simulated first data
        # scale TGC eta hit errors as long as TGC eta are not well aligned
        kwargs.setdefault("DoFixedErrorTgcEta", True)
        kwargs.setdefault("FixedErrorTgcEta", 15.)

    # TODO Fixme - the cxx class retrieves public MuonIdHelperTool ... should be private / service.
    
    muon_cluster_rot_creator = Muon__MuonClusterOnTrackCreator(**kwargs)
    return result, muon_cluster_rot_creator

    
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

def MuonRotCreatorCfg(flags, **kwargs):
    result=ComponentAccumulator()
    acc, mdt_rot_creator = MdtDriftCircleOnTrackCreatorCfg(flags)
    acc.addPublicTool(mdt_rot_creator) # TODO remove
    result.merge(acc)
    
    acc, cluster_rot_creator = MuonClusterOnTrackCreatorCfg(flags)
    acc.addPublicTool(cluster_rot_creator) # TODO remove
    result.merge(acc)
    kwargs.setdefault("ToolMuonDriftCircle", mdt_rot_creator)
    kwargs.setdefault("ToolMuonCluster", cluster_rot_creator)
    kwargs.setdefault("ToolPixelCluster", None)
    kwargs.setdefault("ToolSCT_Cluster", None)
    kwargs.setdefault("ToolTRT_DriftCircle", None)
    kwargs.setdefault("Mode", 'muon' )
    
    muon_rot_creator = Trk__RIO_OnTrackCreator(**kwargs)
    return result, muon_rot_creator
    
