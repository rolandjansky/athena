# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from BTagging.BTagRun3Config import RenameHLTaggerCfg
from BTagging.BTagRun3Config import RunHighLevelTaggersCfg
from BTagging.BTagRun3Config import RenameInputContainerCfg
from BTagging.BTagRun3Config import JetBTaggerSplitAlgsCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from JetTagCalibration.JetTagCalibConfig import JetTagCalibCfg


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
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg( inputFlags ))

    #load folders needed for Run2 ID alignment
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    return result

def BTagRedoESDCfg(flags, jet, extraContainers=[]):
    acc=ComponentAccumulator()

    acc.merge(RenameInputContainerCfg("old"))

    #Register input ESD container in output
    ESDItemList = registerOutputBTaggingContainers(flags, jet)
    ESDItemList += registerJetCollectionEL(flags, jet, [''])
    print(ESDItemList)
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList+extraContainers))

    return acc


def registerJetCollectionEL(flags, JetCollection, TimeStamp):
    ItemList = []
    # btaggingLink
    suffix = ".".join(['btaggingLink'+ ts for ts in TimeStamp])
    ItemList.append('xAOD::JetContainer#'+JetCollection+'Jets')
    ItemList.append('xAOD::JetAuxContainer#'+JetCollection+'JetsAux.'+ suffix)

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

def BTagCfg(inputFlags, JetCollection = [], **kwargs):

    #This is monolithic for now. 
    #Once a first complete example runs, this will be split into small modular chunks.
    result=ComponentAccumulator()

    TrainedTaggers = inputFlags.BTagging.run2TaggersList + ['MultiSVbb1','MultiSVbb2']
    result.merge(JetTagCalibCfg(inputFlags, TaggerList = TrainedTaggers, **kwargs))

    for jet in JetCollection:
        taggerList = inputFlags.BTagging.run2TaggersList
        taggerList += ['MultiSVbb1','MultiSVbb2']

        extraCont = []
        extraCont.append("xAOD::VertexContainer#PrimaryVertices")
        extraCont.append("xAOD::VertexAuxContainer#PrimaryVerticesAux.")
        extraCont.append("xAOD::TrackParticleContainer#InDetTrackParticles")
        extraCont.append("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.")

        JFSecVtx = "xAOD::BTagVertexContainer#"
        AuxJFSecVtx= "xAOD::BTagVertexAuxContainer#"
        SecVtx = "xAOD::VertexContainer#"
        AuxSecVtx = "xAOD::VertexAuxContainer#"
        author = inputFlags.BTagging.OutputFiles.Prefix + jet
        extraCont.append(JFSecVtx + author + 'JFVtxMT')
        extraCont.append(AuxJFSecVtx + author + 'JFVtxMTAux.')
        extraCont.append(SecVtx + author + 'SecVtxMT')
        extraCont.append(AuxSecVtx + author + 'SecVtxMTAux.-vxTrackAtVertex')
        extraCont.append(SecVtx + author + 'MSVMT')
        extraCont.append(AuxSecVtx + author + 'MSVMTAux.-vxTrackAtVertex')
        extraCont.append("xAOD::BTaggingContainer#" + author + 'MT')
        extraCont.append("xAOD::BTaggingAuxContainer#" + author + 'MTAux.')
        result.merge(BTagRedoESDCfg(inputFlags, jet, extraCont))

        secVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}

        result.merge(JetBTaggerSplitAlgsCfg(inputFlags, JetCollection = jet, TaggerList = taggerList, SecVertexingAndAssociators = secVertexingAndAssociators, **kwargs))

    return result

if __name__=="__main__":

    inputESD = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"
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
