# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from MuonboyFlags import muonboyFlags
from MuonRecFlags import muonRecFlags
from ConfiguredMuonRec import ConfiguredMuonRec

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties

from AthenaCommon.CfgGetter import getPublicTool,getService

muonRecFlags.setDefaults()

from MboySvc.MboySvcConf import MboySvc
class MboySvcConfig(MboySvc,ConfiguredBase):
    __slots__ = ()

    def __init__(self,name="MboySvc",**kwargs):
        self.applyUserDefaults(kwargs,name)
        super(MboySvcConfig,self).__init__(name,**kwargs)
        # setup required geo reading
        uglyHackedInclude("AmdcAth/AmdcAth_jobOptions.py")

MboySvcConfig.setDefaultProperties(MinuitFileOut = 0)
# end of class MboySvcConfig


class Muonboy(ConfiguredMuonRec):
    def __init__(self,**kwargs):
        ConfiguredMuonRec.__init__(self,"Muonboy",**kwargs)
        # setup minimum config needed to get the Storegate keys
        # full setup is done in functions configure()
        self.addOutputKey("MuonSegments",             "ConvertedMBoySegments") # C++ default
        self.addOutputKey("MuonTracks",               "ConvertedMBoyTracks") # C++ default
        self.addOutputKey("MuonTracksForCalibration", "ConvertedMBoyTracks") # same as MuonTracks
    # end of __init__


    def configure(self,keys=None):
        super(Muonboy,self).configure(keys)
        if not self.isEnabled(): return
        
        configureMboySegmentMaking()
        
        global ToolSvc,ServiceMgr
        from AthenaCommon.Include import include

        from MboyEDMTool.MboyEDMToolConf import Trk__MuonboyToTrackTool,MboyRTTool,AdjT0__AdjustableT0Tool

        from AthenaCommon.AppMgr import ToolSvc
        TheMdtDriftCircleOnTrackCreator             = getPublicTool('MdtDriftCircleOnTrackCreator')
        TheMdtDriftCircleOnTrackCreatorAdjustableT0 = getPublicTool('MdtDriftCircleOnTrackCreatorAdjustableT0Mboy')

        # ToolSvc += Trk__MuonboyToSegmentTool("MuonboyToSegmentToolAdjustableT0",
        #                                             IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreatorAdjustableT0)
        ToolSvc += Trk__MuonboyToTrackTool("MuonboyToTrackToolAdjustableT0",
                                           IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreatorAdjustableT0)
        # ToolSvc += Trk__MuonboyToSegmentTool("MuonboyToSegmentTool",
        #                                      IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreator)
        ToolSvc += Trk__MuonboyToTrackTool("MuonboyToTrackTool",
                                           IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreator)
        # ToolSvc += MboyRTTool( 
        #              IAdjustableT0Tool                         = TheMdtDriftCircleOnTrackCreatorAdjustableT0.MuonTofTool,
        #              IMdtDriftCircleOnTrackCreatorAdjustableT0 = TheMdtDriftCircleOnTrackCreatorAdjustableT0 ,
        #              IMdtDriftCircleOnTrackCreator             = TheMdtDriftCircleOnTrackCreator
        #              )

        MboySvc = getService("MboySvc")
        # MboySvc.MuonboyToSegmentToolAdjustableT0 = ToolSvc.MuonboyToSegmentToolAdjustableT0
        MboySvc.MuonboyToTrackToolAdjustableT0   = ToolSvc.MuonboyToTrackToolAdjustableT0
        # MboySvc.MuonboyToSegmentTool = ToolSvc.MuonboyToSegmentTool
        MboySvc.MuonboyToTrackTool   = ToolSvc.MuonboyToTrackTool
        # MboySvc.MboyRTTool           = ToolSvc.MboyRTTool

        #-----------------------------------------------------------------
        # setup MboyGetPrepDataFromEventTool
        # from MboyEDMTool.MboyEDMToolConf import MboyGetPrepDataFromEventTool
        # if muonboyFlags.BarrelOnly():
        #     GetPDTool = self.addPublicTool( MboyGetPrepDataFromEventTool(
        #         CollectMDT        = 1 ,
        #         CollectRPC        = 1 ,
        #         CollectTGC        = 0 ,
        #         CollectCSC        = 0 ,
        #         CollectCscCluster = 0 ) )
        # else:
        #     GetPDTool = self.addPublicTool( MboyGetPrepDataFromEventTool() )

        # self.MboyGetPrepDataFromEventTool.TgcPrepDataContainerNameList  = [ "TGC_Measurements" ]
        # if muonRecFlags.useTGCPriorNextBC():
        #     self.MboyGetPrepDataFromEventTool.TgcPrepDataContainerNameList += [ "TGC_MeasurementsNextBC" ]
        #     self.MboyGetPrepDataFromEventTool.TgcPrepDataContainerNameList += [ "TGC_MeasurementsPriorBC" ]
        # 
        # if muonRecFlags.doCSCs():
        #     self.MboyGetPrepDataFromEventTool.CollectCscCluster = 1
        # else:
        #     self.MboyGetPrepDataFromEventTool.CollectCscCluster = 0

#         #-----------------------------------------------------------------
#         #Masking EEL EES chambers, dead layers of CSCs at reconstruction level
#         MboySvc.MultiLayerOut[:] = [ ]
# #        MboySvc.MultiLayerOut += [ "EES/*/*/*/*" ]
# #        MboySvc.MultiLayerOut += [ "EEL/*/1/*/*"  , "EEL/*/2/*/*"  ]
# #        MboySvc.MultiLayerOut += [ "EEL/*/4/*/*"  , "EEL/*/5/*/*"  ]
# #        MboySvc.MultiLayerOut += [ "EEL/-1/6/*/*" , "EEL/-2/6/*/*" ]
# #        MboySvc.MultiLayerOut += [ "EEL/1/8/*/*"  , "EEL/2/8/*/*"  ]
#         if not muonRecFlags.doCSCs():
#             MboySvc.MultiLayerOut += [ "CSS/*/*/*/*" ]
#             MboySvc.MultiLayerOut += [ "CSL/*/*/*/*" ]
#         elif globalflags.DataSource() == 'data' :
#             #remove the know dead layers
#             MboySvc.MultiLayerOut += [ "CSL/1/3/CSC/2/3" ] 
#             MboySvc.MultiLayerOut += [ "CSL/1/5/CSC/2/1" ]

        #-----------------------------------------------------------------
        #MboyDigiEmptyLoop: empty loop on digits (Muonboy independent)
        if not muonboyFlags.DumpOnly():
            from MuonboyAth.MuonboyAthConf import MboyDigiEmptyLoop
            self.addAlg( MboyDigiEmptyLoop(MboyGetPrepDataFromEventTool = ToolSvc.MboyGetPrepDataFromEventTool) )

        #-----------------------------------------------------------------
        #MboyRec: reconstruction
        from MuonboyAth.MuonboyAthConf import MboyRec
        # ProcessingStage = 3 : until tracks (no combined)
        self.addAlg( MboyRec(MboyGetPrepDataFromEventTool = ToolSvc.MboyGetPrepDataFromEventTool) )
        if muonboyFlags.DumpOnly():
            self.MboyRec.SwitchOff = 1

        if muonboyFlags.doSegmentsOnly():
            # Until Segments
            self.MboyRec.ProcessingStage = 1
            MboySvc.LevelOfRecons         = 1
        else:
            # Full standalone reco
            self.MboyRec.ProcessingStage = 3 # until TrackParticles
            MboySvc.LevelOfRecons        = 3

        # register some dataKeys
        self.registerOutputKey("MuonSegments",              self.MboyRec, "SegmentCollectionLocation")
        self.registerOutputKey("MuonTracks"               , self.MboyRec, "TrackCollectionLocation")
        self.registerOutputKey("MuonTracksForCalibration" , self.MboyRec, "TrackCollectionLocation")

        from MuonboyAth.MuonboyAthConf import MboyMuonChamberT0s
        self.addAlg( MboyMuonChamberT0s(IAdjustableT0Tool = TheMdtDriftCircleOnTrackCreatorAdjustableT0.MuonTofTool) )
        
        #-----------------------------------------------------------------
        if muonboyFlags.AllChecks():
            from MuonboyAth.MuonboyAthConf import MboyChecks, MboySegmentCheck
            self.addAlg( MboyChecks() )
            self.addAlg( MboySegmentCheck() )
            self.MboyRec.DoDump = 1 
            self.MboyRec.ShortSummaryTrack = 1    
            self.MboyRec.ShortSummarySegment = 1    
            self.MboyRec.PrintDigitsSummary = 1
        # 
        # MboySvc.MdtAdcCut             = 50.
        # MboySvc.SegAdcCut             = 80.
        # if globalflags.DataSource() == 'data' :
        #     MboySvc.AtlasSimulationSample = 0
        # else:
        #     MboySvc.AtlasSimulationSample = 1
        # 
        # if muonRecFlags.doSegmentT0Fit():
        #     MboySvc.DoT0Fit             = 1
        #     MboySvc.UseT0Fit            = 1
        # else:
        #     MboySvc.DoT0Fit             = 0
        #     MboySvc.UseT0Fit            = 0 
        # 
        # if muonRecFlags.useAlignmentCorrections():
        #     MboySvc.AlignementLock = 0
        #     ServiceMgr.AmdcsimrecAthenaSvc.AlignmentCorr   = 3
        #     ServiceMgr.AmdcsimrecAthenaSvc.AlignmentSource = 2
        # 
        # if muonRecFlags.useWireSagCorrections():
        #     MboySvc.RimpfromRTSvc = 1
        #     MboySvc.WireSagOn     = 1

        # if jobproperties.Beam.beamType() == 'singlebeam' or jobproperties.Beam.beamType()  == 'cosmics' :
        #     MboySvc.UseRoaFromMdt          = 0
        #     MboySvc.UseAllSegments         = 1
        #     MboySvc.TubeResolutionWhenNoT0 = 0.05
        #     if jobproperties.Beam.beamType()  == 'cosmics':
        #         MboySvc.AtlasCosmicsSample = 1
        #     else:
        #         MboySvc.AtlasCosmicsSample = 2
        # else:
        #     MboySvc.TubeResolutionWhenNoT0 = 0.02
        #         
        # 
        # if muonRecFlags.enableErrorTuning() or jobproperties.Beam.beamType() == 'singlebeam' or jobproperties.Beam.beamType()  == 'cosmics':
        #     MboySvc.FinalChi2Cut           = 120
        #     MboySvc.MDTTubeResolution      = 0.0090
        # if muonRecFlags.enableErrorTuning():
        #     MboySvc.DoAlignementFit    = 2
        #     MboySvc.BarrelAngularAlig  = 0.00010
        #     MboySvc.EndcapAngularAlig  = 0.00017
        #     MboySvc.AligStationBarrel  = 0.0040
        #     MboySvc.AligStationEndcap  = 0.0080
        #     MboySvc.AligStationCSC     = 0.0080
        #     MboySvc.AligBarrelEndcap   = 0.0300
        #     MboySvc.CSCLayerAlignError = 0.0000
        #     MboySvc.ResBadlyAlignedBIS = 0.2
        #     MboySvc.ResBadlyAlignedBEE = 0.1
        #     MboySvc.ResBadlyAlignedEIL = 0.2
        #     MboySvc.ResBadlyAlignedEEL = 0.3
        #     MboySvc.ResBadlyAligned    = 0.3
        #     MboySvc.ResBadTomo         = 0.1
        #     MboySvc.AligTGCeta         = 0.6
        #     MboySvc.ErrZMsIdBarrel     = 0.1
        #     MboySvc.ErrZMsIdEndcap     = 0.1
            
    # end of configure()
    
        

    def getCalibConfig(self):
        """Get the configuration object to configure Calibration Ntuple output""" 
        doTracks = not muonboyFlags.doSegmentsOnly()
        if doTracks:
            tracksKey = self.dataKey("MuonTracksForCalibration")
        else:
            tracksKey = ""
            
        return { 'eventTag'    : "Muonboy", 
                 'patternsKey' : '????', 
                 'segmentsKey' : self.dataKey("MuonSegments"), 
                 'tracksKey'   : tracksKey,
                 'doPhi'       : False,
                 'segmentAutor'    : 3,
		 'trackAuthor' : 100}

def configureMboySegmentMaking():
    global ToolSvc,ServiceMgr
    from AthenaCommon.Include import include

    from MboyEDMTool.MboyEDMToolConf import Trk__MuonboyToSegmentTool,MboyRTTool,AdjT0__AdjustableT0Tool

    from AthenaCommon.AppMgr import ToolSvc
    TheMdtDriftCircleOnTrackCreator             = getPublicTool('MdtDriftCircleOnTrackCreator')
    TheMdtDriftCircleOnTrackCreatorAdjustableT0 = getPublicTool('MdtDriftCircleOnTrackCreatorAdjustableT0Mboy')

    ToolSvc += Trk__MuonboyToSegmentTool("MuonboyToSegmentToolAdjustableT0",
                                         IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreatorAdjustableT0)
    # ToolSvc += Trk__MuonboyToTrackTool("MuonboyToTrackToolAdjustableT0",
    #                                     IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreatorAdjustableT0)
    ToolSvc += Trk__MuonboyToSegmentTool("MuonboyToSegmentTool",
                                         IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreator)
    # ToolSvc += Trk__MuonboyToTrackTool("MuonboyToTrackTool",
    #                                    IMdtDriftCircleOnTrackCreator = TheMdtDriftCircleOnTrackCreator)
    ToolSvc += MboyRTTool( 
                 IAdjustableT0Tool                         = TheMdtDriftCircleOnTrackCreatorAdjustableT0.MuonTofTool,
                 IMdtDriftCircleOnTrackCreatorAdjustableT0 = TheMdtDriftCircleOnTrackCreatorAdjustableT0 ,
                 IMdtDriftCircleOnTrackCreator             = TheMdtDriftCircleOnTrackCreator
                 )

    MboySvc = getService("MboySvc")
    MboySvc.MuonboyToSegmentToolAdjustableT0 = ToolSvc.MuonboyToSegmentToolAdjustableT0
    # MuonboyToTrackToolAdjustableT0   = ToolSvc.MuonboyToTrackToolAdjustableT0
    MboySvc.MuonboyToSegmentTool = ToolSvc.MuonboyToSegmentTool
    # MboySvc.MuonboyToTrackTool   = ToolSvc.MuonboyToTrackTool
    MboySvc.MboyRTTool           = ToolSvc.MboyRTTool

    #-----------------------------------------------------------------
    # setup MboyGetPrepDataFromEventTool
    from MboyEDMTool.MboyEDMToolConf import MboyGetPrepDataFromEventTool
    if muonboyFlags.BarrelOnly():
        GetPDTool = MboyGetPrepDataFromEventTool(
            CollectMDT        = 1 ,
            CollectRPC        = 1 ,
            CollectTGC        = 0 ,
            CollectCSC        = 0 ,
            CollectCscCluster = 0 )
    else:
        GetPDTool = MboyGetPrepDataFromEventTool()

    GetPDTool.TgcPrepDataContainerNameList  = [ "TGC_MeasurementsAllBCs" ]
    GetPDTool.TgcUseCurrentBC =True
    GetPDTool.TgcUsePrevBC = muonRecFlags.useTGCPriorNextBC()
    GetPDTool.TgcUseNextBC = muonRecFlags.useTGCPriorNextBC()
   
    
#    if muonRecFlags.useTGCPriorNextBC():
#        GetPDTool.TgcPrepDataContainerNameList += [ "TGC_MeasurementsNextBC" ]
#        GetPDTool.TgcPrepDataContainerNameList += [ "TGC_MeasurementsPriorBC" ]

    if muonRecFlags.doCSCs():
        GetPDTool.CollectCscCluster = 1
    else:
        GetPDTool.CollectCscCluster = 0
    
    ToolSvc += GetPDTool

    #-----------------------------------------------------------------
    #Masking EEL EES chambers, dead layers of CSCs at reconstruction level
    MboySvc.MultiLayerOut[:] = [ ]
#        MboySvc.MultiLayerOut += [ "EES/*/*/*/*" ]
#        MboySvc.MultiLayerOut += [ "EEL/*/1/*/*"  , "EEL/*/2/*/*"  ]
#        MboySvc.MultiLayerOut += [ "EEL/*/4/*/*"  , "EEL/*/5/*/*"  ]
#        MboySvc.MultiLayerOut += [ "EEL/-1/6/*/*" , "EEL/-2/6/*/*" ]
#        MboySvc.MultiLayerOut += [ "EEL/1/8/*/*"  , "EEL/2/8/*/*"  ]
    if not muonRecFlags.doCSCs():
        MboySvc.MultiLayerOut += [ "CSS/*/*/*/*" ]
        MboySvc.MultiLayerOut += [ "CSL/*/*/*/*" ]
    elif globalflags.DataSource() == 'data' :
        #remove the know dead layers
        MboySvc.MultiLayerOut += [ "CSL/1/3/CSC/2/3" ] 
        MboySvc.MultiLayerOut += [ "CSL/1/5/CSC/2/1" ]

    #-----------------------------------------------------------------
    #MboyDigiEmptyLoop: empty loop on digits (Muonboy independent)
    # if not muonboyFlags.DumpOnly():
    #     from MuonboyAth.MuonboyAthConf import MboyDigiEmptyLoop
    #     self.addAlg( MboyDigiEmptyLoop(MboyGetPrepDataFromEventTool = GetPDTool) )

    #-----------------------------------------------------------------
    #MboyRec: reconstruction
    from MuonboyAth.MuonboyAthConf import MboyRec
    # ProcessingStage = 3 : until tracks (no combined)
    # self.addAlg( MboyRec(MboyGetPrepDataFromEventTool = GetPDTool) )
    # if muonboyFlags.DumpOnly():
    #     self.MboyRec.SwitchOff = 1

    # if muonboyFlags.doSegmentsOnly():
    #         # Until Segments
        # self.MboyRec.ProcessingStage = 1
    MboySvc.LevelOfRecons         = 1
    # else:
    #     # Full standalone reco
    #     self.MboyRec.ProcessingStage = 3 # until TrackParticles
    #     MboySvc.LevelOfRecons        = 3

    # register some dataKeys
    # self.registerOutputKey("MuonSegments",              self.MboyRec, "SegmentCollectionLocation")
    # self.registerOutputKey("MuonTracks"               , self.MboyRec, "TrackCollectionLocation")
    # self.registerOutputKey("MuonTracksForCalibration" , self.MboyRec, "TrackCollectionLocation")

    # from MuonboyAth.MuonboyAthConf import MboyMuonChamberT0s
    # self.addAlg( MboyMuonChamberT0s(IAdjustableT0Tool = TheMdtDriftCircleOnTrackCreatorAdjustableT0.MuonTofTool) )
    
    #-----------------------------------------------------------------
    # if muonboyFlags.AllChecks():
    #     from MuonboyAth.MuonboyAthConf import MboyChecks, MboySegmentCheck
    #     self.addAlg( MboyChecks() )
    #     self.addAlg( MboySegmentCheck() )
    #     self.MboyRec.DoDump = 1 
    #     self.MboyRec.ShortSummaryTrack = 1    
    #     self.MboyRec.ShortSummarySegment = 1    
    #     self.MboyRec.PrintDigitsSummary = 1

    MboySvc.MdtAdcCut             = 50.
    MboySvc.SegAdcCut             = 80.
    if globalflags.DataSource() == 'data' :
        MboySvc.AtlasSimulationSample = 0
    else:
        MboySvc.AtlasSimulationSample = 1

    if muonRecFlags.doSegmentT0Fit():
        MboySvc.DoT0Fit             = 1
        MboySvc.UseT0Fit            = 1
    else:
        MboySvc.DoT0Fit             = 0
        MboySvc.UseT0Fit            = 0 

    if muonRecFlags.useAlignmentCorrections():
        MboySvc.AlignementLock = 0
        ServiceMgr.AmdcsimrecAthenaSvc.AlignmentCorr   = 3
        ServiceMgr.AmdcsimrecAthenaSvc.AlignmentSource = 2

    if muonRecFlags.useWireSagCorrections():
        MboySvc.RimpfromRTSvc = 1
        MboySvc.WireSagOn     = 1

    if jobproperties.Beam.beamType() == 'singlebeam' or jobproperties.Beam.beamType()  == 'cosmics' :
        MboySvc.UseRoaFromMdt          = 0
        MboySvc.UseAllSegments         = 1
        MboySvc.TubeResolutionWhenNoT0 = 0.05
        if jobproperties.Beam.beamType()  == 'cosmics':
            MboySvc.AtlasCosmicsSample = 1
        else:
            MboySvc.AtlasCosmicsSample = 2
    else:
        MboySvc.TubeResolutionWhenNoT0 = 0.02
            

    if muonRecFlags.enableErrorTuning() or jobproperties.Beam.beamType() == 'singlebeam' or jobproperties.Beam.beamType()  == 'cosmics':
        MboySvc.FinalChi2Cut           = 120
        MboySvc.MDTTubeResolution      = 0.0090
    if muonRecFlags.enableErrorTuning():
        MboySvc.DoAlignementFit    = 2
        MboySvc.BarrelAngularAlig  = 0.00010
        MboySvc.EndcapAngularAlig  = 0.00017
        MboySvc.AligStationBarrel  = 0.0040
        MboySvc.AligStationEndcap  = 0.0080
        MboySvc.AligStationCSC     = 0.0080
        MboySvc.AligBarrelEndcap   = 0.0300
        MboySvc.CSCLayerAlignError = 0.0000
        MboySvc.ResBadlyAlignedBIS = 0.2
        MboySvc.ResBadlyAlignedBEE = 0.1
        MboySvc.ResBadlyAlignedEIL = 0.2
        MboySvc.ResBadlyAlignedEEL = 0.3
        MboySvc.ResBadlyAligned    = 0.3
        MboySvc.ResBadTomo         = 0.1
        MboySvc.AligTGCeta         = 0.6
        MboySvc.ErrZMsIdBarrel     = 0.1
        MboySvc.ErrZMsIdEndcap     = 0.1


# for backwards compatibility
MuonboyStandalone = Muonboy
MuonboyCombined   = Muonboy

