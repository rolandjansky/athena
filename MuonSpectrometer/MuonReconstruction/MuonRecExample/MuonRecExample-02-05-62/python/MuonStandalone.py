# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of Muon Spectrometer Standalone muon reconstruction"""

###############################################################
#
# Configuration for Standalone
#
#==============================================================
from AthenaCommon import CfgMgr

from MuonStandaloneFlags import muonStandaloneFlags,MuonboylikeStrategy,MoorelikeStrategy
from MuonRecFlags import muonRecFlags
from ConfiguredMuonRec import ConfiguredMuonRec

from MuonRecUtils import logMuon,ExtraFlags

from RecExConfig.RecFlags import rec

from AthenaCommon.CfgGetter import getPublicTool,getPublicToolClone,getPrivateTool
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
    extraFlags.setFlagDefault("materialSource",muonStandaloneFlags.materialSourcePatRec)
    extraFlags.setFlagDefault(muonStandaloneFlags.optimiseMomentumResolutionUsingChi2) # take name & value from JobProperty
    extraFlags.setFlagDefault(muonStandaloneFlags.strategy)
    extraFlags.setFlagDefault(muonStandaloneFlags.trackBuilder)
    extraFlags.setFlagDefault(muonStandaloneFlags.printSummary)
    extraFlags.setFlagDefault(muonStandaloneFlags.refinementTool)
                  
    if extraFlags.strategy:
        kwargs.setdefault("StrategyList", extraFlags.strategy)
    else:
        if extraFlags.trackBuilder == 'Muonboy':
            kwargs.setdefault("StrategyList", MuonboylikeStrategy)  
        elif extraFlags.trackBuilder == 'Moore':
            kwargs.setdefault("StrategyList", MoorelikeStrategy)  
    kwargs.setdefault("DoSummary", extraFlags.printSummary)
    kwargs.setdefault("OutputSingleStationTracks", True)
    kwargs.setdefault("HoleRecoveryTool",       "MuonEORecoveryTool")
    if "TrackBuilderTool" not in kwargs:
        if extraFlags.trackBuilder == 'Muonboy':
            kwargs["TrackBuilderTool"] = getPublicTool("MboyMuonTrackBuilder")
            kwargs["TrackRefinementTool"] = getPublicTool("MboyMuonTrackBuilder")
        elif extraFlags.trackBuilder == 'Moore':
          extraFlags.setFlagDefault('UseTrackingHistory',True)
          kwargs["TrackBuilderTool"] = getPublicToolClone("MooMuonTrackBuilder", "MooTrackBuilderTemplate",
                                                          extraFlags=extraFlags)
          if "TrackRefinementTool" not in kwargs:
              if extraFlags.refinementTool == 'Muonboy': 
                  kwargs["TrackRefinementTool"] = getPublicTool("MboyMuonTrackBuilder")
              elif extraFlags.refinementTool == 'Moore':
                  kwargs["TrackRefinementTool"] = getPublicTool("MooTrackBuilderTemplate")
    if extraFlags.trackBuilder == 'Moore':
        kwargs.setdefault("SegSeedQCut", 2)
        kwargs.setdefault("Seg2ndQCut", 1)
        if muonStandaloneFlags.segmentOrigin == 'Muonboy':
            kwargs.setdefault("MuonSegmentMerger",getPublicTool("MuonSegmentMerger"))
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
        # Temporarily hardcoded
        if muonStandaloneFlags.segmentOrigin == 'Muonboy':
            self.addOutputKey("MuonSegments", "MuonSegments")
        elif muonStandaloneFlags.segmentOrigin == 'Moore':
            self.addOutputKey("MuonSegments", "MooreSegments")
        elif muonStandaloneFlags.segmentOrigin == 'TruthTracking':
            self.addOutputKey("MuonSegments", "MuonSegments")
        elif muonStandaloneFlags.segmentOrigin == 'ThirdChain' :
            self.addOutputKey("MuonSegments", "MuonSegments")
        elif muonStandaloneFlags.segmentOrigin == 'Muon' :
            self.addOutputKey("MuonSegments", "MuonSegments")
        else:
            raise RuntimeError("Unknown segmentOrigin: %r" % (muonStandaloneFlags.segmentOrigin()) )

    def configure(self,keys=None):
        super(MuonStandalone,self).configure(keys)
        if not self.isEnabled(): return

        from AthenaCommon.BeamFlags import jobproperties
        beamFlags = jobproperties.Beam 
        if beamFlags.beamType()=='cosmics': 
            self.addAlg( CfgMgr.MooSegmentFinderAlg( "MuonSegmentMaker", 
                                                     SegmentFinder = getPrivateTool("MooSegmentFinder", WriteChamerT0s = True,
                                                                                    DoSummary = muonStandaloneFlags.printSummary() ), 
                                                     MuonSegmentOutputLocation = "MuonSegments", 
                                                     UseCSC = muonRecFlags.doCSCs(), 
                                                     UseMDT = muonRecFlags.doMDTs(), 
                                                     UseRPC = muonRecFlags.doRPCs(), 
                                                     UseTGC = muonRecFlags.doTGCs(), 
                                                     UseTGCPriorBC = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC(), 
                                                     UseTGCNextBC  = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC() )) 

        elif muonStandaloneFlags.segmentOrigin == 'Muon':
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
                                                         UseTGCNextBC  = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC() ))


        elif muonStandaloneFlags.segmentOrigin == 'Muonboy':
            TheThirdChainMboyMuonSegmentMakerTester = CfgMgr.MboyMuonSegmentMakerTester("MuonSegmentMaker", 
                                                                                        MboyMuonSegmentMaker = getPublicTool("ThirdChainMboyMuonSegmentMaker"),
                                                                                        SegmentCollectionLocation = "MuonSegments" )            
            self.addAlg( TheThirdChainMboyMuonSegmentMakerTester )

        self.addAlg( CfgMgr.xAODMaker__MuonSegmentCnvAlg("MuonSegmentCnvAlg") )
        
        if muonStandaloneFlags.doSegmentsOnly():
                return	                    
        # Tracks builder
        #
        # add the algorithm (which uses the MuonTrackSteering)
        # 
        if( muonStandaloneFlags.trackBuilder == 'Muonboy' or  muonStandaloneFlags.trackBuilder == 'Moore' ):
            TrackBuilder = CfgMgr.MuPatTrackBuilder("MuPatTrackBuilder", TrackSteering = getPublicTool("MuonTrackSteering") )
            if muonStandaloneFlags.trackBuilder == 'Muonboy':
                TrackBuilder.ExtrapolatedTrackOutputLocation = "ExtrapolatedMuonSpectrometerTracks"
                TrackBuilder.TrackToParticleTool =  CfgMgr.MuonboyToParticleTool("MuonboyToParticleTool")
                TrackBuilder.MuonBackTracker =  getPublicTool("MuonBackTracker") 
            self.addAlg( TrackBuilder )

            self.registerInputKey ("MuonSegments", self.MuPatTrackBuilder, "MuonSegmentCollection"   )
            self.registerOutputKey("MuonSpectrometerTracks",   self.MuPatTrackBuilder, "SpectrometerTrackOutputLocation")

            if muonStandaloneFlags.trackBuilder == 'Muonboy':
                self.registerOutputKey("MuonSpectrometerParticles",   self.MuPatTrackBuilder, "SpectrometerParticleOutputLocation")
                self.registerOutputKey("ExtrapolatedMuonSpectrometerTracks",   self.MuPatTrackBuilder, "ExtrapolatedTrackOutputLocation")
                self.registerOutputKey("ExtrapolatedMuonSpectrometerParticles",   self.MuPatTrackBuilder, "ExtrapolatedParticleOutputLocation")
        
        if muonStandaloneFlags.createTrackParticles():
            from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
            xAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg( name = "MuonStandaloneTrackParticleCnvAlg", 
                                                                      TrackParticleCreator = getPublicTool("MuonParticleCreatorTool"),
                                                                      TrackContainerName = "MuonSpectrometerTracks",
                                                                      xAODTrackParticlesFromTracksContainerName = "MuonSpectrometerTrackParticles",
                                                                      ConvertTrackParticles = False,
                                                                      ConvertTracks = True )
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
                  
