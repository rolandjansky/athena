# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from BTagging.BTaggingFlags import BTaggingFlags

def setupTrackAssociator(TrackCollection, options={}, Verbose = False, ContainerName = ""):

      options = dict(options)
      options['name'] = TrackCollection
      from BTagging.BTaggingConfiguration import toolBTagTrackToJetAssociator
      tool = toolBTagTrackToJetAssociator(**options)

      return tool

def setupMuonAssociator(MuonCollectionName, JetCollection, options={}, Verbose = False, ContainerName = ""):
      """Sets up a muon associator (and adds it to the ToolSvc).

      input: MuonCollectionName:  Muon collection name. Must be unique.
             JetCollection:       The jet collection name.
             options:             Python dictionary with options to be passed to the associator.
             Verbose:             Whether to print the associator settings afterwards.
             ContainerName:       Name of the muon container in SG. If left blank BTaggingFlags.MuonCollectionName will be used.

      output: The tool."""

      options = dict(options)
      options['name'] = MuonCollectionName
      from BTagging.BTaggingConfiguration import toolBTagMuonToJetAssociator
      tool = toolBTagMuonToJetAssociator(**options)

      return tool
 

def setupBTagTrackAssociation(name, JetCollection, TaggerList, Verbose = False, options={}):

    # Setup associators
    BTagTrackToJetAssociatorList = []
    BTagTrackToJetAssocNameList = []
    if 'IP2D' in TaggerList or 'IP3D' in TaggerList:
        tracktojetassociator = setupTrackAssociator('BTagTrackToJetAssociator', **options)
        BTagTrackToJetAssociatorList.append(tracktojetassociator)
        BTagTrackToJetAssocNameList.append('BTagTrackToJetAssociator')
        
    if 'MultiSVbb1' in TaggerList or 'MultiSVbb2' in TaggerList:
        tracktojetassociatorbb = setupTrackAssociator('BTagTrackToJetAssociatorBB',  
                                          options={'shareTracks': False,
                                             'useVariableSizedTrackCone' : True,
                                             'coneSizeFitPar1' : 3.15265e-01,
                                             'coneSizeFitPar2' : -3.66502e-01,
                                             'coneSizeFitPar3' : -1.56387e-05})
        BTagTrackToJetAssociatorList.append(tracktojetassociatorbb)
        BTagTrackToJetAssocNameList.append('BTagTrackToJetAssociatorBB')

    MuonToJetAssociatorList = []
    MuonToJetAssocNameList = []
    MuonContainerNameList = []
    if 'SoftMu' in TaggerList:
        tool = setupMuonAssociator('BTagMuonToJetAssociator', JetCollection, Verbose = BTaggingFlags.OutputLevel < 3)
        MuonToJetAssociatorList.append(tool)
        MuonToJetAssocNameList.append('BTagMuonToJetAssociator')
        MuonContainerNameList.append('Muons')

        
  # NOTE: The secondary vertex finders etc. don't need to be set up here depending on the flags; they are set up when needed by the
    options = dict(options)
    options.setdefault('TrackToJetAssociatorList', BTagTrackToJetAssociatorList)
    options.setdefault('TrackToJetAssocNameList', BTagTrackToJetAssocNameList)
    options.setdefault('MuonToJetAssociatorList', MuonToJetAssociatorList)
    options.setdefault('MuonToJetAssocNameList', MuonToJetAssocNameList)
    options.setdefault('MuonContainerNameList', MuonContainerNameList)
    options.setdefault('BTagAssociation', BTaggingFlags.doStandardAssoc)
    options['name'] = name
    from BTagging.BTaggingConf import Analysis__BTagTrackAssociation
    tool = Analysis__BTagTrackAssociation(**options)
    return tool

def setupSecVtxTool(name, JetCollection, Verbose = False, options={}):
      """Adds a SecVtxTool instance and registers it.

      input: name:               The tool's name.
             JetCollection       The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
             options:            Python dictionary of options to be passed to the SecVtxTool.
      output: The tool."""
      from BTagging.BTaggingConfiguration import getConfiguration
      ConfInstance = getConfiguration()

      jetcol = JetCollection
      secVtxFinderList = []
      secVtxFinderTrackNameList = []
      secVtxFinderxAODBaseNameList = []
      from BTagging.BTaggingConfiguration_NewJetFitterCollection import toolNewJetFitterVxFinder_SV

      newJetFitterVxFinder = toolNewJetFitterVxFinder_SV('JFVxFinder')
      secVtxFinderList.append(newJetFitterVxFinder)
      secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
      secVtxFinderxAODBaseNameList.append('JetFitter')
      from BTagging.BTaggingConfiguration_NewJetFitterCollection import toolJetFitterVariablesFactory
      jetFitterVF = toolJetFitterVariablesFactory('JFVarFactory')

      from BTagging.BTaggingConfiguration_InDetVKalVxInJetTool import toolInDetVKalVxInJetTool
      inDetVKalVxInJetTool = toolInDetVKalVxInJetTool("IDVKalVxInJet")
      secVtxFinderList.append(inDetVKalVxInJetTool)
      secVtxFinderTrackNameList.append('BTagTrackToJetAssociator')
      secVtxFinderxAODBaseNameList.append('SV1')

      from BTagging.BTaggingConfiguration_InDetVKalVxMultiVxInJetTool import toolInDetVKalMultiVxInJetTool
      inDetVKalMultiVxInJetTool = toolInDetVKalMultiVxInJetTool("IDVKalMultiVxInJet")
      secVtxFinderList.append(inDetVKalMultiVxInJetTool)
      secVtxFinderTrackNameList.append('BTagTrackToJetAssociatorBB')
      secVtxFinderxAODBaseNameList.append('MSV')

      from BTagging.BTaggingConfiguration_InDetVKalVxMultiVxInJetTool import toolMSVVariablesFactory
      varFactory = toolMSVVariablesFactory("MSVVarFactory")

      options = dict(options)
      options.setdefault('SecVtxFinderList', secVtxFinderList)
      options.setdefault('SecVtxFinderTrackNameList', secVtxFinderTrackNameList)
      options.setdefault('SecVtxFinderxAODBaseNameList', secVtxFinderxAODBaseNameList)
      options.setdefault('PrimaryVertexName',BTaggingFlags.PrimaryVertexCollectionName)
      options.setdefault('vxPrimaryCollectionName',BTaggingFlags.PrimaryVertexCollectionName)
      options.setdefault('BTagJFVtxCollectionName', ConfInstance.getOutputFilesPrefix() + jetcol + ConfInstance._OutputFilesJFVxname)
      options.setdefault('BTagSVCollectionName', ConfInstance.getOutputFilesPrefix() + jetcol + ConfInstance._OutputFilesSVname)
      options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
      options.setdefault('JetFitterVariableFactory', jetFitterVF)
      options.setdefault('MSVVariableFactory', varFactory)
      options['name'] = name
      from BTagging.BTaggingConf import Analysis__BTagSecVertexing
      tool = Analysis__BTagSecVertexing(**options)
      return tool

def setupBTagTool(jetcol, TaggerList, useBTagFlagsDefaults = True, Verbose = False, options={}):
      """Adds a new myBTagTool instance and registers it.

      input: jetcol:             The name of the jet collections.
             ToolSvc:            The ToolSvc instance.
             Verbose:            Whether to print detailed information about the tool.
             options:            Python dictionary of options to be passed to the BTagTool.
             (note the options storeSecondaryVerticesInJet is passed to the removal tool instead)

      The following default options exist:

      BTagLabelingTool                       default: None
      storeSecondaryVerticesInJet            default: BTaggingFlags.writeSecondaryVertices

      output: The btagtool for the desired jet collection."""

      from BTagging.BTaggingConfiguration import getConfiguration
      ConfInstance = getConfiguration()

      tagToolList = []

      if 'IP2D' in TaggerList:
          ip2dtool = IP2DTagCfg('IP2DTag')
          tagToolList.append(ip2dtool)

      if 'IP3D' in TaggerList:
          ip3dtool = IP3DTagCfg('IP3DTag')
          tagToolList.append(ip3dtool)

      if 'SV1' in TaggerList:
          sv1tool = SV1TagCfg('SV1Tag')
          tagToolList.append(sv1tool)
   
      if 'RNNIP' in TaggerList:
          rnniptool = RNNIPTagCfg('RNNIPTag')
          tagToolList.append(rnniptool)

      if 'JetFitterNN' in TaggerList:
          jetfitterNNtool = JetFitterTagNNCfg('JetFitterTagNN')
          tagToolList.append(jetfitterNNtool)
          
      if 'SoftMu' in TaggerList:
          softmutool = SoftMuonTagCfg('SoftMu')
          tagToolList.append(softmutool) 

      options = dict(options)
      if useBTagFlagsDefaults:
        from BTagging.BTaggingConfiguration_CommonTools import toolthisBTagLabeling as toolthisBTagLabeling
        options.setdefault('JetTruthMatchTool',None)
        btagLabeling = toolthisBTagLabeling('thisBTagLabeling',useBTagFlagsDefaults = False, **options)
        del options['JetTruthMatchTool']
        #crash with default JetTruthMatchTool
        #btagLabeling = toolthisBTagLabeling('thisBTagLabeling')
        defaults = { 'Runmodus'                     : BTaggingFlags.Runmodus,
                     'PrimaryVertexName'            : BTaggingFlags.PrimaryVertexCollectionName,
                     'BaselineTagger'               : BTaggingFlags.BaselineTagger,
                     'BTagLabelingTool'             : btagLabeling,
                     'vxPrimaryCollectionName'      : BTaggingFlags.PrimaryVertexCollectionName,
                     'TagToolList'                  : tagToolList,
                     'OutputLevel'                  : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
      options['name'] = 'btag'
      from BTagging.BTaggingConf import Analysis__BTagTool
      btagtool = Analysis__BTagTool(**options)
      
      return btagtool

def toolBTagTrackToJetAssociator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a BTagTrackToJetAssociator tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from ParticleJetTools.ParticleJetToolsConf import Analysis__ParticleToJetAssociator
    return Analysis__ParticleToJetAssociator(**options)


def setupJetBTaggerAlg(JetCollection="", TaggerList=[], SetupScheme="", Verbose = True, options={}, StripJetsSuffix = True):

    jetcol = JetCollection

    from BTagging.BTaggingConfiguration import getConfiguration
    ConfInstance = getConfiguration()
    
    btagtool = setupBTagTool(jetcol, TaggerList,  Verbose = Verbose, options=options)
    options.setdefault('BTagTool', btagtool)

    from BTagging.BTaggingConf import Analysis__JetBTaggerAlg as JetBTaggerAlg
    options = dict(options)
    options.setdefault('OutputLevel', BTaggingFlags.OutputLevel)
    
    # setup the Analysis__BTagTrackAssociation tool
    thisBTagTrackAssociation = setupBTagTrackAssociation('TrackAssociation', jetcol, TaggerList, Verbose = Verbose)
    options.setdefault('BTagTrackAssocTool', thisBTagTrackAssociation)
    
    # setup the secondary vertexing tool
    thisSecVtxTool = setupSecVtxTool('SecVx'+ConfInstance.GeneralToolSuffix(), jetcol, Verbose)
    options.setdefault('BTagSecVertexing', thisSecVtxTool)
    
    # Set remaining options
    btagname = ConfInstance.getOutputFilesPrefix() + jetcol
    options.setdefault('name', 'FTAG')
    options.setdefault('JetCollectionName', jetcol.replace('Track','PV0Track') + "Jets")
    options.setdefault('BTaggingCollectionName', btagname)

    # -- create main BTagging algorithm
    jetbtaggeralg = JetBTaggerAlg(**options)
    return jetbtaggeralg


def IP2DTagCfg(name, suffix = "", options = {}):
    options['xAODBaseName'] = 'IP2D'
    options['trackAssociationName'] = 'BTagTrackToJetAssociator'

    #tool_type was used to define the name of the tool and the metadata to be used
    if (suffix == ""):
        from BTagging.BTaggingConfiguration_IP2DTag import toolIP2DTag
        return toolIP2DTag(name, **options)


def IP3DTagCfg(name, suffix = "", options = {}):
    options['xAODBaseName'] = 'IP3D'
    options['trackAssociationName'] = 'BTagTrackToJetAssociator'

    #tool_type was used to define the name of the tool and the metadata to be used
    if (suffix == ""):
        from BTagging.BTaggingConfiguration_IP3DTag import toolIP3DTag
        return toolIP3DTag(name, **options)

def JetFitterTagNNCfg(name, suffix = "", options = {}):
    options['xAODBaseName'] = 'JetFitter'

    #tool_type was used to define the name of the tool and the metadata to be used
    if (suffix == ""):
        from BTagging.BTaggingConfiguration_NewJetFitterCollection import toolJetFitterTagNN
        return toolJetFitterTagNN(name, **options)


def SV1TagCfg(name, suffix = "", options = {}):
    options['xAODBaseName'] = 'SV1'

    #tool_type was used to define the name of the tool and the metadata to be used
    if (suffix == ""):
        from BTagging.BTaggingConfiguration_SV1Tag import toolSV1Tag
        return toolSV1Tag(name, **options)


def RNNIPTagCfg(name, suffix = "", options = {}):
    options['xAODBaseName'] = 'RNNIP'

    #tool_type was used to define the name of the tool and the metadata to be used
    if (suffix == ""):
        from BTagging.BTaggingConfiguration_RNNIPTag import toolRNNIPTag
        return toolRNNIPTag(name, **options)

def SoftMuonTagCfg(name, suffix = "", options = {}):
    
    #tool_type was used to define the name of the tool and the metadata to be used
    if (suffix == ""):
        from BTagging.BTaggingConfiguration_SoftMuonTag import toolSoftMuonTag
        return toolSoftMuonTag(name, **options)

def JetTagCalibCfg(ConfigFlags, scheme=""):
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
      taggerList = ['IP2D','IP3D','MultiSVbb1','MultiSVbb2','SV1','SoftMu','JetFitterNN','MV2c10','MV2c10mu','MV2c10rnn','MV2c100','MV2cl100','RNNIP','DL1','DL1mu','DL1rnn','JetVertexCharge']
      histoskey = "JetTagCalibHistosKey"
      result.merge(addFolders(ConfigFlags,[readkeycalibpath], connSchema, className='CondAttrListCollection'))
      #conddb.addFolder(connSchema, readkeycalibpath, className='CondAttrListCollection')
      JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = taggerList, channelAliases = BTaggingFlags.CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = BTaggingFlags.RNNIPConfig)

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

    from StoreGate.StoreGateConf import StoreGateSvc
    result.addService(StoreGateSvc("DetectorStore"))
    
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    result.mergeAll(GeoModelCfg(inputFlags))

    from TrkDetDescrSvc.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc, geom_svc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_AlignCondAlg
    result.addCondAlgo(SCT_AlignCondAlg(name = "SCT_AlignCondAlg",UseDynamicAlignFolders = False))

    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DetectorElementCondAlg
    result.addCondAlgo(SCT_DetectorElementCondAlg(name = "SCT_DetectorElementCondAlg"))
    
    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    

    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
    from AthenaCommon import CfgGetter
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("PixelDetectorTool", checkType=True) ]
    result.getService("GeoModelSvc").DetectorTools += [ CfgGetter.getPrivateTool("SCT_DetectorTool", checkType=True) ]

    from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
    trtDetectorTool = TRT_DetectorTool()
    result.getService("GeoModelSvc").DetectorTools += [ trtDetectorTool ]

    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    result.getService("GeoModelSvc").DetectorTools += [ InDetServMatTool() ]

    from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline,IOVDbSvcCfg
    result.merge(addFolders(inputFlags,['/GLOBAL/BField/Maps <noover/>'],'GLOBAL_OFL'))
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

    #hard-code MC conditions tag needed for my ESD file - must be a better way? how to auto-configure?
    iovDbSvc.GlobalTag="OFLCOND-MC16-SDR-20"

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
    btagger = setupJetBTaggerAlg(JetCollection = jet, TaggerList = taggerList)

    result.addEventAlgo(btagger)

    return result

if __name__=="__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

    cfgFlags.Output.ESDFileName="esdOut.pool.root"
    cfgFlags.lock()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg 
    cfg=MainServicesSerialCfg() 


    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(cfgFlags))

    cfg.merge(JetTagCalibCfg(cfgFlags))

    Acc=BTagCfg(cfgFlags) 
   
    cfg.merge(Acc) 

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    print(BTaggingFlags.btaggingESDList)    
    cfg.merge(OutputStreamCfg(cfgFlags,"ESD", ItemList=BTaggingFlags.btaggingESDList))
    cfg.getService("StoreGateSvc").Dump=True


    cfg.setAppProperty("EvtMax",10)
   

    cfg.run()
    #f=open("BTag.pkl","w")
    #cfg.store(f)
    #f.close()
