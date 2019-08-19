# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.JetBTaggerAlgConfig import JetBTaggerAlgCfg

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

def BTagCfg(inputFlags,**kwargs):

    #This is monolithic for now. 
    #Once a first complete example runs, this will be split into small modular chunks.
    #Some such items may be best placed elsewehere (e.g. put magnetic field setup in magnetic field git folder etc)
    result=ComponentAccumulator()

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
    
    iovDbSvc=result.getService("IOVDbSvc")
    iovDbSvc.FoldersToMetaData+=['/GLOBAL/BField/Maps']

    from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
    kwargs.setdefault( "UseDCS", True )
    result.addService(MagField__AtlasFieldSvc("AtlasFieldSvc",**kwargs))

    #load folders needed for Run2 ID alignment
    result.merge(addFoldersSplitOnline(inputFlags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    #load folders needed for IBL
    result.merge(addFolders(inputFlags,['/Indet/IBLDist'],'INDET_OFL'))

    #BTagging part 
    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()

    #Should be parameters
    jet = 'AntiKt4EMTopo'
    #jet = 'AntiKt4LCTopo'
    #Register input ESD container
    ConfInstance.RegisterOutputContainersForJetCollection(JetCollection = jet, Verbose=True)
    #Create BTagging containers with prefix "New" and register containers
    ConfInstance.setOutputFilesPrefix('New'+ConfInstance.getOutputFilesPrefix())
    ConfInstance.RegisterOutputContainersForJetCollection(JetCollection = jet, Verbose=True)

    #Rename the element link of the BTagging container from the Jet container
    from SGComps.SGCompsConf import AddressRemappingSvc
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.btaggingLink->AntiKt4EMTopoJets.oldbtaggingLink']
    result.addService(AddressRemappingSvc)    
    result.getService('ProxyProviderSvc').ProviderNames += [ "AddressRemappingSvc" ]
   
    taggerList = ['IP2D','IP3D','MultiSVbb1','MultiSVbb2','SV1','SoftMu','JetFitterNN','MV2c10','MV2c10mu','MV2c10rnn','MV2c100','MV2cl100','RNNIP','DL1','DL1mu','DL1rnn','JetVertexCharge']
    result.merge(JetBTaggerAlgCfg(inputFlags, JetCollection = jet, TaggerList = taggerList))

    result.merge(JetTagCalibCfg(inputFlags, TaggerList = taggerList))

    return result

if __name__=="__main__":

    inputESD = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"
    from argparse import ArgumentParser
    parser = ArgumentParser(prog="BTagRun3Config: An example configuration module for btagging reconstruction reading an ESD",
                            usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
    parser.add_argument("-f", "--filesIn", default = inputESD, type=str, help="Comma-separated list of input files")
    parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")

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
        cfg=MainServicesThreadedCfg(cfgFlags) 
    else:
        from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
        cfg=MainServicesSerialCfg() 

    # Prevent the flags from being modified
    cfgFlags.lock()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))

    cfg.merge(BTagCfg(cfgFlags)) 

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(cfgFlags,"ESD", ItemList=BTaggingFlags.btaggingESDList))
    cfg.getEventAlgo("OutputStreamESD").ForceRead = True


    #cfg.getService("StoreGateSvc").Dump=True

    cfg.setAppProperty("EvtMax",10)

    cfg.run()
    f=open("BTag.pkl","w")
    cfg.store(f)
    f.close()
