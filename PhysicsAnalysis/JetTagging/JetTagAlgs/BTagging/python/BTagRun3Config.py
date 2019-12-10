# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.JetBTaggerAlgConfig import JetBTaggerAlgCfg
from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg

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

      from JetTagCalibration.JetTagCalibrationConf import Analysis__JetTagCalibCondAlg as JetTagCalibCondAlg
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


def registerOutputContainersForJetCollection(flags, JetCollection, suffix = ''):
      """Registers the jet collection to various containers in BTaggingFlags which govern which
      containers will be parsed to the output xAOD and ESD files. This used to happen in
      ./share/BTagging_jobOptions.py.

      input: JetCollection:       The name of the jet collection."""
      ItemList = []
      # btaggingLink
      ItemList.append('xAOD::JetContainer#'+JetCollection+'Jets.btaggingLink' + suffix)
      ItemList.append('xAOD::JetAuxContainer#'+JetCollection+'JetsAux.btaggingLink'+ suffix)

      OutputFilesSVname = "SecVtx"
      OutputFilesJFVxname = "JFVtx"
      OutputFilesBaseName = "xAOD::BTaggingContainer#"
      OutputFilesBaseAuxName = "xAOD::BTaggingAuxContainer#"
      OutputFilesBaseNameSecVtx = "xAOD::VertexContainer#"
      OutputFilesBaseAuxNameSecVtx = "xAOD::VertexAuxContainer#"
      OutputFilesBaseNameJFSecVtx = "xAOD::BTagVertexContainer#"
      OutputFilesBaseAuxNameJFSecVtx= "xAOD::BTagVertexAuxContainer#"

      if suffix:
          suffix = '_' + suffix

      author = flags.BTagging.OutputFiles.Prefix + JetCollection + suffix
      ItemList.append(OutputFilesBaseName + author)
      ItemList.append(OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB')
      # SecVert
      ItemList.append(OutputFilesBaseNameSecVtx + author + OutputFilesSVname)
      ItemList.append(OutputFilesBaseAuxNameSecVtx + author + OutputFilesSVname + 'Aux.-vxTrackAtVertex')
      # JFSeCVert
      ItemList.append(OutputFilesBaseNameJFSecVtx + author + OutputFilesJFVxname)
      ItemList.append(OutputFilesBaseAuxNameJFSecVtx + author + OutputFilesJFVxname + 'Aux.')

      return ItemList

def BTagRedoESDCfg(flags, jet, extraContainers=[]):
    acc=ComponentAccumulator()

    #Delete BTagging container read from input ESD
    from SGComps.SGCompsConf import AddressRemappingSvc, ProxyProviderSvc
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.btaggingLink->AntiKt4EMTopoJets.btaggingLink_old']
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo->BTagging_AntiKt4EMTopo_old']
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))

    #Register input ESD container in output
    ESDItemList = registerOutputContainersForJetCollection(flags, jet)
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList+extraContainers))

    return acc

def BTagESDtoESDCfg(flags, jet, new):
    acc=ComponentAccumulator()

    #Register input ESD container in output
    ESDItemList = registerOutputContainersForJetCollection(flags, jet)

    #Register new ouput ESD container
    ESDnewItemList = registerOutputContainersForJetCollection(flags, jet, new)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList+ESDnewItemList))

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

    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    

    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
    from AthenaCommon import CfgGetter
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("PixelDetectorTool", checkType=True) ]

    from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
    result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_OFL'))
    #result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_ONL'))
    #result.merge(addFolders(inputFlags,['/GLOBAL/TrackingGeo/LayerMaterialV2'],'GLOBAL_ONL'))
    result.merge(addFolders(inputFlags,['/EXT/DCS/MAGNETS/SENSORDATA'],'DCS_OFL'))
    
    from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
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
    taggerList = inputFlags.BTagging.run2TaggersList
    taggerList += ['MultiSVbb1','MultiSVbb2']


    for jet in JetCollection:
        if timestamp:
            #Time-stamped BTagging container (21.2)
            for ts in timestamp:
              result.merge(BTagESDtoESDCfg(inputFlags, jet, ts))
            kwargs['TimeStamp'] = timestamp
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
            result.merge(BTagRedoESDCfg(inputFlags, jet, extraCont))

        if splitAlg:
            #Track Association
            kwargs['Release'] = '22'
            result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", jet+'Jets.BTagTrackToJetAssociator', **kwargs))
            kwargs['Release'] = '21'
            #result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", jet+'Jets.BTagTrackToJetAssociator', **kwargs))
            result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", jet+'Jets.BTagTrackToJetAssociatorBB', **kwargs))
            del kwargs['Release']

            #Sec vertexing
            result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", 'JetFitter', jet+'Jets.BTagTrackToJetAssociator'))
            result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", 'SV1', jet+'Jets.BTagTrackToJetAssociator'))
            result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", 'MSV', jet+'Jets.BTagTrackToJetAssociatorBB'))

        result.merge(JetBTaggerAlgCfg(inputFlags, JetCollection = jet, TaggerList = taggerList, **kwargs))

    result.merge(JetTagCalibCfg(inputFlags, TaggerList = taggerList))

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
    else:
        from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
        acc=MainServicesSerialCfg()

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
