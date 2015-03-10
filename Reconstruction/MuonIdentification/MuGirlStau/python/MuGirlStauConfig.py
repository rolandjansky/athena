# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuGirlStau.MuGirlStauConf import MuGirlNS__StauTool
from AthenaCommon.Configurable import *
from AthenaCommon.GlobalFlags import globalflags 
#from MuGirlStau.MuGirlStauFlags import muGirlStauFlags
from MuonRecExample import MuonRecTools
from MuonRecExample.MuonRecUtils import mdtCalibWindowNumber 
import os,os.path

class MuGirlNS__StauToolConfig(MuGirlNS__StauTool):
    def __init__(self, name = Configurable.DefaultName):
        super(MuGirlNS__StauToolConfig, self).__init__(name)

        from AthenaCommon.AppMgr import ToolSvc
        from MuGirlStau.MuGirlStauConf import MuGirlNS__StauBetaTofTool
        BetaTofTool_stau = MuGirlNS__StauBetaTofTool(name = 'StauBetaTofTool')
        ToolSvc += BetaTofTool_stau 


        DriftCircleOnTrack_stau = MuonRecTools.getPublicToolClone("StauMdtDriftCircleCreator", "MdtDriftCircleOnTrackCreator",
                                                                   DoFixedError = False, FixedError = 0.,
                                                                   TimeWindowSetting = mdtCalibWindowNumber('Collision_t0fit'))
                                                                   #  TimeWindowLowerBound = 50., TimeWindowUpperBound = 100 )
                                                                   #  ScaleErrorsManually = True, FixedErrorScale = 2., FixedError = 0.1) 
        ToolSvc += DriftCircleOnTrack_stau
        DriftCircleOnTrack_stau.TimingMode = 3
        DriftCircleOnTrack_stau.MuonTofTool = BetaTofTool_stau

        from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__MdtMathSegmentFinder
        finder_stau = Muon__MdtMathSegmentFinder("MuGirlStauSegmentFinder")
        finder_stau.DoDrop=True
        finder_stau.Chi2PerDofDropping=20
        finder_stau.MDTAssocationPullcut=5
        finder_stau.RecoverMdtOutliers=False
        finder_stau.RemoveSingleMdtOutliers=False
        ToolSvc += finder_stau
        from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__DCMathSegmentMaker
        maker_stau = Muon__DCMathSegmentMaker("MuGirlStauSegmentMaker")
        maker_stau.MdtSegmentFinder = finder_stau
        maker_stau.CurvedErrorScaling=True
        maker_stau.SinAngleCut=0.05
        maker_stau.MdtCreator = DriftCircleOnTrack_stau
        ToolSvc += maker_stau 

        #from MuidTrackBuilder.MuidTrackBuilderConf import Rec__CombinedMuonTrackBuilder
        #CombinedMuonTrackBuilder_stau = Rec__CombinedMuonTrackBuilder(name = 'MuGirlStauCombinedMuonTrackBuilder')
        #CombinedMuonTrackBuilder_stau.CleanCombined=False
        #ToolSvc += CombinedMuonTrackBuilder_stau

        #from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        #GlobalFitTool_stau = MuGirlNS__GlobalFitTool(name = 'MuGirlStauGlobalFitTool')
        #GlobalFitTool_stau.TrackFitter = CombinedMuonTrackBuilder_stau
        #ToolSvc += GlobalFitTool_stau
        
#         from TrackToCalo.TrackToCaloConf import Rec__ParticleCaloCellAssociationTool
#         caloCellAssociationTool = Rec__ParticleCaloCellAssociationTool("MuGirlStauCaloCellAssociationTool")
#         ToolSvc += caloCellAssociationTool
#         self.ParticleCaloCellAssociationTool = caloCellAssociationTool
        
        self.StauMdtDriftCircleCreator = DriftCircleOnTrack_stau
        self.StauMdtSegmentMaker = maker_stau
        self.StauBetaTofTool = BetaTofTool_stau
        #self.StauGlobalFitTool = GlobalFitTool_stau
        # print maker_stau

        if globalflags.DataSource() == 'data': 
            self.isData=True
            self.rpcTimeShift=88.
        else:
            self.isData=False

        from AthenaCommon.Utils.unixtools import find_datafile
        self.doCalibration = True

        self.mdtCalibFileName = "mdt_calibration.data";
        self.rpcCalibFileName = "rpc_calibration.data";
        self.caloCalibFileName = "calo_calibration.data";

#print MuGirlNS__StauTool
