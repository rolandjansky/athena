#! /usr/bin/env python 

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def addSoftBVrt(sequencer=None,WP='Loose',postfix=''):
    coneForAvailableWP = {
        'Loose' : 0.75,
        'Medium' : 0.75,
        'Tight' : 0.6
    }

    from AthenaCommon import Logging
    SoftBVrtLog = Logging.logging.getLogger('SoftBVrt' + WP + postfix)

    if not WP in coneForAvailableWP:
        SoftBVrtLog.warning("Working point "+WP+" not available. Failing configuration.")
        return 

    if sequencer is None:
        SoftBVrtLog.warning("No sequencer provided. Skipping")
        #print a message and return 
        return
    from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkHasTruth
    if DerivationFrameworkHasTruth:
        from DerivationFrameworkMCTruth.MCTruthCommon import addHFAndDownstreamParticles
        if not hasattr(sequencer,'SoftVtxBHadMCTruthCommonHFAndDecaysKernel'):
            addHFAndDownstreamParticles(kernel=sequencer, addB=True, addC=False, generations=0, prefix='SoftVtxBHad')
        if not hasattr(sequencer,'SoftVtxCHadMCTruthCommonHFAndDecaysKernel'):
            addHFAndDownstreamParticles(kernel=sequencer, addB=False, addC=True, generations=0, prefix='SoftVtxCHad')

    # configure the tool to match the truth 
    from AthenaCommon.AppMgr import ToolSvc
    from ParticleJetTools.ParticleJetToolsConf import ParticleJetDeltaRLabelTool

    # prepare one matching tool. Can be the same one for all instances
    
    SoftBTruthMatchingTool = ParticleJetDeltaRLabelTool(      
        "SoftBJetDrLabeler",
        LabelName = "HadronConeExclTruthLabelID",
        DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
        BLabelName = "ConeExclBHadronsFinal",
        CLabelName = "ConeExclCHadronsFinal",
        TauLabelName = "ConeExclTausFinal",
        BParticleCollection = "SoftVtxBHadTruthHFWithDecayParticles",
        CParticleCollection = "SoftVtxCHadTruthHFWithDecayParticles",
        TauParticleCollection = "TruthLabelTausFinal",
        PartPtMin = 500.0,
        JetPtMin =     0.0,
        DRMax = 0.3,
        MatchMode = "MinDR"
    )                    

    ToolSvc += SoftBTruthMatchingTool

    from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder
    from AthenaCommon import CfgMgr
    if not hasattr(sequencer,"SoftB_BTagAug"):
        sequencer += CfgMgr.BTagVertexAugmenter("SoftB_BTagAug")

    SoftBJetSVFinderTool      = InDetVKalVxInJetFinder("SoftBJetSVFinder"+WP+postfix)
    ToolSvc += SoftBJetSVFinderTool
    SoftBJetSVFinderTool.ConeForTag = coneForAvailableWP[WP]
    from GaudiKernel.Configurable import INFO
    softTagAlg = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterTool" + WP+postfix,
                                                             OutputLevel=INFO, #DEBUG                                 
                                                         )

    softTagAlg.SVFinderName = 'SoftBJetSVFinder'+WP+postfix
    softTagAlg.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
    softTagAlg.TrackSelectionTool.CutLevel = "LoosePrimary"
    softTagAlg.OperatingPoint = WP
    softTagAlg.DoTruthMatching = True

    sequencer += softTagAlg


