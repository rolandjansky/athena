# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of Muon Spectrometer Standalone muon reconstruction"""

###############################################################
#
# Configuration for Standalone
#
#==============================================================
from AthenaCommon import CfgMgr

from .MuonStandaloneFlags import muonStandaloneFlags,MoorelikeStrategy
from .MuonRecFlags import muonRecFlags
from .ConfiguredMuonRec import ConfiguredMuonRec

from .MuonRecUtils import ExtraFlags

from AthenaCommon.CfgGetter import getPublicTool,getPrivateTool,getPublicToolClone
from RecExConfig.ObjKeyStore                  import cfgKeyStore

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
#==============================================================

# call  setDefaults to update default flags
muonRecFlags.setDefaults()
muonStandaloneFlags.setDefaults()

#
# Tools for MuPat track building
#

def MuonTrackSteering(name="MuonTrackSteering", extraFlags=None, **kwargs):
    if extraFlags is None:
        extraFlags = ExtraFlags()
        
    extraFlags.setFlagDefault("namePrefix", "MuSt_")
    extraFlags.setFlagDefault("doSegmentPhiMatching", True)
    extraFlags.setFlagDefault(muonStandaloneFlags.optimiseMomentumResolutionUsingChi2) # take name & value from JobProperty
    extraFlags.setFlagDefault(muonStandaloneFlags.strategy)
    extraFlags.setFlagDefault(muonStandaloneFlags.trackBuilder)
    extraFlags.setFlagDefault(muonStandaloneFlags.printSummary)
    extraFlags.setFlagDefault(muonStandaloneFlags.refinementTool)
                  
    if extraFlags.strategy:
        kwargs.setdefault("StrategyList", extraFlags.strategy)
    else:
        kwargs.setdefault("StrategyList", MoorelikeStrategy)  

    kwargs.setdefault("DoSummary", extraFlags.printSummary)
    kwargs.setdefault("OutputSingleStationTracks", True)
    kwargs.setdefault("HoleRecoveryTool",getPublicTool("MuonEORecoveryTool"))
    if "TrackBuilderTool" not in kwargs:
        extraFlags.setFlagDefault('UseTrackingHistory',True)
        kwargs["TrackBuilderTool"] = getPublicToolClone("MooMuonTrackBuilder", "MooTrackBuilderTemplate",
                                                        extraFlags=extraFlags)
        if "TrackRefinementTool" not in kwargs:
            kwargs["TrackRefinementTool"] = getPublicTool("MooTrackBuilderTemplate")

    kwargs.setdefault("SegSeedQCut", 2)
    kwargs.setdefault("Seg2ndQCut", 1)
    return CfgMgr.Muon__MuonTrackSteering(name,**kwargs)

# Segment finding algorithms
def MooSegmentFinderAlg( name="MuonSegmentMaker",**kwargs ):
    kwargs.setdefault("SegmentFinder", getPublicToolClone("MuonSegmentFinder","MooSegmentFinder",
                                                          DoSummary=muonStandaloneFlags.printSummary()))
    kwargs.setdefault("MuonClusterSegmentFinderTool",getPublicTool("MuonClusterSegmentFinder"))
    kwargs.setdefault("MuonSegmentOutputLocation", "TrackMuonSegments")
    kwargs.setdefault("UseCSC", muonRecFlags.doCSCs())
    kwargs.setdefault("UseMDT", muonRecFlags.doMDTs())
    kwargs.setdefault("UseRPC", muonRecFlags.doRPCs())
    kwargs.setdefault("UseTGC", muonRecFlags.doTGCs())
    if ConfigFlags.Muon.MuonTrigger:
        kwargs.setdefault("UseTGCPriorBC", False)
        kwargs.setdefault("UseTGCNextBC", False)
    else:
        kwargs.setdefault("UseTGCPriorBC", muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC())
        kwargs.setdefault("UseTGCNextBC", muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC())
    kwargs.setdefault("doTGCClust", muonRecFlags.doTGCClusterSegmentFinding())
    kwargs.setdefault("doRPCClust", muonRecFlags.doRPCClusterSegmentFinding())
    if muonStandaloneFlags.reconstructionMode() != 'collisions':
        kwargs.setdefault("Key_MuonLayerHoughToolHoughDataPerSectorVec","")
    return CfgMgr.MooSegmentFinderAlg(name,**kwargs)

#
def MuonSegmentFilterAlg(name="MuonSegmentFilterAlg", **kwargs):
    kwargs.setdefault("SegmentCollectionName", "TrackMuonSegments")
    return CfgMgr.MuonSegmentFilterAlg(name, **kwargs)


def MooSegmentFinderNCBAlg( name="MuonSegmentMaker_NCB",**kwargs ):
    reco_cscs = muonRecFlags.doCSCs() and MuonGeometryFlags.hasCSC()
    kwargs.setdefault("SegmentFinder",getPublicToolClone("MooSegmentFinder_NCB","MooSegmentFinder",
                                                         DoSummary=False,
                                                         Csc2dSegmentMaker = (getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
                                                                                                 segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
                                                                                                                                  "CscSegmentUtilTool",
                                                                                                                                  TightenChi2 = False, 
                                                                                                                                  IPconstraint=False)) if reco_cscs else ""),
                                                         Csc4dSegmentMaker = (getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
                                                                                                 segmentTool = getPublicTool("CscSegmentUtilTool_NCB")) if reco_cscs else ""),
                                                         DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False))
    kwargs.setdefault("MuonPatternCombinationLocation", "NCB_MuonHoughPatternCombinations")
    kwargs.setdefault("MuonSegmentOutputLocation", "NCB_TrackMuonSegments")
    kwargs.setdefault("UseCSC", reco_cscs)
    kwargs.setdefault("UseMDT", False)
    kwargs.setdefault("UseRPC", False)
    kwargs.setdefault("UseTGC", False)
    kwargs.setdefault("UseTGCPriorBC", False)
    kwargs.setdefault("UseTGCNextBC", False)
    kwargs.setdefault("doTGCClust", False)
    kwargs.setdefault("doRPCClust", False)
    kwargs.setdefault("Key_MuonLayerHoughToolHoughDataPerSectorVec","")

    return CfgMgr.MooSegmentFinderAlg(name,**kwargs)

def MuonSegmentFinderAlg( name="MuonSegmentMaker", **kwargs):
    SegmentFinder = getPublicTool("MuonClusterSegmentFinderTool")
    Cleaner = getPublicToolClone("MuonTrackCleaner_seg","MuonTrackCleaner")
    Cleaner.Extrapolator = getPublicTool("MuonStraightLineExtrapolator")
    Cleaner.Fitter = getPublicTool("MCTBSLFitterMaterialFromTrack")
    Cleaner.PullCut = 3
    Cleaner.PullCutPhi = 3
    Cleaner.UseSLFit = True
    SegmentFinder.TrackCleaner = Cleaner
    # for test purposes allow parallel running of truth segment finding and new segment finder
    SegmentLocation = "TrackMuonSegments"
    if muonStandaloneFlags.segmentOrigin == 'TruthTracking':
        SegmentLocation = "ThirdChainSegments"
    MuonSegmentFinderAlg = CfgMgr.MuonSegmentFinderAlg( name,SegmentCollectionName=SegmentLocation, 
                                                        MuonPatternCalibration = getPublicTool("MuonPatternCalibration"),
                                                        MuonPatternSegmentMaker = getPublicTool("MuonPatternSegmentMaker"),
                                                        PrintSummary = muonStandaloneFlags.printSummary(),
                                                        TGC_PRDs ='TGC_MeasurementsAllBCs' if not muonRecFlags.useTGCPriorNextBC else 'TGC_Measurements' , 
                                                        **kwargs )
   # print ('TGC_MeasurementsAllBCs' if not muonRecFlags.useTGCPriorNextBC else 'TGC_Measurements')
   
    # we check whether the layout contains any CSC chamber and if yes, we check that the user also wants to use the CSCs in reconstruction
    if muonRecFlags.doCSCs() and MuonGeometryFlags.hasCSC():
        getPublicTool("CscSegmentUtilTool")
        MuonSegmentFinderAlg.Csc2dSegmentMaker = getPublicTool("Csc2dSegmentMaker")
        MuonSegmentFinderAlg.Csc4dSegmentMaker = getPublicTool("Csc4dSegmentMaker")
    else:
        MuonSegmentFinderAlg.Csc2dSegmentMaker = ""
        MuonSegmentFinderAlg.Csc4dSegmentMaker = ""
        MuonSegmentFinderAlg.CSC_clusterkey = ""
    return MuonSegmentFinderAlg


def MuonStandaloneTrackParticleCnvAlg( name="MuonStandaloneTrackParticleCnvAlg",**kwargs):
    from AthenaCommon.Include import include
    include("BeamSpotConditions/BeamCondAlgSetup.py" )
    from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg, xAODMaker__TrackCollectionCnvTool, xAODMaker__RecTrackParticleContainerCnvTool

    muonParticleCreatorTool = getPublicTool("MuonParticleCreatorTool")
    muonTrackCollectionCnvTool = xAODMaker__TrackCollectionCnvTool( name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muonParticleCreatorTool )
    muonRecTrackParticleContainerCnvTool = xAODMaker__RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muonParticleCreatorTool )

    kwargs.setdefault("TrackParticleCreator", muonParticleCreatorTool)
    kwargs.setdefault("RecTrackParticleContainerCnvTool", muonRecTrackParticleContainerCnvTool)
    kwargs.setdefault("TrackCollectionCnvTool", muonTrackCollectionCnvTool)
    kwargs.setdefault("RecTrackParticleContainerCnvTool", muonRecTrackParticleContainerCnvTool)
    kwargs.setdefault("TrackContainerName", "MuonSpectrometerTracks")
    kwargs.setdefault("xAODTrackParticlesFromTracksContainerName", "MuonSpectrometerTrackParticles")
    kwargs.setdefault("AODContainerName", "")
    kwargs.setdefault("AODTruthContainerName", "")
    kwargs.setdefault("xAODTruthLinkVector",  "")
    kwargs.setdefault("ConvertTrackParticles", False)
    kwargs.setdefault("ConvertTracks", True)

    return xAODMaker__TrackParticleCnvAlg(name,**kwargs)

#
# The top level configurator
#
class MuonStandalone(ConfiguredMuonRec):
    def __init__(self,**kwargs):
        ConfiguredMuonRec.__init__(self,"MuonStandalone",**kwargs)
        # setup minimum config needed to get Storegate keys
        # full setup is done in configure()

        # keys for output of segment building stage
        self.addOutputKey("MuonSegments", "MuonSegments")

    def configure(self,keys=None):
        super(MuonStandalone,self).configure(keys)
        if not self.isEnabled(): return

        # do the following in case of (at least one) NSW
        reco_stgc = muonRecFlags.dosTGCs() and MuonGeometryFlags.hasSTGC()
        reco_mircomegas = muonRecFlags.doMicromegas() and MuonGeometryFlags.hasMM()
        reco_cscs = muonRecFlags.doCSCs() and MuonGeometryFlags.hasCSC()
      
        
        self.addAlg( CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg",
            MuonLayerScanTool =  getPublicTool("MuonLayerHoughTool"),
            PrintSummary = muonStandaloneFlags.printSummary(),
            CscPrepDataContainer = ("CSC_Clusters" if reco_cscs else ""),
            sTgcPrepDataContainer = ("STGC_Measurements" if reco_stgc else ""),
            MMPrepDataContainer = ("MM_Measurements" if reco_mircomegas else ""),
            TgcPrepDataContainer = 'TGC_MeasurementsAllBCs' if not muonRecFlags.useTGCPriorNextBC else 'TGC_Measurements'  ) )
        if not muonStandaloneFlags.patternsOnly():
            self.addAlg( MuonSegmentFinderAlg("MuonSegmentMaker" ))
            if reco_cscs:
                self.addAlg(MooSegmentFinderNCBAlg("MuonSegmentMaker_NCB"))

                if not cfgKeyStore.isInInput ('xAOD::MuonSegmentContainer', 'MuonSegments_NCB'):
                     self.addAlg( CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg_NCB",
                                                                      SegmentContainerName="NCB_TrackMuonSegments",
                                                                      xAODContainerName="NCB_MuonSegments") )

        if (not cfgKeyStore.isInInput ('xAOD::MuonSegmentContainer', 'MuonSegments')):
            self.addAlg( CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg") )


        if muonStandaloneFlags.doSegmentsOnly():
            return	                    
        # Tracks builder
        #
        # add the algorithm (which uses the MuonTrackSteering)
        # 
        TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder", 
                                                TrackSteering=getPrivateTool("MuonTrackSteering"), 
                                                SpectrometerTrackOutputLocation="MuonSpectrometerTracks", 
                                                MuonSegmentCollection="TrackMuonSegments")

        self.addAlg( TrackBuilder )
        #### Add a segment collection only containing only EM and EO hits
        if muonRecFlags.runCommissioningChain():
            self.addAlg(MuonSegmentFilterAlg(FilteredCollectionName="TrackMuonSegmentsEMEO"))
  
            chamberRecovery_EMEO = getPublicToolClone("MuonChamberRecovery_EMEO", "MuonChamberHoleRecoveryTool", 
                                                               sTgcPrepDataContainer="",
                                                               MMPrepDataContainer="")

            MooTrackBuilder_EMEO = getPublicToolClone("MooMuonTrackBuilder_EMEO", 
                                               "MooTrackBuilderTemplate",
                                                ChamberHoleRecoveryTool = chamberRecovery_EMEO)
            TrackSteeringTool_EMEO = getPublicToolClone("MuonTrackSteering_EMEO", "MuonTrackSteering", TrackBuilderTool = MooTrackBuilder_EMEO)
            
            
            TrackBuilder_EMEO = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder_EMEO", 
                                                TrackSteering=TrackSteeringTool_EMEO, 
                                                SpectrometerTrackOutputLocation="EMEO_MuonSpectrometerTracks", 
                                                MuonSegmentCollection="TrackMuonSegmentsEMEO")
            self.addAlg(TrackBuilder_EMEO)
            if muonStandaloneFlags.createTrackParticles():
                xAODTrackParticleCnvAlg_EMEO = MuonStandaloneTrackParticleCnvAlg("MuonStandaloneTrackParticleCnvAlg_EMEO",
                                                                           TrackContainerName = "EMEO_MuonSpectrometerTracks",
                                                                           xAODTrackParticlesFromTracksContainerName="EMEO_MuonSpectrometerTrackParticles")
                self.addAlg( xAODTrackParticleCnvAlg_EMEO )
               

        if muonStandaloneFlags.createTrackParticles():
            xAODTrackParticleCnvAlg = MuonStandaloneTrackParticleCnvAlg("MuonStandaloneTrackParticleCnvAlg")
            self.addAlg( xAODTrackParticleCnvAlg )



    def getCalibConfig(self):
        """Get the configuration to configure Calibration Ntuple output"""
        doTracks = not muonStandaloneFlags.doSegmentsOnly()
        if doTracks:
            tracksKey = "MuonSpectrometerTracks"
        else:
            tracksKey = ""
            
        return { 'eventTag'         : muonStandaloneFlags.segmentOrigin(), 
                 'patternsKey'      : '',#self.dataKey("MuonPatterns"), 
                 'segmentsKey'      : self.dataKey("MuonSegments"), 
                 'tracksKey'        : tracksKey,
                 'doPhi'            : muonStandaloneFlags.trackBuilder == 'Moore',
                 'segmentAuthor'    : 5,
                 'trackAuthor'     : 200
                 }
                  
