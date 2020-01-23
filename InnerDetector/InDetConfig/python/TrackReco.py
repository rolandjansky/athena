from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from CfgUtils import setDefaults
from AthenaConfiguration.UnifyProperties import unifyProperty,unifySet,_propsToUnify
from AthenaCommon.AppMgr import ToolSvc
_propsToUnify['THistSvc.Output']=unifySet
import InDetKeys


def InDetBCM_ZeroSuppressionCfg(flags, **kwargs):
	from BCM_ZeroSuppression.BCM_ZeroSuppressionConf import BCM_ZeroSuppression
	result = ComponentAccumulator()
	kwargs = setDefaults(kwargs,
                        BcmContainerName = flags.InDetKeys.BCM_RDOs)
	algo = BCM_ZeroSuppression("InDetBCM_ZeroSuppression", **kwargs)
	result.addEventAlgo(algo, primary = True)
	return result

def InDetPixelGangedAmbiguitiesFinderCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
    InDetPixelGangedAmbiguitiesFinder = InDet__PixelGangedAmbiguitiesFinder( name = "InDetPixelGangedAmbiguitiesFinder")
    acc.addPublicTool( InDetPixelGangedAmbiguitiesFinder )
    return acc,InDetPixelGangedAmbiguitiesFinder


def InDetPixelClusterizationCfg(flags, **kwargs) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__PixelClusterization
    sub_acc,merged_pixels_tool = InDetMergedPixelsToolCfg(flags)
    acc.merge(sub_acc)
    sub_acc,ambi_finder = InDetPixelGangedAmbiguitiesFinderCfg(flags)
    acc.merge(sub_acc)
    acc.addEventAlgo( InDet__PixelClusterization("InDetPixelClusterization",
                                                          clusteringTool          = merged_pixels_tool,
                                                          gangedAmbiguitiesFinder = ambi_finder,
                                                          ###DetectorManagerName     = flags.InDetKeys.PixelManager,
                                                          DataObjectName          = flags.InDetKeys.PixelRDOs,
                                                          ClustersName            = flags.InDetKeys.PixelClusters ))
    return acc


def InDetPixelClusterizationPUCfg(flags, **kwargs) :
    return InDetPixelClusterizationCfg(flags, **setDefaults( kwargs,
        name                    = "InDetPixelClusterizationPU",
        DataObjectName          = flags.InDetKeys.PixelPURDOs,
        ClustersName            = flags.InDetKeys.PixelPUClusters,
        AmbiguitiesMap          = "PixelClusterAmbiguitiesMapPU"))



def InDetMergedPixelsToolCfg(flags) :
      from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
      acc = ComponentAccumulator()
      #
      # --- MergedPixelTool (public)
      #
      minSplitProbability  = 0
      minSplitSize         = 3
      clusterSplitProbTool = None
      clusterSplitterTool  = None
      #
      # --- do we use new splittig or not ?
      #
      if flags.InDetFlags.doPixelClusterSplitting :
         #
         # --- Neutral Network version ?
         #
         if flags.InDetFlags.pixelClusterSplittingType == 'NeuralNet':
            useBeamConstraint = flags.InDetFlags.useBeamConstraint
            
            # --- new NN prob tool
	    ####sub_acc, tool = NnClusterizationFactoryCfg(flags)
            from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory
            NnClusterizationFactory = InDet__NnClusterizationFactory(name                         = "NnClusterizationFactory",
                                                             PixelLorentzAngleTool        = "", ##ToolSvc.PixelLorentzAngleTool,
                                                             useToT                       = flags.InDetFlags.doNNToTCalibration,
                                                             NnCollectionReadKey          = "PixelClusterNN",
                                                             NnCollectionWithTrackReadKey = "PixelClusterNNWithTrack")

            MultiplicityContent = [1 , 1 , 1]
            if flags.InDetFlags.doSLHC:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitProbTool as PixelClusterSplitProbTool
            else:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitProbTool as PixelClusterSplitProbTool
            NnPixelClusterSplitProbTool=PixelClusterSplitProbTool(name                     = "NnPixelClusterSplitProbTool",
                                                                           PriorMultiplicityContent = MultiplicityContent,
                                                                           NnClusterizationFactory  = NnClusterizationFactory,
                                                                           useBeamSpotInfo          = useBeamConstraint)
            #ToolSvc += NnPixelClusterSplitProbTool
            #if (InDetFlags.doPrintConfigurables()):
            #  print NnPixelClusterSplitProbTool

            # --- remember this prob tool  
            clusterSplitProbTool = NnPixelClusterSplitProbTool
            
            # --- new NN splitter
            if flags.InDetFlags.doSLHC :
                from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitter as PixelClusterSplitter
            else:
                from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitter as PixelClusterSplitter
            NnPixelClusterSplitter=PixelClusterSplitter(name                                = "NnPixelClusterSplitter",
                                                                 NnClusterizationFactory             = NnClusterizationFactory,
                                                                 ThresholdSplittingIntoTwoClusters   = 0.5, # temp.
                                                                 ThresholdSplittingIntoThreeClusters = 0.25, # temp.
                                                                 SplitOnlyOnBLayer                   = False,
                                                                 useBeamSpotInfo                     = useBeamConstraint)

            
            #ToolSvc += NnPixelClusterSplitter
            #if (InDetFlags.doPrintConfigurables()):
            #  print NnPixelClusterSplitter

            # remember splitter tool  
            clusterSplitterTool = NnPixelClusterSplitter
            

         #
         # --- Neutral Network version ?
         #
         elif flags.InDetFlags.pixelClusterSplittingType() == 'AnalogClus':      
            # new splitter tool
            from SiClusterizationTool.SiClusterizationToolConf import InDet__TotPixelClusterSplitter
            TotPixelClusterSplitter=InDet__TotPixelClusterSplitter (name="TotPixelClusterSplitter")

            #ToolSvc += TotPixelClusterSplitter
            #if (InDetFlags.doPrintConfigurables()):
            #  print TotPixelClusterSplitter

            # remember splitter tool    
            clusterSplitterTool = TotPixelClusterSplitter
                        
      #
      # --- now load the framework for the clustering
      #
      #
      from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
      InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool")

      acc.addPublicTool(InDetClusterMakerTool)
      #InDetClusterMakerTool = InDet__ClusterMakerTool(name                 = "InDetClusterMakerTool",
      #                                          ##PixelCalibSvc        = None,
      #                                          PixelOfflineCalibSvc = None,
      #                                          UsePixelCalibCondDB  = False,
      #						 PixelLorentzAngleTool = None )




      from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
      InDetMergedPixelsTool = InDet__MergedPixelsTool(name                    = "InDetMergedPixelsTool", 
                                                      globalPosAlg            = InDetClusterMakerTool,
                                                      MinimalSplitSize        = 0,
                                                      MaximalSplitSize        = 49,
                                                      MinimalSplitProbability = 0,
						      ###SplitClusterAmbiguityMap= 'SplitClusterAmbiguityMap',
                                                      DoIBLSplitting = True)
      # assign the tools if there are any                                                
      if not flags.InDetFlags.doTIDE_Ambi and clusterSplitProbTool is not None : InDetMergedPixelsTool.SplitProbTool   = clusterSplitProbTool
      if not flags.InDetFlags.doTIDE_Ambi and clusterSplitterTool is not None  : InDetMergedPixelsTool.ClusterSplitter = clusterSplitterTool

      sub_acc = ComponentAccumulator()      
      acc.addPublicTool(InDetMergedPixelsTool)
      acc.merge(sub_acc)
      return acc,InDetMergedPixelsTool


'''
def InDetMergedPixelsToolCfgGoetz(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__MergedPixelsTool
    sub_acc,tool = InDetClusterMakerToolCfg(flags)
    acc.merge(sub_acc)
    InDetMergedPixelsTool = InDet__MergedPixelsTool("InDetMergedPixelsTool",
                                                    globalPosAlg             = tool,
                                                    MinimalSplitSize         = 0,
                                                    MaximalSplitSize         = 49,
                                                    MinimalSplitProbability  = 0,
                                                    DoIBLSplitting           = True,
                                                    SplitClusterAmbiguityMap = flags.get("InDetKeys.SplitClusterAmbiguityMap"))

    if not flags.get("InDetFlags.doTIDE_Ambi")  and flags.get("InDetFlags.doPixelClusterSplitting") :
        # @TODO handle all conditions by getPixelClusterSplitterTool rather than duplicating the conditions here
        if flags.get("InDetFlags.pixelClusterSplittingType") == 'NeuralNet' :
            sub_acc,tool = NnPixelClusterSplitProbToolCfg(flags)
            acc.merge(sub_acc)
            InDetMergedPixelsTool.SplitProbTool = tool
        if flags.get("InDetFlags.pixelClusterSplittingType") in ['NeuralNet' , 'AnalogClus' ] :
            sub_acc,tool = PixelClusterSplitterToolCfg(flags)
            acc.merge(sub_acc)
            InDetMergedPixelsTool.ClusterSplitter = tool
    acc.addPublicTool(InDetMergedPixelsTool)
    return acc,InDetMergedPixelsTool
'''



###------------------------------------------------------------------------------
'''
def InDetClusterMakerToolCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
    InDetClusterMakerTool=InDet__ClusterMakerTool("InDetClusterMakerTool")
    ##if flags.get("DetFlags.makeRIO.pixel_on") and not (flags.get("athenaCommonFlags.isOnline") or flags.get("InDetFlags.doSLHC")) :
    if flags.DetFlags.makeRIO.pixel_on and not (flags.athenaCommonFlags.isOnline or flags.InDetFlags.doSLHC) :
        sub_acc,tool = PixelCalibSvcCfg(flags)
        acc.merge(sub_acc)
        InDetClusterMakerTool.PixelCalibSvc        = None
        sub_acc,svc = PixelOfflineCalibSvcCfg(flags)
        acc.merge(sub_acc)
        InDetClusterMakerTool.PixelOfflineCalibSvc = svc
        InDetClusterMakerTool.UsePixelCalibCondDB  = True
    else :
        InDetClusterMakerTool.PixelCalibSvc        = None
        InDetClusterMakerTool.PixelOfflineCalibSvc = None
        InDetClusterMakerTool.UsePixelCalibCondDB  = False
        ##InDetClusterMakerTool.PixelModuleData = ""
    InDetClusterMakerTool.SCTLorentzAngleTool  = None  ####################
    acc.addPublicTool(InDetClusterMakerTool)
    return acc,InDetClusterMakerTool
'''

def InDetClusterMakerToolCfg(flags, **kwargs) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
    InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool")

    ##################################### Edited ########################################
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
    sctLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool
    sctSiliconConditionsTool = sctLorentzAngleToolSetup.sctSiliconConditionsTool
    #####################################################################################
    SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
    #acc.addCondAlgo(SCTSiLorentzAngleCondAlg(name = "SCTSiLorentzAngleCondAlg",
    #                                       SiConditionsTool = sctSiliconConditionsTool,
    #                                       UseMagFieldSvc = True,
    #                                       UseMagFieldDcs = False))

    SCTLorentzAngleTool = SiLorentzAngleTool(name = "SCTLorentzAngleTool", DetectorName="SCT", SiLorentzAngleCondData="SCTSiLorentzAngleCondData")
    SCTLorentzAngleTool.UseMagFieldSvc = True #may need also MagFieldSvc instance
    acc.addPublicTool(SCTLorentzAngleTool)

    #channelFolder = "/SCT/DAQ/Config/Chip"
    #moduleFolder = "/SCT/DAQ/Config/Module" 
    #murFolder = "/SCT/DAQ/Config/MUR"
    #SCT_ConfigurationCondAlg=CompFactory.SCT_ConfigurationCondAlg
    #acc.addCondAlgo(SCT_ConfigurationCondAlg(ReadKeyChannel = channelFolder,
    #                                       ReadKeyModule = moduleFolder,
    #                                       ReadKeyMur = murFolder))
    #acc.merge(addFolders(flags, [channelFolder, moduleFolder, murFolder], "SCT", className="CondAttrListVec"))

    ##InDetClusterMakerTool.SCTLorentzAngleTool  = None
    acc.addPublicTool(InDetClusterMakerTool)

    return acc,InDetClusterMakerTool
##------------------------------------------------------------------------------

def PixelOfflineCalibSvcCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    sub_acc, hist_folder = addTHistOutputCfg(flags, 'PIXEL_Calib', 'PixelHistoCalib.root')
    acc.merge( sub_acc )

    from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
    sub_acc,tool = PixelRecoDbToolCfg( flags )
    acc.merge(sub_acc)
    svc = PixelOfflineCalibSvc( PixelRecoDbTool = tool )
    acc.addService(svc)
    return acc,svc

##------------------------------------------------------------------------------

def PixelRecoDbToolCfg(flags) :
    # Load Pixel reconstruction parameters
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    sub_acc, hist_folder = addTHistOutputCfg(flags, 'PIXEL', 'PixelHisto.root')
    acc.merge( sub_acc )

    if not flags.get("athenaCommonFlags.isOnline") :
        sub_acc,alg = PixelCalibCondAlgCfg(flags)
        acc.merge( sub_acc  )

    from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
    tool = PixelRecoDbTool()
    if not flags.get("athenaCommonFlags.isOnline") :
        tool.InputSource  = 2
        tool.CalibFolder  = "PixelOfflineCalibData"
    else :
        tool.InputSource  = 1,
        tool.CalibFolder  = ""
    acc.addPublicTool(tool)
    return acc,tool
##------------------------------------------------------------------------------
def addTHistOutputCfg(flags, folder, file_name, opt='RECREATE') :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    if len(folder) == 0 or folder[-1]=='/' :
        raise Exception('Got a root folder name %s, but root folder should not end with "/"' % folder)
    from GaudiSvc.GaudiSvcConf import THistSvc
    svc=THistSvc( Output  = [ "%s DATAFILE='%s' TYPE='ROOT' OPT='%s'" % (folder,file_name,opt) ] )
    acc.addService(svc)
    return acc, folder

##------------------------------------------------------------------------------

def PixelCalibCondAlgCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    acc = ComponentAccumulator()

    acc.merge( addFolders(flags, '<db>COOLOFL_PIXEL/OFLP200</db> /PIXEL/PixReco'))

    from PixelCalibAlgs.PixelCalibAlgsConf import PixelCalibCondAlg
    alg = PixelCalibCondAlg("PixelCalibCondAlg",
                           ReadKey  = "/PIXEL/PixReco",
                           WriteKey = "PixelOfflineCalibData")
    acc.addCondAlgo( alg )
    return acc,alg

##------------------------------------------------------------------------------



def NnPixelClusterSplitProbToolCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    # --- new NN prob tool
    MultiplicityContent = [1 , 1 , 1]
    if flags.get("InDetFlags.doSLHC") :
        from SiClusterizationTool.SiClusterizationToolConf import InDet__TruthPixelClusterSplitProbTool as PixelClusterSplitProbTool
    else:
        from SiClusterizationTool.SiClusterizationToolConf import InDet__NnPixelClusterSplitProbTool as PixelClusterSplitProbTool
    sub_acc, tool = NnClusterizationFactoryCfg(flags)
    acc.merge(sub_acc)
    NnPixelClusterSplitProbTool = PixelClusterSplitProbTool("NnPixelClusterSplitProbTool",
                                                            PriorMultiplicityContent = MultiplicityContent,
                                                            NnClusterizationFactory  = tool,
                                                            useBeamSpotInfo          = flags.get("InDetFlags.useBeamConstraint"))
    acc.addPublicTool(NnPixelClusterSplitProbTool)
    return acc,NnPixelClusterSplitProbTool

##------------------------------------------------------------------------------

def PixelClusterSplitterToolCfg(flags) :
    cluster_splitting_map={'NeuralNet'  : NnPixelClusterSplitterCfg ,
                           'AnalogClus' : TotPixelClusterSplitterCfg }
    return cluster_splitting_map.get( flags.get("InDetFlags.pixelClusterSplittingType") )(flags)

##------------------------------------------------------------------------------

def NnClusterizationFactoryCfg(flags) :
    # @TODO needs CoolHistSvc
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    acc = ComponentAccumulator()
    sub_acc,PixelCalibSvc      = PixelCalibSvcCfg(flags)
    acc.merge(sub_acc)

    sub_acc,NetworkToHistoTool = NeuralNetworkToHistoToolCfg(flags)
    acc.merge(sub_acc)

    acc.merge( addFolders( flags, '<db>COOLOFL_PIXEL/OFLP200</db> /PIXEL/PixelClustering/PixelClusNNCalib') )

    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory
    NnClusterizationFactory = InDet__NnClusterizationFactory("NnClusterizationFactory",
                                                             PixelCalibSvc                      = PixelCalibSvc,
                                                             NetworkToHistoTool                 = NetworkToHistoTool,
                                                             CoolFolder                         = "/PIXEL/PixelClustering/PixelClusNNCalib",
                                                             useRecenteringNNWithTracks         = False,
                                                             correctLorShiftBarrelWithoutTracks = 0.,
                                                             LoadNoTrackNetwork                 = True,
                                                             LoadWithTrackNetwork               = True)

    if "R2" in flags.get("globalflags.DetDescrVersion") or "IBL3D25" in flags.get("globalflags.DetDescrVersion") :
        NnClusterizationFactory.doRunI                             = False
        NnClusterizationFactory.useToT                             = flags.get("InDetFlags.doNNToTCalibration")
        NnClusterizationFactory.useRecenteringNNWithoutTracks      = False
        NnClusterizationFactory.correctLorShiftBarrelWithTracks    = 0.
    else :
        NnClusterizationFactory.doRunI                             = True
        NnClusterizationFactory.useToT                             = False
        NnClusterizationFactory.useRecenteringNNWithoutTracks      = True
        NnClusterizationFactory.correctLorShiftBarrelWithTracks    = 0.030
    acc.addPublicTool(NnClusterizationFactory)
    return acc,NnClusterizationFactory

##-----------------------------------------------------------------------------------

def PixelCalibSvcCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    #####if flags.get("athenaCommonFlags.isOnline") or not flags.get("DetFlags.haveRIO.pixel_on") :
    if flags.athenaCommonFlags.isOnline or not flags.DetFlags.haveRIO.pixel_on :
       return acc,None


    from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
    sub_acc,tool = PixelCalibDbToolCfg(flags)
    print 'DEBUG PixelCalibDbToolCfg type:%s' % type(tool)
    acc.merge( sub_acc )

    svc = PixelCalibSvc(CalibrationDbTool = tool )
    print 'DEBUG PixelCalibDbToolCfg type:%s' % type(tool)
    print 'DEBUG PixelCalibSvc.CalibrationDbTool type:%s' % type(svc.CalibrationDbTool)
    acc.addService(svc)
    return acc,svc

##--------------------------------------------------------------------------------------

def NeuralNetworkToHistoToolCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    acc = ComponentAccumulator()
    tool = Trk__NeuralNetworkToHistoTool(name = "NeuralNetworkToHistoTool")
    # used as a private tool
    # acc.addPublicTool(tool)
    return acc,tool

##--------------------------------------------------------------------------------------

def PixelCalibDbToolCfg(flags) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge( addFolders( flags, '<db>COOLOFL_PIXEL/OFLP200</db> /PIXEL/PixCalib') )
    tool = PixelCalibDbTool( CalibFolder = "/PIXEL/PixCalib" )
    acc.addPublicTool(tool)
    return acc,tool
##--------------------------------------------------------------------------------------

#def InDetPixelGangedAmbiguitiesFinderCfg(flags, **kwargs):
#    from SiClusterizationTool.SiClusterizationToolConf import InDet__PixelGangedAmbiguitiesFinder
#    result = ComponentAccumulator() 
#    result.merge( InDet__PixelGangedAmbiguitiesFinder(name = "InDetPixelGangedAmbiguitiesFinder") )
#    return result

##################################################################
##################################################################
##################################################################
##################################################################
##################################################################
##################################################################
##################################################################
##################################################################
##################################################################
##################################################################
##################################################################



def InDet_SCTClusterizationCfg(flags, **kwargs) :
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()

    #####################
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setup()

    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()


    #### Clustering tool ######
    accbuf, InDetClusterMakerTool = InDetClusterMakerToolCfg(flags, **kwargs)
    acc.merge(accbuf)
    from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
    InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name              = "InDetSCT_ClusteringTool",
                                                          globalPosAlg     = InDetClusterMakerTool,
                                                          conditionsTool = InDetSCT_ConditionsSummaryToolWithoutFlagged)
    if flags.InDetFlags.selectSCTIntimeHits :
       if flags.InDetFlags.InDet25nsec : 
          InDetSCT_ClusteringTool.timeBins = "01X" 
       else: 
          InDetSCT_ClusteringTool.timeBins = "X1X" 

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
    acc.addEventAlgo( InDet__SCT_Clusterization(name                    = "InDetSCT_Clusterization",
                                                          clusteringTool          = InDetSCT_ClusteringTool,
                                                          # ChannelStatus         = InDetSCT_ChannelStatusAlg,
                                                          DataObjectName          = 'SCT_RDOs', ##InDetKeys.SCT_RDOs(),
                                                          ClustersName            = 'SCT_Clusters', ##InDetKeys.SCT_Clusters(),
                                                          conditionsTool          = InDetSCT_ConditionsSummaryToolWithoutFlagged  ) )

    return acc


def InDet_SCTClusterizationPUCfg(flags, **kwargs) :
    return InDet_SCTClusterizationCfg(flags, **setDefaults( kwargs,
        name                    = "InDetSCT_ClusterizationPU",
        DataObjectName          = flags.InDetKeys.SCT_PU_RDOs,
        ClustersName            = flags.InDetKeys.SCT_PU_Clusters))


if __name__ == '__main__':
	from AthenaCommon.Configurable import Configurable
	Configurable.configurableRun3Behavior=1
	import sys
	sys.path.insert(1, "../share")
	import IDTestFlags
	flags = IDTestFlags.IDTestFlags_q221()
	#acc = InDetBCM_ZeroSuppressionCfg(flags)
	#acc.printConfig()