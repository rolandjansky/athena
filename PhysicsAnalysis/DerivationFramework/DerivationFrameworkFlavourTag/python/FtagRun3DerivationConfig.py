# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CFElements import findAllAlgorithms

from GaudiKernel.Configurable import WARNING

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaCommon.Configurable import ConfigurableRun3Behavior
from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from AthenaConfiguration.ComponentFactory import CompFactory

# these files are sloppy with imports, see ATLASRECTS-6635
with ConfigurableRun3Behavior():
    from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
    from BTagging.BTagRun3Config import BTagAlgsCfg

# this is where you add the new trainings!
derivationTrainingMap={
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

# for backward compatability
def FtagJetCollection(jetcol, seq, pvCol='PrimaryVertices', OutputLevel=WARNING):
    FtagJetCollections([jetcol], seq, [pvCol], OutputLevel)

# this should be able to tag a few collections
def FtagJetCollections(jetcols, seq, pvCols=[], OutputLevel=WARNING):

    if len(pvCols) != len(jetcols):
        if pvCols:
            raise ValueError('PV collection length is not the same as Jets')
        pvCols=['PrimaryVertices']*len(jetcols)

    with ConfigurableRun3Behavior():

        from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

        acc = ComponentAccumulator()

        # TODO: For some reason we can't use JetTagCalibCfg here, like
        # we do in reconstruction, retagging, and trigger. It seems to
        # break something in the database. Hopefully we'll rewrite the
        # derivations in pure ComponentAccumulator code, which seems
        # to support JetTagCalibCfg.
        _setupCondDb(cfgFlags)

        if 'AntiKt4EMTopoJets' in jetcols:
            acc.merge(
                RenameInputContainerEmTopoHacksCfg('oldAODVersion')
            )

        if 'AntiKt4EMPFlowJets' in jetcols and cfgFlags.BTagging.Trackless:
            acc.merge(
                RenameInputContainerEmPflowHacksCfg('tracklessAODVersion')
            )

        for jetcol,pvCol in zip(jetcols, pvCols):
            acc.merge(getFtagComponent(
                cfgFlags,
                jetcol,
                pvCol,
            ))

    # Unpack ComponentAccumulator configuration into the derivation
    # sequence.
    algs = findAllAlgorithms(acc.getSequence("AthAlgSeq"))
    for alg in algs:
        seq += conf2toConfigurable(alg)
    acc.wasMerged()

    # This part would merge JetTagCalibCfg, if we ever get it to
    # work. Unfortunately calling this messes up the muon conditions
    # algs.
    #
    # you can't use accumulator.wasMerged() here because the above
    # code only merged the algorithms. Instead we rely on this hacky
    # looking construct.
    #
    # acc._sequence = []
    # appendCAtoAthena(acc)


# this returns a component accumulator, which is merged across jet
# collections in FtagJetCollections above
def getFtagComponent(cfgFlags, jetcol, pvCol):

    jetcol_name_without_Jets = jetcol.replace('Jets','')
    track_collection = 'InDetTrackParticles'
    if cfgFlags.BTagging.Pseudotrack:
        track_collection = 'InDetPseudoTrackParticles'

    acc = ComponentAccumulator()
    acc.merge(BTagTrackAugmenterAlgCfg(
        cfgFlags,
        TrackCollection=track_collection,
        PrimaryVertexCollectionName=pvCol,
    ))

    acc.addEventAlgo(CompFactory.FlavorTagDiscriminants.TrackLeptonDecoratorAlg(
        'TrackLeptonDecoratorAlg',
        trackContainer=track_collection,
    ))

    nnList = derivationTrainingMap[jetcol_name_without_Jets]

    acc.merge(BTagAlgsCfg(
        inputFlags=cfgFlags,
        JetCollection=jetcol_name_without_Jets,
        nnList=nnList,
        trackCollection=track_collection,
        primaryVertices=pvCol,
    ))

    return acc


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


# hack function to make the database work
def _setupCondDb(cfgFlags):
    """
    TODO: replace this with JetTagCalibCfg
    """
    from AthenaCommon.AppMgr import athCondSeq
    if not hasattr(athCondSeq,"JetTagCalibCondAlg"):
        Aliases = cfgFlags.BTagging.calibrationChannelAliases
        grades= cfgFlags.BTagging.Grades
        RNNIPConfig = {'rnnip':''}

        JetTagCalibCondAlg=CompFactory.Analysis.JetTagCalibCondAlg
        jettagcalibcondalg = "JetTagCalibCondAlg"
        readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
        connSchema = "GLOBAL_OFL"
        if not cfgFlags.Input.isMC:
            readkeycalibpath = readkeycalibpath.replace(
                "/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
            connSchema = "GLOBAL"
        histoskey = "JetTagCalibHistosKey"
        from IOVDbSvc.CondDB import conddb

        conddb.addFolder(
            connSchema,
            readkeycalibpath,
            className='CondAttrListCollection')
        JetTagCalib = JetTagCalibCondAlg(
            jettagcalibcondalg,
            ReadKeyCalibPath=readkeycalibpath,
            HistosKey = histoskey,
            taggers = cfgFlags.BTagging.taggerList,
            channelAliases = Aliases,
            IP2D_TrackGradePartitions = grades,
            RNNIP_NetworkConfig = RNNIPConfig)

        athCondSeq+=conf2toConfigurable( JetTagCalib, indent="  " )

