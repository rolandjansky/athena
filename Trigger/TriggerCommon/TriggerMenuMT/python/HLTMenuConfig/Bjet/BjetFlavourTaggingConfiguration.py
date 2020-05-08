#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable
#from AthenaCommon.Constants import DEBUG

def getFlavourTagging( inputJets, inputVertex, inputTracks ):

    acc = ComponentAccumulator()

    #filesInput="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1"
    kwargs = {}
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    #ConfigFlags.Input.isMC=False
    #ConfigFlags.Input.Files=filesInput.split(",")

    #from BTagging.BTaggingConf import Analysis__BTagTrackAugmenterAlg
    from AthenaConfiguration.ComponentFactory import CompFactory
    Analysis__BTagTrackAugmenterAlg=CompFactory.Analysis.BTagTrackAugmenterAlg
    bTagTrackAugmenter = Analysis__BTagTrackAugmenterAlg( "Analysis__BTagTrackAugmenterAlg" )
    bTagTrackAugmenter.TrackContainer = inputTracks
    bTagTrackAugmenter.PrimaryVertexContainer = inputVertex
    acc.addEventAlgo(bTagTrackAugmenter)

    from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
    from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
    from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
    from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
    

    TrackToJetAssociators = ['BTagTrackToJetAssociator', 'BTagTrackToJetAssociatorBB']
    kwargs['Release'] = '22'
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputTracks, 'BTagTrackToJetAssociator', **kwargs))
    kwargs['Release'] = '21'
    acc.merge(JetParticleAssociationAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputTracks, 'BTagTrackToJetAssociatorBB', **kwargs))
    del kwargs['Release'] 

    btagname = "HLT_BTagging_AntiKt4EMTopo"
    btagname_JFVtx = btagname + "JFVtx"
    btagname_SecVtx = btagname + "SecVtx"
    btagname_jetLink = btagname + ".jetLink"

    SecVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}
    for k, v in SecVertexingAndAssociators.items():
        if v not in TrackToJetAssociators:
            raise RuntimeError( v + ' is not configured')
        acc.merge(JetSecVtxFindingAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputVertex, k, v))
        JetSecVertexingAlg = JetSecVertexingAlgCfg(ConfigFlags, inputJets.replace("Jets",""), inputVertex, k, v)
        SecVertexingAlg = JetSecVertexingAlg.getEventAlgo(inputJets.replace("Jets","").lower() + "_" + k.lower() + "_secvtx") #If inputJets.replace("Jets","") is used in JetSecVertexingAlgCfg; Have to change it here aswell
        if k == "JetFitter":
            SecVertexingAlg.BTagJFVtxCollectionName = recordable( btagname_JFVtx )
        elif k == "SV1":
            SecVertexingAlg.BTagSVCollectionName = recordable( btagname_SecVtx )
        acc.merge(JetSecVertexingAlg)
    
    JetBTaggingAlg = JetBTaggingAlgCfg(ConfigFlags, JetCollection = inputJets.replace("Jets",""), PrimaryVertexCollectionName=inputVertex, TaggerList = ConfigFlags.BTagging.TrigTaggersList, SetupScheme = "Trig", SVandAssoc = SecVertexingAndAssociators, **kwargs)
    BTaggingAlg = JetBTaggingAlg.getEventAlgo((ConfigFlags.BTagging.OutputFiles.Prefix + inputJets.replace("Jets","") + ConfigFlags.BTagging.GeneralToolSuffix).lower()) #Defined in JetBTaggingAlgConfig.py; Ends up to be "btagging_hlt_inview"
    BTaggingAlg.BTaggingCollectionName = recordable( btagname )
    BTaggingAlg.BTagJFVtxCollectionName = btagname_JFVtx
    BTaggingAlg.BTagSVCollectionName = btagname_SecVtx
    BTaggingAlg.JetLinkName = btagname_jetLink
    acc.merge(JetBTaggingAlg)


    #from TrigBjetHypo.TrigBjetHypoConf import TrigBtagFexMT
    #Analysis__TrigBtagFexMT=CompFactory.Analysis.TrigBtagFexMT
    #bTagFex = Analysis__TrigBtagFexMT( "TrigBtagFexMT" )
    #bTagFex.JetKey = inputJets
    #bTagFex.PriVtxKey = inputVertex
    #bTagFex.TracksKey = inputTracks
    #bTagFex.d0Key = inputTracks + ".btagIp_d0"
    #bTagFex.z0SinThetaKey = inputTracks + ".btagIp_d0Uncertainty"
    #bTagFex.d0UncertaintyKey = inputTracks + ".btagIp_z0SinTheta"
    #bTagFex.z0SinThetaUncertaintyKey = inputTracks + ".btagIp_z0SinThetaUncertainty"
    #bTagFex.trackposKey = inputTracks + ".btagIp_trackDisplacement"
    #bTagFex.trackmomKey = inputTracks + ".btagIp_trackMomentum"
    #bTagFex.BTaggingKey = "HLT_BTagging_AntiKt4EMTopo"

    #from TrigBjetHypo.TrigBtagFexMTConfig import TrigBtagFexMT_OnlineMonitoring
    #bTagFex.MonTool = TrigBtagFexMT_OnlineMonitoring()
    #acc.addEventAlgo(bTagFex)


    return acc


