# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuGirl.MuGirlFlags import muGirlFlags
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec
from MuGirl.MuGirlConf import MuGirlNS__MuGirlRecoTool
from AthenaCommon.Configurable import *
from AthenaCommon.AppMgr import ToolSvc
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample import MuonRecTools
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.CfgGetter import getPublicTool
beamFlags = jobproperties.Beam

class MuGirlRecoConfig(MuGirlNS__MuGirlRecoTool):
    __slots__ = {}

    def __init__(self, name = Configurable.DefaultName):
        super(MuGirlRecoConfig, self).__init__(name)
        self.MuGirlParticleCreatorTool = None

        global ToolSvc

        self.doNTuple           = muGirlFlags.doNTuple()
        self.doMSRefit          = muGirlFlags.doMSRefit()
        self.doTruth            = rec.doTruth() and muGirlFlags.doNTuple()
        self.PtLowerLimit       = muGirlFlags.PtLowerLimit() 
        #self.momentumLowerLimit = muGirlFlags.momentumLowerLimit() 
        self.nIDhits            = muGirlFlags.nIDhits() 

        #self.doMuonFeature      = True
        #self.doSARefit          = False
        #if muGirlFlags.doLHR():
        #from TrackToCalo.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory
        #MuGirlExtrapolTrackToCaloTool = ExtrapolTrackToCaloToolFactory(depth="showerdefault")
        #ToolSvc += MuGirlExtrapolTrackToCaloTool
        #self.ExtrapolTrackToCaloTool = MuGirlExtrapolTrackToCaloTool

        from MuGirlStau.MuGirlStauConfig import MuGirlNS__StauToolConfig
        StauTool = MuGirlNS__StauToolConfig(name = 'MuGirlStauTool')
        ToolSvc += StauTool
        self.MuGirlStauTool = StauTool
        from MuGirlCandidate.MuGirlCandidateConfig import MuGirlNS__CandidateToolConfig
        MuGirlCandidateTool = MuGirlNS__CandidateToolConfig(name = 'MuGirlCandidateTool')
        ToolSvc += MuGirlCandidateTool
        MuGirlCandidateTool.doCSC =  muonRecFlags.doCSCs()
        MuGirlCandidateTool.writeChamberT0s =  muGirlFlags.writeChamberT0s()
        self.MuGirlCandidate = MuGirlCandidateTool
        MuGirlCandidateTool.MuidCaloEnergy=ToolSvc.MuidCaloEnergyTool

        from MuGirlPerformanceTools.MuGirlPerformanceToolsConf import MuGirlNS__PerformanceTruthTool
        MuGirlPerformanceTruthTool = MuGirlNS__PerformanceTruthTool(name = 'MuGirlPerformanceTruthTool')
        detDesc = globalflags.DetDescrVersion()
        if detDesc.startswith("DC") or detDesc.startswith("Rome") or detDesc.startswith("ATLAS-DC3"):
            MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryRecord"
        else:
            MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryLayer"
        ToolSvc += MuGirlPerformanceTruthTool
        self.PerformanceTruthTool = MuGirlPerformanceTruthTool

        from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
        MuGirlParticleCreatorTool = MuGirlNS__MuGirlParticleCreatorTool(name = 'MuGirlParticleCreatorTool')
        MuGirlParticleCreatorTool.doNTuple  = self.doNTuple
        MuGirlParticleCreatorTool.doTruth   = self.doTruth
        MuGirlParticleCreatorTool.doMSRefit = self.doMSRefit
        MuGirlParticleCreatorTool.PerformanceTruthTool = MuGirlPerformanceTruthTool
        MuGirlParticleCreatorTool.MuidCaloEnergyTool=ToolSvc.MuidCaloEnergyTool
        if beamFlags.beamType() == 'cosmics':
           MuGirlParticleCreatorTool.inDetTrackParticlesLocation = 'TrackParticleCandidate_split'
        ToolSvc += MuGirlParticleCreatorTool
        self.MuGirlParticleCreatorTool = MuGirlParticleCreatorTool
        
        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        ToolSvc += MuGirlNS__GlobalFitTool(name = 'GlobalFitTool')
        GlobalFitTool = MuGirlNS__GlobalFitTool(name = 'GlobalFitTool')
        self.MuGirlGlobalFitTool = GlobalFitTool
        StauGlobalFitTool= MuGirlNS__GlobalFitTool(name = 'StauGlobalFitTool')
        ToolSvc += StauGlobalFitTool
        StauTool.StauGlobalFitTool = StauGlobalFitTool
        ToolSvc += MuGirlNS__GlobalFitTool(name = 'MSGlobalFitTool')
        MSGlobalFitTool = MuGirlNS__GlobalFitTool(name = 'MSGlobalFitTool')
        MSGlobalFitTool.doSAfit = True 
        self.MuGirlMSGlobalFitTool = MSGlobalFitTool

        from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags
 

        from AthenaCommon.CfgGetter import getPublicToolClone
        from MuonRecExample.MuonStandaloneFlags import MoorelikeStrategy
        
        #####                       clone instance name     original instance name ####
        mtst = getPublicToolClone( "MuGirl_TrackSteering", "MuonTrackSteering",
                                   #DoSummary = muGirlFlags.printSummary(),  # or similar if that exists. Otherwise set to False or skip this line
                                   SegSeedQCut = 2,
                                   Seg2ndQCut = 2)
        
        ToolSvc.MSGlobalFitTool.TrackSteering = mtst
        ToolSvc.GlobalFitTool.TrackSteering = mtst

   
        from MuGirlSelection.MuGirlSelectionConf import MuGirlNS__ANNSelectionTool
        MuGirlSelectionTool = MuGirlNS__ANNSelectionTool("MuGirlSelectionTool")
        ToolSvc += MuGirlSelectionTool
        MuGirlSelectionTool.CandidateTool = MuGirlCandidateTool
        self.ANNSelectionTool = MuGirlSelectionTool
        #if rec.Commissioning(): 
        # commissioning config 
        #    self.PtLowerLimit=0.0     
        #    self.momentumLowerLimit=2000.0     

        from MuonCombinedRecExample import CombinedMuonTrackSummary 
        self.IdTrackSelectorTool = getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")
        #from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool 
        #MuGirlIdTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "MuGirlIdTrackSelectorTool") 
        #MuGirlIdTrackSelectorTool.pTMin      = self.PtLowerLimit 
        ###MuGirlIdTrackSelectorTool.pMin       = self.momentumLowerLimit 
        #MuGirlIdTrackSelectorTool.nHitSi     = self.nIDhits 
        #MuGirlIdTrackSelectorTool.nHitBLayer = 0 
        ###MuGirlIdTrackSelectorTool.TrackSummaryTool = MuonRecTools.getPublicTool("MuidTrackSummaryTool") 
        #MuGirlIdTrackSelectorTool.TrackSummaryTool = ToolSvc.CombinedMuonTrackSummary
        #MuGirlIdTrackSelectorTool.nHitPix    = 0 
        #MuGirlIdTrackSelectorTool.IPd0Max    = 999. 
        #MuGirlIdTrackSelectorTool.IPz0Max    = 999. 
        #ToolSvc += MuGirlIdTrackSelectorTool 
        #self.IdTrackSelectorTool = MuGirlIdTrackSelectorTool 
        print self.IdTrackSelectorTool 
        print "MuGirlRecoConfig: momentum, pt thresholds %s" % self.PtLowerLimit

    def __setattr__(self, name, value):
        global ToolSvc
        super(MuGirlRecoConfig, self).__setattr__(name, value)
        if name in ['doNTuple', 'doTruth', 'doMSRefit', 'PerformanceTruthTool'] and hasattr(ToolSvc, 'MuGirlParticleCreatorTool'):
            print "MuGirlRecoConfig: MuGirlParticleCreatorTool.%s=%s" % (name, str(value))
            ToolSvc.MuGirlParticleCreatorTool.__setattr__(name, value)



class MuGirlTagConfig(MuGirlNS__MuGirlRecoTool):
    __slots__ = {}

    def __init__(self, name = Configurable.DefaultName):
        super(MuGirlTagConfig, self).__init__(name)
        self.MuGirlParticleCreatorTool = None

        global ToolSvc

        self.doNTuple           = muGirlFlags.doNTuple()
        self.doMSRefit          = muGirlFlags.doMSRefit()
        self.doTruth            = rec.doTruth() and muGirlFlags.doNTuple()
        self.PtLowerLimit       = muGirlFlags.PtLowerLimit() 
        #self.momentumLowerLimit = muGirlFlags.momentumLowerLimit() 
        self.nIDhits            = muGirlFlags.nIDhits() 

        #self.doMuonFeature      = True
        #self.doSAfit            = True
        #if muGirlFlags.doLHR():
        #from TrackToCalo.ExtrapolTrackToCaloToolBase import ExtrapolTrackToCaloToolFactory
        #MuGirlExtrapolTrackToCaloTool = ExtrapolTrackToCaloToolFactory(depth="showerdefault")
        #ToolSvc += MuGirlExtrapolTrackToCaloTool
        #self.ExtrapolTrackToCaloTool = MuGirlExtrapolTrackToCaloTool

        from MuGirlStau.MuGirlStauConfig import MuGirlNS__StauToolConfig
        StauTool = MuGirlNS__StauToolConfig(name = 'MuGirlTagStauTool')
        ToolSvc += StauTool
        self.MuGirlStauTool = StauTool
        from MuGirlCandidate.MuGirlCandidateConfig import MuGirlNS__CandidateToolConfig
        MuGirlCandidateTool = MuGirlNS__CandidateToolConfig(name = 'MuGirlTagCandidateTool')
        ToolSvc += MuGirlCandidateTool
        MuGirlCandidateTool.doCSC =  muonRecFlags.doCSCs()
        MuGirlCandidateTool.writeChamberT0s =  muGirlFlags.writeChamberT0s()
        self.MuGirlCandidate = MuGirlCandidateTool
        MuGirlCandidateTool.MuidCaloEnergy=ToolSvc.MuidCaloEnergyTool

        from MuGirlPerformanceTools.MuGirlPerformanceToolsConf import MuGirlNS__PerformanceTruthTool
        MuGirlPerformanceTruthTool = MuGirlNS__PerformanceTruthTool(name = 'MuGirlTagPerformanceTruthTool')
        detDesc = globalflags.DetDescrVersion()
        if detDesc.startswith("DC") or detDesc.startswith("Rome") or detDesc.startswith("ATLAS-DC3"):
            MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryRecord"
        else:
            MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryLayer"
        ToolSvc += MuGirlPerformanceTruthTool
        self.PerformanceTruthTool = MuGirlPerformanceTruthTool

        from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
        MuGirlParticleCreatorTool = MuGirlNS__MuGirlParticleCreatorTool(name = 'MuGirlTagParticleCreatorTool')
        MuGirlParticleCreatorTool.doNTuple  = self.doNTuple
        MuGirlParticleCreatorTool.doTruth   = self.doTruth
        MuGirlParticleCreatorTool.doMSRefit = self.doMSRefit
        MuGirlParticleCreatorTool.PerformanceTruthTool = MuGirlPerformanceTruthTool
        MuGirlParticleCreatorTool.MuidCaloEnergyTool=ToolSvc.MuidCaloEnergyTool
        if beamFlags.beamType() == 'cosmics':
           MuGirlParticleCreatorTool.inDetTrackParticlesLocation = 'TrackParticleCandidate_split'
        ToolSvc += MuGirlParticleCreatorTool
        self.MuGirlParticleCreatorTool = MuGirlParticleCreatorTool
        
        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        ToolSvc += MuGirlNS__GlobalFitTool(name = 'MuGirlTagGlobalFitTool')
        GlobalFitTool = MuGirlNS__GlobalFitTool(name = 'MuGirlTagGlobalFitTool')
        self.MuGirlGlobalFitTool = GlobalFitTool
        StauGlobalFitTool= MuGirlNS__GlobalFitTool(name = 'MuGirlTagStauGlobalFitTool')
        ToolSvc += StauGlobalFitTool
        StauTool.StauGlobalFitTool = StauGlobalFitTool
        ToolSvc += MuGirlNS__GlobalFitTool(name = 'MuGirlTagMSGlobalFitTool')
        MSGlobalFitTool = MuGirlNS__GlobalFitTool(name = 'MuGirlTagMSGlobalFitTool')
        MSGlobalFitTool.doSAfit = True 
        self.MuGirlMSGlobalFitTool = MSGlobalFitTool

        from MuonRecExample.MuonStandaloneFlags import muonStandaloneFlags


   
        from MuGirlSelection.MuGirlSelectionConf import MuGirlNS__ANNSelectionTool
        MuGirlSelectionTool = MuGirlNS__ANNSelectionTool("MuGirlTagSelectionTool")
        ToolSvc += MuGirlSelectionTool
        MuGirlSelectionTool.CandidateTool = MuGirlCandidateTool
        self.ANNSelectionTool = MuGirlSelectionTool
        #if rec.Commissioning(): 
        # commissioning config 
        #    self.PtLowerLimit=0.0     
        #    self.momentumLowerLimit=2000.0     

        from MuonCombinedRecExample import CombinedMuonTrackSummary 
        self.IdTrackSelectorTool = getPublicTool("MuonCombinedInDetDetailedTrackSelectorTool")
        #from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool 
        #MuGirlIdTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "MuGirlIdTrackSelectorTool") 
        #MuGirlIdTrackSelectorTool.pTMin      = self.PtLowerLimit 
        ###MuGirlIdTrackSelectorTool.pMin       = self.momentumLowerLimit 
        #MuGirlIdTrackSelectorTool.nHitSi     = self.nIDhits 
        #MuGirlIdTrackSelectorTool.nHitBLayer = 0 
        ###MuGirlIdTrackSelectorTool.TrackSummaryTool = MuonRecTools.getPublicTool("MuidTrackSummaryTool") 
        #MuGirlIdTrackSelectorTool.TrackSummaryTool = ToolSvc.CombinedMuonTrackSummary
        #MuGirlIdTrackSelectorTool.nHitPix    = 0 
        #MuGirlIdTrackSelectorTool.IPd0Max    = 999. 
        #MuGirlIdTrackSelectorTool.IPz0Max    = 999. 
        #ToolSvc += MuGirlIdTrackSelectorTool 
        #self.IdTrackSelectorTool = MuGirlIdTrackSelectorTool 
        print self.IdTrackSelectorTool 
        print "MuGirlTagConfig: momentum, pt thresholds %s" % self.PtLowerLimit

    def __setattr__(self, name, value):
        global ToolSvc
        super(MuGirlTagConfig, self).__setattr__(name, value)
        if name in ['doNTuple', 'doTruth', 'doMSRefit', 'PerformanceTruthTool'] and hasattr(ToolSvc, 'MuGirlTagParticleCreatorTool'):
            print "MuGirlTagConfig: MuGirlParticleCreatorTool.%s=%s" % (name, str(value))
            ToolSvc.MuGirlTagParticleCreatorTool.__setattr__(name, value)

