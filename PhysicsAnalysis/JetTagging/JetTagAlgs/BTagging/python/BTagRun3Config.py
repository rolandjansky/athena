# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg
from JetTagCalibration.JetTagCalibConfig import JetTagCalibCfg


def RenameInputContainerCfg(suffix):
    acc=ComponentAccumulator()

    #Delete BTagging container read from input ESD
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
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

def RenameHLTaggerCfg(JetCollection, Tagger, suffix):
    acc=ComponentAccumulator()
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_pu->BTagging_' + JetCollection + '.'+ Tagger + '_pu' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_pc->BTagging_' + JetCollection + '.'+ Tagger + '_pc' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_pb->BTagging_' + JetCollection + '.'+ Tagger + '_pb' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_ptau->BTagging_' + JetCollection + '.'+ Tagger + '_ptau' + suffix]
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))

    return acc


def PrepareStandAloneBTagCfg(inputFlags):
    result=ComponentAccumulator()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    result.merge(PoolReadCfg(inputFlags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    # get standard config for magnetic field - map and cache
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg( inputFlags ))

    #Beamspot conditions
    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    result.merge(BeamSpotCondAlgCfg(inputFlags))
    
    #load folders needed for Run2 ID alignment
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    return result

def BTagRecoSplitCfg(inputFlags, JetCollection = ['AntiKt4EMTopo','AntiKt4EMPFlow'], **kwargs):

    result=ComponentAccumulator()

    # Can only configure b-tagging for collisions; not cosmics, etc.
    if inputFlags.Beam.Type != 'collisions':
        return result

    taggerList = inputFlags.BTagging.run2TaggersList

    result.merge(JetTagCalibCfg(inputFlags, TaggerList = taggerList, **kwargs))

    SecVertexers = [ "JetFitter" , "SV1" ]
    for jc in JetCollection:
        result.merge(JetBTaggerSplitAlgsCfg(inputFlags, JetCollection = jc, TaggerList = taggerList, SecVertexers = SecVertexers, **kwargs))

    # the following is needed to reliably determine whether we're really being steered from an old-style job option
    # assume we're running CPython
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in result._allSequences:
            el.name = "TopAlg"

    return result


def JetBTaggerSplitAlgsCfg(inputFlags, JetCollection="", TaggerList=[], SecVertexers = [], SetupScheme="", **kwargs):

    result=ComponentAccumulator()
    jet = JetCollection

    timestamp = kwargs.get('TimeStamp', None)
    if not timestamp:
        timestamp = ['']
    else:
        del kwargs['TimeStamp']

    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
           'BTagging/201903/rnnip/antikt4empflow/network.json',
           'BTagging/201903/dl1r/antikt4empflow/network.json',
           'BTagging/20210729/dipsLoose/antikt4empflow/network.json', #new r22 trainings
           'BTagging/20210729/dips/antikt4empflow/network.json',
           'BTagging/20210824r22/dl1dLoose/antikt4empflow/network.json', #“recommended tagger” which is DL1dLoose20210824r22 named DL1dv00 in EDM
           'BTagging/20210824r22/dl1d/antikt4empflow/network.json',
           'BTagging/20210824r22/dl1r/antikt4empflow/network.json',
        ],
        'AntiKt4EMTopo': [
           'BTagging/201903/rnnip/antikt4empflow/network.json',
           'BTagging/201903/dl1r/antikt4empflow/network.json',
           'BTagging/20210729/dipsLoose/antikt4empflow/network.json', #new r22 trainings
           'BTagging/20210729/dips/antikt4empflow/network.json',
           'BTagging/20210824r22/dl1dLoose/antikt4empflow/network.json', #“recommended tagger” which is DL1dLoose20210824r22 named DL1dv00 in EDM
           'BTagging/20210824r22/dl1d/antikt4empflow/network.json',
           'BTagging/20210824r22/dl1r/antikt4empflow/network.json',
        ]
    }

    #Track Association
    result.merge(JetParticleAssociationAlgCfg(inputFlags, jet+'Jets', "InDetTrackParticles", "TracksForBTagging", **kwargs))
    result.merge(JetParticleAssociationAlgCfg(inputFlags, jet+'Jets', "Muons", "MuonsForBTagging", **kwargs))

    for sv in SecVertexers:
        result.merge(JetSecVtxFindingAlgCfg(inputFlags, jet, "PrimaryVertices", sv, "TracksForBTagging"))
        result.merge(JetSecVertexingAlgCfg(inputFlags, inputFlags.BTagging.OutputFiles.Prefix + jet, jet, "InDetTrackParticles",  "PrimaryVertices", sv))

    #BTagging
    for ts in timestamp:
        result.merge(JetBTaggingAlgCfg( \
            inputFlags
          , BTaggingCollection = inputFlags.BTagging.OutputFiles.Prefix + jet
          , JetCollection = jet
          , PrimaryVertexCollectionName="PrimaryVertices"
          , TaggerList = TaggerList
          , SecVertexers = SecVertexers
          , Tracks = "TracksForBTagging"
          , Muons = "MuonsForBTagging"
          , TimeStamp = ts
          , **kwargs
          )
        )

    if jet in postTagDL2JetToTrainingMap:
        #Track Augmenter
        result.merge(BTagTrackAugmenterAlgCfg(inputFlags))

        for ts in timestamp:
            #HighLevel taggers can not be run with time stamped containers
            if ts == "":
                result.merge(RunHighLevelTaggersCfg(inputFlags, jet, 'BTagTrackToJetAssociator', postTagDL2JetToTrainingMap[jet], ts))

    return result


def RunHighLevelTaggersCfg(inputFlags, JetCollection, Associator, TrainingMaps, TimeStamp):
    result = ComponentAccumulator()

    AthSequencer=CompFactory.AthSequencer

    BTagCollection = inputFlags.BTagging.OutputFiles.Prefix+JetCollection
    sequenceName = BTagCollection + "_HLTaggers"
    if TimeStamp:
            BTagCollection += '_' + TimeStamp
            sequenceName += '_' + TimeStamp

    HLBTagSeq = AthSequencer(sequenceName, Sequential = True)
    result.addSequence(HLBTagSeq)

    tracks = 'InDetTrackParticles'
    result.merge(BTagHighLevelAugmenterAlgCfg(inputFlags, JetCollection=JetCollection, BTagCollection=BTagCollection, Associator=Associator, TrackCollection=tracks), sequenceName=sequenceName )
    for dl2 in TrainingMaps:
        result.merge(HighLevelBTagAlgCfg(inputFlags, BTagCollection, TrackCollection=tracks, NNFile=dl2), sequenceName=sequenceName )

    return result

