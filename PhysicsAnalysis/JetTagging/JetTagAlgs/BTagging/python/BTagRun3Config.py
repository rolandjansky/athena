# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.JetBTaggerAlgConfig import JetBTaggerAlgCfg
from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg

def JetTagCalibCfg(ConfigFlags, scheme="", TaggerList = []):
    result=ComponentAccumulator()

    #if ConfInstance.checkFlagsUsingBTaggingFlags():
    if True:
      #IP2D
      grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
                  "0HitInExp", "0HitIn",
                  "0HitNInExp", "0HitNIn",
                  "InANDNInShared", "PixShared", "SctShared",
                  "InANDNInSplit", "PixSplit",
                  "Good"]

      #IP3D
      #Same as IP2D. Revisit JetTagCalibCondAlg.cxx if not.

      JetTagCalibCondAlg=CompFactory.Analysis.JetTagCalibCondAlg
      jettagcalibcondalg = "JetTagCalibCondAlg"
      readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
      connSchema = "GLOBAL_OFL"
      if not ConfigFlags.Input.isMC:
          readkeycalibpath = readkeycalibpath.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
          connSchema = "GLOBAL"
      histoskey = "JetTagCalibHistosKey"
      result.merge(addFolders(ConfigFlags,[readkeycalibpath], connSchema, className='CondAttrListCollection'))
      JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = TaggerList, channelAliases = BTaggingFlags.CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = BTaggingFlags.RNNIPConfig)
  # Maybe needed for trigger use
  #from IOVDbSvc.CondDB import conddb
  #if conddb.dbdata == 'COMP200':
  #  conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/BTagCalib/RUN12", className='CondAttrListCollection')
  #  if globalflags.DataSource()!='data':
  #    conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/TrigBTagCalib/RUN12", className='CondAttrListCollection')
  #elif conddb.isMC:
  #  conddb.addFolder("GLOBAL_OFL", "/GLOBAL/BTagCalib/RUN12", className='CondAttrListCollection')
  #  conddb.addFolder("GLOBAL_OFL", "/GLOBAL/TrigBTagCalib/RUN12", className='CondAttrListCollection')

    result.addCondAlgo(JetTagCalib)

    return result

def registerJetCollectionEL(flags, JetCollection, TimeStamp):
    ItemList = []
    # btaggingLink
    suffix = ".".join(['btaggingLink'+ ts for ts in TimeStamp])
    ItemList.append('xAOD::JetContainer#'+JetCollection+'Jets.' + suffix)
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
      # jetLink
      jetLink = '.jetLink'
      ItemList.append(OutputFilesBaseName+author+jetLink)
      ItemList.append(OutputFilesBaseAuxName+author+jetLink)

      ItemList.append(OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB')

      return ItemList

def BTagRedoESDCfg(flags, jet, extraContainers=[]):
    acc=ComponentAccumulator()

    acc.merge(RenameInputContainerCfg("old"))

    #Register input ESD container in output
    ESDItemList = registerOutputBTaggingContainers(flags, jet)
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList+extraContainers))

    return acc

def BTagESDtoESDCfg(flags, jet, timestamp):
    acc=ComponentAccumulator()

    ESDItemList = []
    #Register new ouput ESD container
    for ts in timestamp:
        ESDItemList += registerOutputBTaggingContainers(flags, jet, ts)

    ESDItemList += registerJetCollectionEL(flags, jet, timestamp)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList))

    return acc

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

def BTagCfg(inputFlags,**kwargs):

    #This is monolithic for now. 
    #Once a first complete example runs, this will be split into small modular chunks.
    #Some such items may be best placed elsewehere (e.g. put magnetic field setup in magnetic field git folder etc)
    result=ComponentAccumulator()
    timestamp = kwargs.get('TimeStamp', None)
    if timestamp: del kwargs['TimeStamp']
    splitAlg = kwargs.get('SplitAlg', None)
    del kwargs['SplitAlg']
    release = kwargs.get('Release', None)
    if release: del kwargs['Release']

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    

    GeometryDBSvc=CompFactory.GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    result.merge(PixelGeometryCfg( inputFlags ))

    from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
    result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_OFL'))
    #result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_ONL'))
    #result.merge(addFolders(inputFlags,['/GLOBAL/TrackingGeo/LayerMaterialV2'],'GLOBAL_ONL'))
    result.merge(addFolders(inputFlags,['/EXT/DCS/MAGNETS/SENSORDATA'],'DCS_OFL'))
    
    MagField__AtlasFieldSvc=CompFactory.MagField.AtlasFieldSvc
    kwargs.setdefault( "UseDCS", True )
    result.addService(MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs))
    del kwargs['UseDCS']

    #load folders needed for Run2 ID alignment
    result.merge(addFoldersSplitOnline(inputFlags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    #load folders needed for IBL
    result.merge(addFolders(inputFlags,['/Indet/IBLDist'],'INDET_OFL'))

    #Should be parameters
    JetCollection = ['AntiKt4EMTopo','AntiKt4EMPFlow']

    TrainedTaggers = inputFlags.BTagging.run2TaggersList + ['MultiSVbb1','MultiSVbb2']
    result.merge(JetTagCalibCfg(inputFlags, TaggerList = TrainedTaggers))

    for jet in JetCollection:
        taggerList = inputFlags.BTagging.run2TaggersList
        taggerList += ['MultiSVbb1','MultiSVbb2']
        if timestamp:
            #Time-stamped BTagging container (21.2)
            result.merge(BTagESDtoESDCfg(inputFlags, jet, timestamp))
            kwargs['TimeStamp'] = timestamp
            result.merge(RenameInputContainerCfg("old"))
        else:
            extraCont = []
            if splitAlg:
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

        if splitAlg:
            timestamp = kwargs.get('TimeStamp', None)
            if not timestamp:
                timestamp = ['']
            else:
                del kwargs['TimeStamp']

            postTagDL2JetToTrainingMap={
                'AntiKt4EMPFlow': [
                #'BTagging/201903/smt/antikt4empflow/network.json',
                'BTagging/201903/rnnip/antikt4empflow/network.json',
                'BTagging/201903/dl1r/antikt4empflow/network.json',
                'BTagging/201903/dl1/antikt4empflow/network.json',
                #'BTagging/201903/dl1rmu/antikt4empflow/network.json',
                ]
            }

            if jet in postTagDL2JetToTrainingMap:
                #Remove DL1 and RNNIP from taggers list, those taggers are run with HighLevelBTagAlg
                taggerList.remove('RNNIP')
                taggerList.remove('DL1')

            #Track Association
            TrackToJetAssociators = ['BTagTrackToJetAssociator', 'BTagTrackToJetAssociatorBB']

            kwargs['Release'] = '22'
            result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", 'BTagTrackToJetAssociator', **kwargs))
            kwargs['Release'] = '21'
            #result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", 'BTagTrackToJetAssociator', **kwargs))
            result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", 'BTagTrackToJetAssociatorBB', **kwargs))
            del kwargs['Release']

            #Sec vertex finding
            SecVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator'}
            for k, v in SecVertexingAndAssociators.items():
                if v not in TrackToJetAssociators:
                    raise RuntimeError( v + ' is not configured')
                result.merge(JetSecVtxFindingAlgCfg(inputFlags, jet, "InDetTrackParticles", k, v))

                #Sec vertexing
                result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", k, v))

            #result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", 'MSV', 'BTagTrackToJetAssociatorBB'))

            #BTagging
            for ts in timestamp:
                result.merge(JetBTaggingAlgCfg(inputFlags, JetCollection = jet, TaggerList = taggerList, SVandAssoc = SecVertexingAndAssociators, TimeStamp = ts, **kwargs))

            if jet in postTagDL2JetToTrainingMap:
                #Track Augmenter
                result.merge(BTagTrackAugmenterAlgCfg(inputFlags))

                for ts in timestamp:
                    #HighLevel taggers can not be run with time stamped containers
                    if ts == "":
                        result.merge(RunHighLevelTaggersCfg(inputFlags, jet, 'BTagTrackToJetAssociator', postTagDL2JetToTrainingMap[jet], ts))

        else:
            result.merge(JetBTaggerAlgCfg(inputFlags, JetCollection = jet, TaggerList = taggerList, **kwargs))

    return result

def RunHighLevelTaggersCfg(inputFlags, JetCollection, Associator, TrainingMaps, TimeStamp):
    result = ComponentAccumulator()

    from AthenaCommon.AlgSequence import AthSequencer

    BTagCollection = 'BTagging_'+JetCollection
    sequenceName = BTagCollection + "_HLTaggers"
    if TimeStamp:
            BTagCollection += '_' + TimeStamp
            sequenceName += '_' + TimeStamp

    HLBTagSeq = AthSequencer(sequenceName)
    HLBTagSeq.Sequential = True
    result.addSequence(HLBTagSeq)

    result.merge(BTagHighLevelAugmenterAlgCfg(inputFlags, sequenceName, BTagCollection = BTagCollection, Associator = Associator, **kwargs) )
    for dl2 in TrainingMaps:
        result.merge(HighLevelBTagAlgCfg(inputFlags, sequenceName, BTagCollection, dl2) )

    return result

def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')

if __name__=="__main__":

    inputESD = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"
    import argparse
    parser = argparse.ArgumentParser(prog="BTagRun3Config: An example configuration module for btagging reconstruction reading an ESD",
                            usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
    parser.add_argument("-f", "--filesIn", default = inputESD, type=str, help="Comma-separated list of input files")
    parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-r", "--release", default="22", type=str, help="Release number to test different scenarii.")
    parser.add_argument('-s', '--splitAlg', type=str2bool, default=False, help="Split JetBTaggerAlg.")

    args = parser.parse_args()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.isMC=True
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
        from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg 
        acc=MainServicesThreadedCfg(cfgFlags)
        acc.getService("MessageSvc").Format = "% F%80W%S%7W%R%T %0W%M"
    else:
        from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
        acc=MainServicesSerialCfg()
        acc.getService("MessageSvc").Format = "% F%80W%S%7W%R%T %0W%M"

    # Prevent the flags from being modified
    cfgFlags.lock()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(cfgFlags))

    kwargs = {}
    if args.release == "21.2":
        kwargs["TimeStamp"] = ['201810','201903']
    kwargs["SplitAlg"] = args.splitAlg

    acc.merge(BTagCfg(cfgFlags, **kwargs))

    acc.setAppProperty("EvtMax",-1)

    acc.run()
    f=open("BTag.pkl","wb")
    acc.store(f)
    f.close()
