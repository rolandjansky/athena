# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import findAllAlgorithms

from GaudiKernel.Configurable import WARNING

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from AthenaConfiguration.ComponentFactory import CompFactory

# for backward compatability
def FtagJetCollection(jetcol, seq, pvCol='PrimaryVertices', OutputLevel=WARNING):
    FtagJetCollections([jetcol], seq, [pvCol], OutputLevel)

# this should be able to tag a few collections
def FtagJetCollections(jetcols, seq, pvCols=[], OutputLevel=WARNING):

    if len(pvCols) != len(jetcols):
        if pvCols:
            print( 'FtagJetCollections:  PV collection length is not the same as Jets using PrimaryVertices for all' )
        pvCols=['PrimaryVertices']*len(jetcols)
    
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    taggerlist = ['IP2D', 'IP3D', 'SV1', 'JetFitterNN']

    #Add the Flipped taggers
    if cfgFlags.BTagging.RunFlipTaggers is True:
        fliptaggerlist = ['IP2DNeg', 'IP3DNeg','IP2DFlip', 'IP3DFlip']
        for ele in fliptaggerlist:
            taggerlist.append(ele)


    
    setupCondDb(cfgFlags, taggerlist)

    acc = ComponentAccumulator()

    if 'AntiKt4EMTopoJets' in jetcols:
        acc.merge(RenameInputContainerEmTopoHacksCfg('oldAODVersion'))

    if 'AntiKt4EMPFlowJets' in jetcols and cfgFlags.BTagging.Trackless:
        acc.merge(RenameInputContainerEmPflowHacksCfg('tracklessAODVersion'))

    for jetcol,pvCol in zip(jetcols, pvCols):
        if 'AntiKt4EMPFlowJets' in jetcols and cfgFlags.BTagging.Trackless:
            continue
        acc.merge(getFtagComponent(cfgFlags, jetcol, taggerlist, pvCol, OutputLevel))

    Configurable.configurableRun3Behavior=0
    algs = findAllAlgorithms(acc.getSequence("AthAlgSeq"))
    for alg in algs:

        seq += conf2toConfigurable(alg)

    acc.wasMerged()


# this returns a component accumulator, which is merged across jet
# collections in FtagJetCollections above
def getFtagComponent(cfgFlags, jetcol, taggerlist, pvCol='PrimaryVertices', OutputLevel=WARNING):

    from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
    from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
    from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
    from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
    from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
    from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
    from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg

    jetcol_name_without_Jets = jetcol.replace('Jets','')
    BTaggingCollection = cfgFlags.BTagging.OutputFiles.Prefix + jetcol_name_without_Jets

    track_collection = 'InDetTrackParticles'
    if cfgFlags.BTagging.Pseudotrack:
        track_collection = 'InDetPseudoTrackParticles'
    muon_collection = 'Muons'

    acc = ComponentAccumulator()

    acc.merge(JetParticleAssociationAlgCfg(cfgFlags, jetcol, track_collection, "TracksForBTagging"))
    acc.merge(JetParticleAssociationAlgCfg(cfgFlags, jetcol, muon_collection, "MuonsForBTagging"))

    SecVertexers = [ 'JetFitter' , 'SV1']

    if cfgFlags.BTagging.RunFlipTaggers is True:
        SecVertexersFlip = ['JetFitterFlip','SV1Flip']
        for ele in SecVertexersFlip:
            SecVertexers.append(ele)

    for sv in SecVertexers:
        acc.merge(JetSecVtxFindingAlgCfg(cfgFlags, jetcol_name_without_Jets, pvCol, sv, "TracksForBTagging"))
        acc.merge(JetSecVertexingAlgCfg(cfgFlags, BTaggingCollection, jetcol_name_without_Jets, track_collection, pvCol, sv))


    acc.merge(JetBTaggingAlgCfg( \
        cfgFlags
      , BTaggingCollection = BTaggingCollection
      , JetCollection = jetcol_name_without_Jets
      , PrimaryVertexCollectionName=pvCol
      , TaggerList = taggerlist
      , SecVertexers = SecVertexers
      , Tracks = "TracksForBTagging"
      , Muons = "MuonsForBTagging"
      )
    )


    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
            'BTagging/201903/rnnip/antikt4empflow/network.json',
            'BTagging/201903/dl1r/antikt4empflow/network.json',
            'BTagging/20210517/dipsLoose/antikt4empflow/network.json', #first r22 trainings
            'BTagging/20210517/dips/antikt4empflow/network.json',
            'BTagging/20210528r22/dl1d/antikt4empflow/network.json',
            'BTagging/20210519r22/dl1r/antikt4empflow/network.json', 
            'BTagging/20210729/dipsLoose/antikt4empflow/network.json', #new r22 trainings
            'BTagging/20210729/dips/antikt4empflow/network.json',
            'BTagging/20210824r22/dl1dLoose/antikt4empflow/network.json', #“recommended tagger” which is DL1dLoose20210824r22 named DL1dv00 in EDM
            'BTagging/20210824r22/dl1d/antikt4empflow/network.json',
            'BTagging/20210824r22/dl1r/antikt4empflow/network.json',
        ],
        'AntiKt4PFlowCustomVtx': [
            'BTagging/201903/rnnip/antikt4empflow/network.json',
            'BTagging/201903/dl1r/antikt4empflow/network.json',
        ],
        'AntiKt4EMTopo': [
            'BTagging/201903/rnnip/antikt4empflow/network.json',
            'BTagging/201903/dl1r/antikt4empflow/network.json',
            'BTagging/20210517/dipsLoose/antikt4empflow/network.json', #first r22 trainings
            'BTagging/20210517/dips/antikt4empflow/network.json',
            'BTagging/20210528r22/dl1d/antikt4empflow/network.json',
            'BTagging/20210519r22/dl1r/antikt4empflow/network.json', 
            'BTagging/20210729/dipsLoose/antikt4empflow/network.json', #new r22 trainings
            'BTagging/20210729/dips/antikt4empflow/network.json',
            'BTagging/20210824r22/dl1dLoose/antikt4empflow/network.json', #“recommended tagger” which is DL1dLoose20210824r22 named DL1dv00 in EDM
            'BTagging/20210824r22/dl1d/antikt4empflow/network.json',
            'BTagging/20210824r22/dl1r/antikt4empflow/network.json',
        ],
        'AntiKtVR30Rmax4Rmin02Track': [
            'BTagging/201903/rnnip/antiktvr30rmax4rmin02track/network.json', 
            'BTagging/201903/dl1r/antiktvr30rmax4rmin02track/network.json',
            'BTagging/20210517/dipsLoose/antikt4empflow/network.json', #first r22 trainings
            'BTagging/20210517/dips/antikt4empflow/network.json',
            'BTagging/20210528r22/dl1d/antikt4empflow/network.json',
            'BTagging/20210519r22/dl1r/antikt4empflow/network.json', 
            'BTagging/20210729/dipsLoose/antikt4empflow/network.json', #new r22 trainings
            'BTagging/20210729/dips/antikt4empflow/network.json',
            'BTagging/20210824r22/dl1dLoose/antikt4empflow/network.json', #“recommended tagger” which is DL1dLoose20210824r22 named DL1dv00 in EDM
            'BTagging/20210824r22/dl1d/antikt4empflow/network.json',
            'BTagging/20210824r22/dl1r/antikt4empflow/network.json',
        ]
    }

    acc.merge(BTagTrackAugmenterAlgCfg(cfgFlags,  TrackCollection = track_collection, PrimaryVertexCollectionName = pvCol))

    acc.merge(BTagHighLevelAugmenterAlgCfg(
        cfgFlags,
        JetCollection=jetcol_name_without_Jets,
        BTagCollection=BTaggingCollection,
        TrackCollection=track_collection,
        Associator='BTagTrackToJetAssociator',
    ))
    
    for jsonfile in postTagDL2JetToTrainingMap[jetcol_name_without_Jets]:
        acc.merge(HighLevelBTagAlgCfg(cfgFlags, BTaggingCollection=BTaggingCollection, TrackCollection=track_collection, NNFile=jsonfile) )
        # Schedule NN-based IP 'flip' taggers (rnnipflip and dipsflip) - this should for the moment only run on the low-level taggers and not on 'dl1x'
        # FlipConfig is "STANDARD" by default - for flip tagger set up with option "NEGATIVE_IP_ONLY" (flip sign of d0 and use only (flipped) positive d0 values)
        if 'dips' in jsonfile or 'rnnip' in jsonfile:
            if cfgFlags.BTagging.RunFlipTaggers is True and 'dl1' not in jsonfile:
                acc.merge(HighLevelBTagAlgCfg(cfgFlags, BTaggingCollection=BTaggingCollection, TrackCollection=track_collection, NNFile=jsonfile,FlipConfig="NEGATIVE_IP_ONLY") )

    return acc

# this probably only has to happen once
def setupCondDb(cfgFlags, taggerlist):
    from AthenaCommon.AppMgr import athCondSeq
    if not hasattr(athCondSeq,"JetTagCalibCondAlg"):
        CalibrationChannelAliases = ["AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo","AntiKtVR30Rmax4Rmin02PV0Track->AntiKtVR30Rmax4Rmin02PV0Track,AntiKt4EMPFlow,AntiKt4EMTopo","AntiKt4PFlowCustomVtx->AntiKt4EMTopo"]
        grades= cfgFlags.BTagging.Grades
        RNNIPConfig = {'rnnip':''}

        JetTagCalibCondAlg=CompFactory.Analysis.JetTagCalibCondAlg
        jettagcalibcondalg = "JetTagCalibCondAlg"
        readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
        connSchema = "GLOBAL_OFL"
        if not cfgFlags.Input.isMC:
            readkeycalibpath = readkeycalibpath.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
            connSchema = "GLOBAL"
        histoskey = "JetTagCalibHistosKey"
        from IOVDbSvc.CondDB import conddb

        conddb.addFolder(connSchema, readkeycalibpath, className='CondAttrListCollection')
        JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = taggerlist,
            channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)

        athCondSeq+=conf2toConfigurable( JetTagCalib, indent="  " )



# Valerio's magic hacks for emtopo
def RenameInputContainerEmTopoHacksCfg(suffix):

    acc=ComponentAccumulator()

    #Delete BTagging container read from input ESD
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.BTagTrackToJetAssociator->AntiKt4EMTopoJets.BTagTrackToJetAssociator_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.JFVtx->AntiKt4EMTopoJets.JFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.SecVtx->AntiKt4EMTopoJets.SecVtx_' + suffix]

    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.btaggingLink->AntiKt4EMTopoJets.btaggingLink_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo->BTagging_AntiKt4EMTopo_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.->BTagging_AntiKt4EMTopo_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexContainer#BTagging_AntiKt4EMTopoSecVtx->BTagging_AntiKt4EMTopoSecVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexAuxContainer#BTagging_AntiKt4EMTopoSecVtxAux.->BTagging_AntiKt4EMTopoSecVtx_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexContainer#BTagging_AntiKt4EMTopoJFVtx->BTagging_AntiKt4EMTopoJFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexAuxContainer#BTagging_AntiKt4EMTopoJFVtxAux.->BTagging_AntiKt4EMTopoJFVtx_' + suffix+"Aux."]
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))
    return acc
def RenameInputContainerEmPflowHacksCfg(suffix):

    acc=ComponentAccumulator()

    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMPFlowJets.BTagTrackToJetAssociator->AntiKt4EMPFlowJets.BTagTrackToJetAssociator_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMPFlowJets.JFVtx->AntiKt4EMPFlowJets.JFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMPFlowJets.SecVtx->AntiKt4EMPFlowJets.SecVtx_' + suffix]

    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMPFlowJets.btaggingLink->AntiKt4EMPFlowJets.btaggingLink_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingContainer#BTagging_AntiKt4EMPFlow->BTagging_AntiKt4EMPFlow_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMPFlowAux.->BTagging_AntiKt4EMPFlow_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexContainer#BTagging_AntiKt4EMPFlowSecVtx->BTagging_AntiKt4EMPFlowSecVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexAuxContainer#BTagging_AntiKt4EMPFlowSecVtxAux.->BTagging_AntiKt4EMPFlowSecVtx_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexContainer#BTagging_AntiKt4EMPFlowJFVtx->BTagging_AntiKt4EMPFlowJFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexAuxContainer#BTagging_AntiKt4EMPFlowJFVtxAux.->BTagging_AntiKt4EMPFlowJFVtx_' + suffix+"Aux."]
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))
    return acc
