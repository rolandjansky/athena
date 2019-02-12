# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of Muon Spectrometer Standalone muon reconstruction"""

###############################################################
#
# Configuration for Standalone
#
#==============================================================
from AthenaCommon import CfgMgr

from MuonStandaloneFlags import muonStandaloneFlags,MoorelikeStrategy
from MuonRecFlags import muonRecFlags
from ConfiguredMuonRec import ConfiguredMuonRec

from MuonRecUtils import logMuon,ExtraFlags

from RecExConfig.RecFlags import rec
from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone,getPrivateTool
from RecExConfig.ObjKeyStore                  import cfgKeyStore

import sys
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

        if muonRecFlags.doNSWNewThirdChain():
            getPublicTool("MuonLayerHoughTool")
            self.addAlg( CfgMgr.MuonLayerHoughAlg( "MuonLayerHoughAlg", PrintSummary = muonStandaloneFlags.printSummary()  ) )
            if not muonStandaloneFlags.patternsOnly():
                SegmentFinder = getPublicTool("MuonClusterSegmentFinderTool")
                Cleaner = getPublicToolClone("MuonTrackCleaner_seg","MuonTrackCleaner")
                Cleaner.PullCut = 3
                Cleaner.PullCutPhi = 3
                SegmentFinder.TrackCleaner = Cleaner
            # for test purposes allow parallel running of truth segment finding and new segment finder
                MuonSegmentFinderAlg = CfgMgr.MuonSegmentFinderAlg( "MuonSegmentMaker",SegmentCollectionName=SegmentLocation, 
                                                                    MuonPatternCalibration = getPublicTool("MuonPatternCalibration"),
                                                                    MuonPatternSegmentMaker = getPublicTool("MuonPatternSegmentMaker"),
                                                                    MuonTruthSummaryTool = None,
                                                                    PrintSummary = muonStandaloneFlags.printSummary() )
                if( muonRecFlags.doCSCs() ):
                    getPublicTool("CscSegmentUtilTool")
                    getPublicTool("Csc2dSegmentMaker")
                    getPublicTool("Csc4dSegmentMaker")
                else:
                    MuonSegmentFinderAlg.Csc2dSegmentMaker = None
                    MuonSegmentFinderAlg.Csc4dSegmentMaker = None
                self.addAlg( MuonSegmentFinderAlg )
        else:
            getPublicTool("MuonLayerHoughTool")
            self.addAlg( CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker",
                                                     SegmentFinder = getPublicToolClone("MuonSegmentFinder","MooSegmentFinder",
                                                                                        DoSummary=muonStandaloneFlags.printSummary()),
                                                     MuonSegmentOutputLocation = SegmentLocation,
                                                     UseCSC = muonRecFlags.doCSCs(),
                                                     UseMDT = muonRecFlags.doMDTs(),
                                                     UseRPC = muonRecFlags.doRPCs(),
                                                     UseTGC = muonRecFlags.doTGCs(),
                                                     UseTGCPriorBC = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC(),
                                                     UseTGCNextBC  = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC(),
                                                     doTGCClust = muonRecFlags.doTGCClusterSegmentFinding(),
                                                     doRPCClust = muonRecFlags.doRPCClusterSegmentFinding() ))

            self.addAlg(CfgMgr.MooSegmentFinderAlg("MuonSegmentMaker_NCB",
                                                     SegmentFinder = getPublicToolClone("MooSegmentFinder_NCB","MuonSegmentFinder",
                                                                                        DoSummary=False,
                                                                                        Csc2dSegmentMaker = getPublicToolClone("Csc2dSegmentMaker_NCB","Csc2dSegmentMaker",
                                                                                                                               segmentTool = getPublicToolClone("CscSegmentUtilTool_NCB",
                                                                                                                                                                "CscSegmentUtilTool",
                                                                                                                                                                TightenChi2 = False, 
                                                                                                                                                                IPconstraint=False)),
                                                                                        Csc4dSegmentMaker = getPublicToolClone("Csc4dSegmentMaker_NCB","Csc4dSegmentMaker",
                                                                                                                               segmentTool = getPublicTool("CscSegmentUtilTool_NCB")),
                                                                                        DoMdtSegments=False,DoSegmentCombinations=False,DoSegmentCombinationCleaning=False),
                                                     MuonPatternCombinationLocation = "NCB_MuonHoughPatternCombinations", 
                                                     MuonSegmentOutputLocation = "NCB_MuonSegments", 
                                                     UseCSC = muonRecFlags.doCSCs(),
                                                     UseMDT = False,
                                                     UseRPC = False,
                                                     UseTGC = False,
                                                     UseTGCPriorBC = False,
                                                     UseTGCNextBC  = False,
                                                     doTGCClust = False,
                                                     doRPCClust = False) )
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
            from AthenaCommon.Include import include
            include("InDetBeamSpotService/BeamCondSvc.py" )        
            from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg, xAODMaker__TrackCollectionCnvTool, xAODMaker__RecTrackParticleContainerCnvTool

            muonParticleCreatorTool = getPublicTool("MuonParticleCreatorTool")

            muonTrackCollectionCnvTool = xAODMaker__TrackCollectionCnvTool( name = "MuonTrackCollectionCnvTool", TrackParticleCreator = muonParticleCreatorTool )
            
            muonRecTrackParticleContainerCnvTool = xAODMaker__RecTrackParticleContainerCnvTool(name = "MuonRecTrackParticleContainerCnvTool", TrackParticleCreator = muonParticleCreatorTool )

            xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg", 
                                                                      TrackParticleCreator = muonParticleCreatorTool,
                                                                      TrackCollectionCnvTool=muonTrackCollectionCnvTool,
                                                                      RecTrackParticleContainerCnvTool = muonRecTrackParticleContainerCnvTool,
                                                                      TrackContainerName = "MuonSpectrometerTracks",
                                                                      xAODTrackParticlesFromTracksContainerName = "MuonSpectrometerTrackParticles",
                                                                      AODContainerName = "",
                                                                      AODTruthContainerName = "",
                                                                      xAODTruthLinkVector =  "",
                                                                      ConvertTrackParticles = False,
                                                                      ConvertTracks = True)

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
                  
