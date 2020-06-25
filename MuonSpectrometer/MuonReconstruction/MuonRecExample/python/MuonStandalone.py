# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

from .MuonRecUtils import logMuon,ExtraFlags

from RecExConfig.RecFlags import rec
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone,getPrivateTool
from RecExConfig.ObjKeyStore                  import cfgKeyStore

import sys

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

from TriggerJobOpts.TriggerFlags import TriggerFlags
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
    kwargs.setdefault("HoleRecoveryTool",       "MuonEORecoveryTool")
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
    kwargs.setdefault("MuonSegmentOutputLocation", "MuonSegments")
    kwargs.setdefault("UseCSC", muonRecFlags.doCSCs())
    kwargs.setdefault("UseMDT", muonRecFlags.doMDTs())
    kwargs.setdefault("UseRPC", muonRecFlags.doRPCs())
    kwargs.setdefault("UseTGC", muonRecFlags.doTGCs())
    if TriggerFlags.MuonSlice.doTrigMuonConfig:
        kwargs.setdefault("UseTGCPriorBC", False)
        kwargs.setdefault("UseTGCNextBC", False)
    else:
        kwargs.setdefault("UseTGCPriorBC", muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC())
        kwargs.setdefault("UseTGCNextBC", muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC())
    kwargs.setdefault("doTGCClust", muonRecFlags.doTGCClusterSegmentFinding())
    kwargs.setdefault("doRPCClust", muonRecFlags.doRPCClusterSegmentFinding())

    return CfgMgr.MooSegmentFinderAlg(name,**kwargs)

def MooSegmentFinderNCBAlg( name="MuonSegmentMaker_NCB",**kwargs ):
    kwargs.setdefault("SegmentFinder",getPublicToolClone("MooSegmentFinder_NCB","MuonSegmentFinder",
                                                         DoSummary=False,
                                                         Csc2dSegmentMaker = (getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
                                                                                                 segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
                                                                                                                                  "CscSegmentUtilTool",
                                                                                                                                  TightenChi2 = False, 
                                                                                                                                  IPconstraint=False)) if MuonGeometryFlags.hasCSC() else ""),
                                                         Csc4dSegmentMaker = (getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
                                                                                                 segmentTool = getPublicTool("CscSegmentUtilTool_NCB")) if MuonGeometryFlags.hasCSC() else ""),
                                                         DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False))
    kwargs.setdefault("MuonPatternCombinationLocation", "NCB_MuonHoughPatternCombinations")
    kwargs.setdefault("MuonSegmentOutputLocation", "NCB_MuonSegments")
    kwargs.setdefault("MuonSegmentOutputLocation", "NCB_MuonSegments")
    kwargs.setdefault("UseCSC", muonRecFlags.doCSCs())
    kwargs.setdefault("UseMDT", False)
    kwargs.setdefault("UseRPC", False)
    kwargs.setdefault("UseTGC", False)
    kwargs.setdefault("UseTGCPriorBC", False)
    kwargs.setdefault("UseTGCNextBC", False)
    kwargs.setdefault("doTGCClust", False)
    kwargs.setdefault("doRPCClust", False)

    return CfgMgr.MooSegmentFinderAlg(name,**kwargs)

def MuonStandaloneTrackParticleCnvAlg( name="MuonStandaloneTrackParticleCnvAlg",**kwargs):
    from AthenaCommon.Include import include
    include("InDetBeamSpotService/BeamCondSvc.py" )        
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

    return xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg",**kwargs)

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

        from AthenaCommon.BeamFlags import jobproperties
        beamFlags = jobproperties.Beam 
        SegmentLocation = "MuonSegments"
        if muonStandaloneFlags.segmentOrigin == 'TruthTracking':
            SegmentLocation = "ThirdChainSegments"

        # do the following in case of (at least one) NSW
        if (MuonGeometryFlags.hasSTGC() and MuonGeometryFlags.hasMM()):
            getPublicTool("MuonLayerHoughTool")
            self.addAlg( CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg", 
                PrintSummary = muonStandaloneFlags.printSummary(),
                CscPrepDataContainer = ("CSC_Clusters" if MuonGeometryFlags.hasCSC() else ""),
                sTgcPrepDataContainer = ("STGC_Measurements" if MuonGeometryFlags.hasSTGC() else ""),
                MMPrepDataContainer = ("MM_Measurements" if MuonGeometryFlags.hasMM() else "")  ) )
            if not muonStandaloneFlags.patternsOnly():
                SegmentFinder = getPublicTool("MuonClusterSegmentFinderTool")
                Cleaner = getPublicToolClone("MuonTrackCleaner_seg","MuonTrackCleaner")
                Cleaner.Extrapolator = getPublicTool("MuonStraightLineExtrapolator")
                Cleaner.Fitter = getPublicTool("MCTBSLFitterMaterialFromTrack")
                Cleaner.PullCut = 3
                Cleaner.PullCutPhi = 3
                Cleaner.UseSLFit = True
                SegmentFinder.TrackCleaner = Cleaner
            # for test purposes allow parallel running of truth segment finding and new segment finder
                MuonSegmentFinderAlg = CfgMgr.MuonSegmentFinderAlg( "MuonSegmentMaker",SegmentCollectionName=SegmentLocation, 
                                                                    MuonPatternCalibration = getPublicTool("MuonPatternCalibration"),
                                                                    MuonPatternSegmentMaker = getPublicTool("MuonPatternSegmentMaker"),
                                                                    MuonTruthSummaryTool = None,
                                                                    PrintSummary = muonStandaloneFlags.printSummary() )
                # we check whether the layout contains any CSC chamber and if yes, we check that the user also wants to use the CSCs in reconstruction
                if MuonGeometryFlags.hasCSC() and muonRecFlags.doCSCs():
                    getPublicTool("CscSegmentUtilTool")
                    getPublicTool("Csc2dSegmentMaker")
                    getPublicTool("Csc4dSegmentMaker")
                else:
                    MuonSegmentFinderAlg.Csc2dSegmentMaker = ""
                    MuonSegmentFinderAlg.Csc4dSegmentMaker = ""
                self.addAlg( MuonSegmentFinderAlg )
        else:
            getPublicTool("MuonLayerHoughTool")
            self.addAlg(MooSegmentFinderAlg("MuonSegmentMaker"))

            self.addAlg(MooSegmentFinderNCBAlg("MuonSegmentMaker_NCB"))

            if (not cfgKeyStore.isInInput ('xAOD::MuonSegmentContainer', 'MuonSegments_NCB')):
                self.addAlg( CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg_NCB",SegmentContainerName="NCB_MuonSegments",xAODContainerName="NCB_MuonSegments") )

        if (not cfgKeyStore.isInInput ('xAOD::MuonSegmentContainer', 'MuonSegments')):
            self.addAlg( CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg") )
        
        if muonStandaloneFlags.doSegmentsOnly():
            return	                    
        # Tracks builder
        #
        # add the algorithm (which uses the MuonTrackSteering)
        # 
        TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder", TrackSteering = getPublicTool("MuonTrackSteering") )
        self.addAlg( TrackBuilder )
        
        self.registerOutputKey("MuonSpectrometerTracks",   self.MuPatTrackBuilder, "SpectrometerTrackOutputLocation")
        self.registerInputKey ("MuonSegments", self.MuPatTrackBuilder, "MuonSegmentCollection"   )

        
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
		 'trackAuthor'	   : 200
                 }
                  
