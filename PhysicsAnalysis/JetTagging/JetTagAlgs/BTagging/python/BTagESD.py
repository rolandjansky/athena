# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from FlavorTagDiscriminants.BTagJetAugmenterAlgConfig import (
    BTagJetAugmenterAlgCfg)

from FlavorTagDiscriminants.FlavorTagNNConfig import FlavorTagNNCfg

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


def BTagHLTaggersCfg(inputFlags, JetCollection = []):
    acc = ComponentAccumulator()

    # Nothing written out
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(cfgFlags,"ESD", ItemList=[]))

    DeprecatedHLTaggers = ['rnnip', 'DL1']
    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
        #'BTagging/201903/smt/antikt4empflow/network.json',
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',
        #'BTagging/201903/dl1rmu/antikt4empflow/network.json',
        ]
    }
    for jet in JetCollection:
        if jet in postTagDL2JetToTrainingMap:
            for tagger in DeprecatedHLTaggers:
                acc.merge(RenameHLTaggerCfg(jet, tagger, '_old'))
            #Track Augmenter
            acc.merge(BTagTrackAugmenterAlgCfg(cfgFlags))

            #HighLevel taggers can not be run with time stamped containers
            acc.merge(RunHighLevelTaggersCfg(cfgFlags, jet, 'BTagTrackToJetAssociator', postTagDL2JetToTrainingMap[jet], ""))

    return acc


def RunHighLevelTaggersCfg(inputFlags, jet, tracks, Associator, taggers):
    result = ComponentAccumulator()

    BTagCollection = inputFlags.BTagging.OutputFiles.Prefix+JetCollection
    result.merge(BTagJetAugmenterAlgCfg(inputFlags, JetCollection=jet, BTagCollection=BTagCollection, Associator=Associator, TrackCollection=tracks) )
    for dl2 in taggers:
        result.merge(FlavorTagNNCfg(inputFlags, BTagCollection, TrackCollection=tracks, NNFile=dl2) )

    return result


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


def PrepareStandAloneBTagCfg(inputFlags):
    result=ComponentAccumulator()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    result.merge(PoolReadCfg(inputFlags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    # get standard config for magnetic field - map and cache
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    result.merge(AtlasFieldCacheCondAlgCfg( inputFlags ))

    #load folders needed for Run2 ID alignment
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    return result

def BTagRedoESDCfg(flags, jet, extraContainers=[]):
    acc=ComponentAccumulator()

    acc.merge(RenameInputContainerCfg("old"))

    #Register input ESD container in output
    ESDItemList = registerOutputBTaggingContainers(flags, jet)
    ESDItemList += registerJetCollectionEL(flags, jet)
    print(ESDItemList)
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList+extraContainers))

    return acc

def registerJetCollectionEL(flags, JetCollection):
    ItemList = []
    # btaggingLink
    ItemList.append('xAOD::JetContainer#'+JetCollection+'Jets')
    ItemList.append('xAOD::JetAuxContainer#'+JetCollection+'JetsAux.btaggingLink')
    return ItemList

def registerOutputBTaggingContainers(flags, JetCollection, suffix = ''):
      """Registers the jet collection to various containers in BTaggingFlags which govern which
      containers will be parsed to the output xAOD and ESD files. This used to happen in
      ./share/BTagging_jobOptions.py.

      input: JetCollection:       The name of the jet collection."""
      ItemList = []

      OutputFilesSVname = "SecVtx"
      OutputFilesJFVxname = "JFVtx"
      OutputFilesBaseName = "xAOD::BTaggingContainer#"
      OutputFilesBaseAuxName = "xAOD::BTaggingAuxContainer#"
      OutputFilesBaseNameSecVtx = "xAOD::VertexContainer#"
      OutputFilesBaseAuxNameSecVtx = "xAOD::VertexAuxContainer#"
      OutputFilesBaseNameJFSecVtx = "xAOD::BTagVertexContainer#"
      OutputFilesBaseAuxNameJFSecVtx= "xAOD::BTagVertexAuxContainer#"

      author = flags.BTagging.OutputFiles.Prefix + JetCollection
      # SecVert
      ItemList.append(OutputFilesBaseNameSecVtx + author + OutputFilesSVname)
      ItemList.append(OutputFilesBaseAuxNameSecVtx + author + OutputFilesSVname + 'Aux.-vxTrackAtVertex')
      # JFSeCVert
      ItemList.append(OutputFilesBaseNameJFSecVtx + author + OutputFilesJFVxname)
      ItemList.append(OutputFilesBaseAuxNameJFSecVtx + author + OutputFilesJFVxname + 'Aux.')

      if suffix:
          author += '_' + suffix

      ItemList.append(OutputFilesBaseName + author)
      ItemList.append(OutputFilesBaseAuxName + author + 'Aux.*')
      #ItemList.append(OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB')

      return ItemList

if __name__=="__main__":

    inputESD = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"
    import argparse
    parser = argparse.ArgumentParser(prog="BTagRun3Config: An example configuration module for btagging reconstruction reading an ESD",
                            usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
    parser.add_argument("-f", "--filesIn", default = inputESD, type=str, help="Comma-separated list of input files")
    parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-l", "--highlevel", type=str2bool, default=False, help="Run only high level taggers.")

    args = parser.parse_args()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.Files= args.filesIn.split(",")
    #cfgFlags.Input.isMC=False
    #cfgFlags.Input.Files=["/atlas/guirriec/git-athena/q431_2019-03-02T2147/myESD_2019.pool.root"]

    cfgFlags.Output.ESDFileName="esdOut.pool.root"

    # Flags relating to multithreaded execution
    cfgFlags.Concurrency.NumThreads = args.nThreads
    if args.nThreads>0:
        cfgFlags.Scheduler.ShowDataDeps = True
        cfgFlags.Scheduler.ShowDataFlow = True
        cfgFlags.Scheduler.ShowControlFlow = True
        cfgFlags.Concurrency.NumConcurrentEvents = args.nThreads
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    acc=MainServicesCfg(cfgFlags)
    acc.getService("MessageSvc").Format = "% F%80W%S%7W%R%T %0W%M"

    # Prevent the flags from being modified
    cfgFlags.lock()

    acc.merge(PrepareStandAloneBTagCfg(cfgFlags))

    JetCollection = ['AntiKt4EMTopo', 'AntiKt4EMPFlow']

    if args.highlevel:
        acc.merge(BTagHLTaggersCfg(cfgFlags, JetCollection = JetCollection))

    acc.setAppProperty("EvtMax",-1)

    acc.run()
    f=open("BTag.pkl","wb")
    acc.store(f)
    f.close()
