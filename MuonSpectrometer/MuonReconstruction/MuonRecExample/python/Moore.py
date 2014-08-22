# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Configuration of Moore standalone muon reconstruction"""

###############################################################
#
# Configuration for Moore
#
#==============================================================

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.BeamFlags import jobproperties
beamFlags = jobproperties.Beam
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon import CfgMgr

from RecExConfig.RecFlags import rec

from MooreFlags import mooreFlags
from MuonRecFlags import muonRecFlags
from ConfiguredMuonRec import ConfiguredMuonRec

from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,getService,getServiceClone

from MuonRecUtils import logMuon,ConfiguredBase,ExtraFlags
#==============================================================

# call  setDefaults to update flags
muonRecFlags.setDefaults()
mooreFlags.setDefaults()


class Moore(ConfiguredMuonRec):
    def __init__(self,**kwargs):
        ConfiguredMuonRec.__init__(self,"Moore",**kwargs)
        # setup minimum config needed to get Storegate keys
        # full setup is done in configure()

        # keys for segment building stage
        self.addOutputKey("PhiPatterns" , "MuonCleanPhiPatterns")     # hardcoded in C++
        self.addOutputKey("CscSegments" , "Csc4dSegmentCombinations") # C++ default
        self.addOutputKey("MuonPatterns", "MuonHoughPatternCombinations") # C++ default
        self.addOutputKey("MuonSegmentCombinations",                "MooreSegmentCombinations") # C++ default
        self.addOutputKey("MuonSegmentCombinationsForCalibration" , "MdtSegmentCombinations") # C++ default
        self.addOutputKey("MuonSegments" ,                          "MooreSegments") # C++ default
        # keys for tracks building stage
        self.addOutputKey("MuonTrackParticles" , "MooreTrackParticles") # C++ default
        self.addOutputKey("MuonTracks" ,         "MooreTracks") # C++ default


    def configure(self,keys=None):
        super(Moore,self).configure(keys)
        if not self.isEnabled(): return
        #
        # Segment builder
        #
        self.addAlg( CfgMgr.MooSegmentFinderAlg( "MooSegmentMaker",
                                                 SegmentFinder = getPrivateTool("MooSegmentFinder", WriteChamerT0s = True),
                                                 UseCSC = muonRecFlags.doCSCs(),
                                                 UseMDT = muonRecFlags.doMDTs(),
                                                 UseRPC = muonRecFlags.doRPCs(),
                                                 UseTGC = muonRecFlags.doTGCs(),
                                                 UseTGCPriorBC = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC(),
                                                 UseTGCNextBC  = muonRecFlags.doTGCs() and muonRecFlags.useTGCPriorNextBC() ))
        
        # register some data keys, so configurable properties get updated
        # intermediate results of tool
        SegmentFinderTool = self.MooSegmentMaker.SegmentFinder # shortcut
        self.registerOutputKey("MuonPatterns"                         ,self.MooSegmentMaker,"MuonPatternCombinationLocation")
        self.registerOutputKey("CscSegments"                          ,SegmentFinderTool,"Csc4dSegmentCombinationLocation")
        self.registerOutputKey("MuonSegmentCombinationsForCalibration",SegmentFinderTool,"MdtSegmentCombinationLocation")
        # final result of alg
        self.registerOutputKey("MuonSegmentCombinations", self.MooSegmentMaker, "MuonSegmentCombinationOutputLocation")
        self.registerOutputKey("MuonSegments", self.MooSegmentMaker, "MuonSegmentOutputLocation")

        #
        # Tracks builder
        #
        if not mooreFlags.doSegmentsOnly():
            # run Moore with the new steering algorithm
            if mooreFlags.doNewMoore():
                # new Moore Algorithm
                TrackSteeringTool = getPublicTool("MooreTrackSteering")
                TrackBuilder = CfgMgr.MuPatTrackBuilder("MuonCombiTrackBuilder", TrackSteering = TrackSteeringTool)
                self.addAlg( TrackBuilder )

                self.registerInputKey ("MuonSegments", self.MuonCombiTrackBuilder, "MuonSegmentCollection"   )
                self.registerOutputKey("MuonTracks",   self.MuonCombiTrackBuilder, "SpectrometerTrackOutputLocation")
                
            else:
                TrackSteeringTool = getPublicTool("MooTrackSteering")
                self.addAlg( CfgMgr.MuonCombiTrackBuilder("MuonCombiTrackBuilder", TrackSteering = TrackSteeringTool) )
                self.registerInputKey ("MuonSegmentCombinations", self.MuonCombiTrackBuilder, "MuonSegmentCombinationInputLocation"   )
                self.registerOutputKey("MuonTracks",              self.MuonCombiTrackBuilder, "TrackOutputLocation")





    def getCalibConfig(self):
        """Get the configuration to configure Calibration Ntuple output"""
        doTracks = not mooreFlags.doSegmentsOnly()
        if doTracks:
            tracksKey = self.dataKey("MuonTracks")
        else:
            tracksKey = ""
            
        return { 'eventTag'         : "Moore", 
                 'patternsKey'      : self.dataKey("MuonPatterns"), 
                 'segmentsKey'      : self.dataKey("MuonSegments"), 
                 'tracksKey'        : tracksKey,
		 'doPhi'	    : True,
		 'segmentAuthor'    : 4,
		 'trackAuthor'	  : 0 }
 





