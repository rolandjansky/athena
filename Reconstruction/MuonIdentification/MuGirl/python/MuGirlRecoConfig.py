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
from AthenaCommon import CfgMgr

beamFlags = jobproperties.Beam

class MuGirlRecoConfig(MuGirlNS__MuGirlRecoTool):
    __slots__ = {}

    def __init__(self, name = Configurable.DefaultName, configureForTrigger = False, doStau = True ):
        super(MuGirlRecoConfig, self).__init__(name)
        self.MuGirlParticleCreatorTool = None

        global ToolSvc
        if configureForTrigger:
            print "MuGirlRecoConfig: configuring for trigger"

        self.doNTuple           = muGirlFlags.doNTuple()
        self.doTruth            = rec.doTruth() and muGirlFlags.doNTuple()
        self.PtLowerLimit       = muGirlFlags.PtLowerLimit() 
        self.doStau             = doStau

        from MuGirlCandidate.MuGirlCandidateConfig import MuGirlNS__CandidateToolConfig
        if configureForTrigger:
            MuGirlCandidateTool = MuGirlNS__CandidateToolConfig(name = 'TrigMuGirlCandidateTool')
        else:
            MuGirlCandidateTool = MuGirlNS__CandidateToolConfig(name = 'MuGirlCandidateTool')
        ToolSvc += MuGirlCandidateTool
        MuGirlCandidateTool.doCSC =  muonRecFlags.doCSCs()
        self.MuGirlCandidate = MuGirlCandidateTool
        if configureForTrigger:
            CaloEnergyTool = getPublicTool("TMEF_CaloEnergyTool")
            self.doLHR = False 
        else:
            CaloEnergyTool = ToolSvc.MuidCaloEnergyTool
            self.CaloMuonLikelihoodTool = getPublicTool("CaloMuonLikelihoodTool")
        MuGirlCandidateTool.MuidCaloEnergy=CaloEnergyTool

        from MuGirlPerformanceTools.MuGirlPerformanceToolsConf import MuGirlNS__PerformanceTruthTool
        MuGirlPerformanceTruthTool = MuGirlNS__PerformanceTruthTool(name = 'MuGirlPerformanceTruthTool')
        MuGirlPerformanceTruthTool.TrackRecordCollection = "MuonEntryLayer"
        ToolSvc += MuGirlPerformanceTruthTool
        self.PerformanceTruthTool = MuGirlPerformanceTruthTool

        from MuGirlParticleCreatorTool.MuGirlParticleCreatorToolConf import MuGirlNS__MuGirlParticleCreatorTool
        MuGirlParticleCreatorTool = MuGirlNS__MuGirlParticleCreatorTool(name = 'MuGirlParticleCreatorTool')
        MuGirlParticleCreatorTool.doNTuple  = self.doNTuple
        MuGirlParticleCreatorTool.doTruth   = self.doTruth
        MuGirlParticleCreatorTool.PerformanceTruthTool = MuGirlPerformanceTruthTool
        MuGirlParticleCreatorTool.MuidCaloEnergyTool=CaloEnergyTool
        if beamFlags.beamType() == 'cosmics':
           MuGirlParticleCreatorTool.inDetTrackParticlesLocation = 'TrackParticleCandidate_split'
        ToolSvc += MuGirlParticleCreatorTool
        self.MuGirlParticleCreatorTool = MuGirlParticleCreatorTool
        
        from MuGirlGlobalFit.MuGirlGlobalFitConf import MuGirlNS__GlobalFitTool
        GlobalFitTool = MuGirlNS__GlobalFitTool(name = 'GlobalFitTool')
        if configureForTrigger:
            GlobalFitTool.trackFitter = getPublicTool("TMEF_CombinedMuonTrackBuilder")
        ToolSvc += GlobalFitTool
        self.MuGirlGlobalFitTool = GlobalFitTool

        if doStau:
            from MuGirlStau.MuGirlStauConfig import MuGirlNS__StauToolConfig
            if configureForTrigger:
                StauTool = MuGirlNS__StauToolConfig(name = 'TrigMuGirlStauTool')
            else:
                StauTool = MuGirlNS__StauToolConfig(name = 'MuGirlStauTool')


            ToolSvc += StauTool
            self.MuGirlStauTool = StauTool
            StauTool.StauGlobalFitTool = GlobalFitTool

        from MuGirlSelection.MuGirlSelectionConf import MuGirlNS__ANNSelectionTool
        MuGirlSelectionTool = MuGirlNS__ANNSelectionTool("MuGirlSelectionTool")
        ToolSvc += MuGirlSelectionTool
        MuGirlSelectionTool.CandidateTool = MuGirlCandidateTool
        self.ANNSelectionTool = MuGirlSelectionTool

        print "MuGirlRecoConfig: momentum, pt thresholds %s" % self.PtLowerLimit

    def __setattr__(self, name, value):
        global ToolSvc
        super(MuGirlRecoConfig, self).__setattr__(name, value)
        if name in ['doNTuple', 'doTruth', 'doMSRefit', 'PerformanceTruthTool'] and hasattr(ToolSvc, 'MuGirlParticleCreatorTool'):
            print "MuGirlRecoConfig: MuGirlParticleCreatorTool.%s=%s" % (name, str(value))
            ToolSvc.MuGirlParticleCreatorTool.__setattr__(name, value)

