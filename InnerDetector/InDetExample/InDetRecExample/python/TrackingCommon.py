# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon.Logging import logging
log = logging.getLogger('TrackingCommon')

def createAndAddCondAlg(creator, the_name, **kwargs) :
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AlgSequence import AthSequencer
    cond_seq=AthSequencer("AthCondSeq")
    for seq in [AlgSequence(),cond_seq] :
        log.debug('createAndAddCondAlg match ?  %s == %s ? %s ', dir(seq), the_name, hasattr(seq,the_name) )
        if hasattr(seq,the_name) :
            if seq.getName() != "AthCondSeq" :
                raise Exception('Algorithm already in a sequnece but not the conditions seqence')
            return
    cond_seq += creator(**kwargs)

def createAndAddEventAlg(creator, the_name, **kwargs) :
    from AthenaCommon.AlgSequence import AlgSequence
    seq=AlgSequence()
    log.debug('createAndAddEventAlg match ?  %s == %s ? %s ', seq.getChildren(), the_name, hasattr(seq,the_name) )
    if hasattr(seq,the_name) :
        return getattr(seq,the_name)
    alg = creator(the_name,**kwargs)
    seq += alg
    return alg

def setDefaults(kwargs, **def_kwargs) :
    def_kwargs.update(kwargs)
    return def_kwargs

def copyArgs(kwargs, copy_list) :
    dict_copy={}
    for elm in copy_list :
        if elm in kwargs :
            dict_copy[elm]=kwargs[elm]
    return dict_copy

def splitDefaultPrefix(name) :
    default_prefix=''
    for prefix in ['InDet','InDetTrig'] :
        if name[0:len(prefix)] == prefix :
            name=name[len(prefix):]
            default_prefix=prefix
            break
    return default_prefix,name

def makeName( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

def makeNameGetPreAndSuffix( name, kwargs) :
    default_prefix,name=splitDefaultPrefix(name)
    namePrefix=kwargs.pop('namePrefix',default_prefix)
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix,namePrefix,nameSuffix

def getDefaultName(func) :
    # @TODO only works for python 2
    import inspect
    defaults=inspect.getargspec(func).defaults
    if len(defaults)==0 :
        raise Exception("Missing default name for %s" % func.__name__)
    return defaults[0]

def makePublicTool(tool_creator) :
    '''
    If the decorated method gets called the created tool will be added to ToolSvc
    '''
    def createPublicTool(*args,**kwargs):
        from AthenaCommon.AppMgr import ToolSvc
        name=kwargs.pop('name',None)
        private=kwargs.pop("private",False)
        if len(args) == 1 :
            if name is not None :
                raise Exception('Name given as positional and keyword argument')
            name=args[0]

        if name is None :
            name=getDefaultName(tool_creator)
        orig_name = name
        default_prefix,name=splitDefaultPrefix(name)
        the_name =  kwargs.get('namePrefix',default_prefix) + name + kwargs.get('nameSuffix','')

        if private is True or the_name not in ToolSvc :
            if len(args) > 1 :
                raise Exception('Too many positional arguments')
            tool = tool_creator(orig_name, **kwargs)
            if tool is None :
                return None
            if the_name != tool.name() :
                raise Exception('Tool has not the exepected name %s but %s' % (the_name, tool.name()))
            if private is False :
                log.debug ('Add to ToolSvc %s' % (tool.name()))
                ToolSvc += tool
            return tool
        else :
            return getattr(ToolSvc, the_name)

    createPublicTool.__name__   = tool_creator.__name__
    createPublicTool.__module__ = tool_creator.__module__
    return createPublicTool


def getInDetNewTrackingCuts() :
    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doDBMstandalone():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
    elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
    elif InDetFlags.doVtxBeamSpot():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxBeamSpot")
    elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
    elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
    elif InDetFlags.doSLHC():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("SLHC")
    elif InDetFlags.doIBL():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("IBL")
    elif InDetFlags.doHighPileup():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HighPileup")
    elif InDetFlags.doMinBias():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("MinBias")
    elif InDetFlags.doDVRetracking():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("LargeD0")
    else:
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
    return InDetNewTrackingCuts
    # --- backward compatible


def getPixelRIO_OnTrackErrorScalingDbOverrideCondAlg( **kwargs) :
    '''
    usage:
       createAndAddCondAlg( getPixelRIO_OnTrackErrorScalingDbOverrideCondAlg, 'PixelRIO_OnTrackErrorScalingDbOverrideCondAlg' )
    '''
    the_name=kwargs.pop("name",'PixelRIO_OnTrackErrorScalingDbOverrideCondAlg')
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    # kPixBarrelPhi
    params   = [ 10. , 0.0044]
    # kPixBarrelEta
    params  += [ 10. , 0.0312]
    # kPixEndcapPhi,
    params  += [ 10. , 0.026]
    # kPixEndcapEta,
    params  += [ 10. , 0.0]
    if geoFlags.isIBL() :
        error_scaling_type='PixelRIO_OnTrackErrorScaling'
        # kPixIBLPhi
        params  += [ 10. , 0.0044]
        # kPixIBLEta
        params  += [ 10. , 0.0312]
    else :
        error_scaling_type='PixelRIO_OnTrackErrorScalingRun1'


    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import RIO_OnTrackErrorScalingDbOverrideCondAlg
    return RIO_OnTrackErrorScalingDbOverrideCondAlg( name = the_name,
                                                     **setDefaults( kwargs,
                                                                    ErrorScalingTypeName  = error_scaling_type,
                                                                    WriteKey              = "/Indet/TrkErrorScalingPixel",
                                                                    ErorScalingParameters = params,
                                                                    OutputLevel = 1) )  # VERBOSE

def getRIO_OnTrackErrorScalingCondAlg( **kwargs) :
    the_name=kwargs.pop("name",None)
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    if geoFlags.isIBL() :
        error_scaling_type =   ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else :
        error_scaling_type =   ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]

    error_scaling_type +=      ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey +=    ["/Indet/TrkErrorScalingSCT"]

    error_scaling_type +=      ["TRTRIO_OnTrackErrorScaling"]
    error_scaling_outkey +=    ["/Indet/TrkErrorScalingTRT"]

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested( "/Indet/TrkErrorScaling" ):
        log.debug('request /Indet/TrkErrorScaling')
        conddb.addFolderSplitOnline('INDET','/Indet/Onl/TrkErrorScaling','/Indet/TrkErrorScaling', className="CondAttrListCollection")
    else :
        log.debug('folder /Indet/TrkErrorScaling already requested.')
    for elm in conddb.iovdbsvc.Folders :
        log.debug('IOVDbSvc folder %s', elm)


    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import RIO_OnTrackErrorScalingCondAlg
    if the_name is None :
        return RIO_OnTrackErrorScalingCondAlg( **setDefaults(kwargs,
                                                             ReadKey             = "/Indet/TrkErrorScaling",
                                                             ErrorScalingType    = error_scaling_type,
                                                             OutKeys             = error_scaling_outkey) )
    else :
        return RIO_OnTrackErrorScalingCondAlg( the_name,
                                               **setDefaults(kwargs,
                                                             ReadKey             = "/Indet/TrkErrorScaling",
                                                             ErrorScalingType    = error_scaling_type,
                                                             OutKeys             = error_scaling_outkey) )




def getEventInfoKey() :
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.DetFlags    import DetFlags

    isData = (globalflags.DataSource == 'data')

    eventInfoKey = "EventInfo"
    if not isData:
        eventInfoKey = "McEventInfo"
    if globalflags.isOverlay() and isData :
        if DetFlags.overlay.pixel_on() or DetFlags.overlay.SCT_on() or DetFlags.overlay.TRT_on():
            from OverlayCommonAlgs.OverlayFlags import overlayFlags
            eventInfoKey = (overlayFlags.dataStore() + '+' + eventInfoKey).replace("StoreGateSvc+","")
        else :
            eventInfoKey = "McEventInfo"
    return eventInfoKey


def getNeuralNetworkToHistoTool(**kwargs) :
    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    name = kwargs.pop('name',"NeuralNetworkToHistoTool")
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,name) :
        return getattr(ToolSvc,name)

    NeuralNetworkToHistoTool=Trk__NeuralNetworkToHistoTool(name,
                                                           **kwargs)
    ToolSvc += NeuralNetworkToHistoTool
    return NeuralNetworkToHistoTool

def getPixelClusterNnCondAlg(**kwargs) :
    track_nn = kwargs.pop('TrackNetwork',False)
    nn_names = [
          "NumberParticles_NoTrack/",
          "ImpactPoints1P_NoTrack/",
          "ImpactPoints2P_NoTrack/",
          "ImpactPoints3P_NoTrack/",
          "ImpactPointErrorsX1_NoTrack/",
          "ImpactPointErrorsX2_NoTrack/",
          "ImpactPointErrorsX3_NoTrack/",
          "ImpactPointErrorsY1_NoTrack/",
          "ImpactPointErrorsY2_NoTrack/",
          "ImpactPointErrorsY3_NoTrack/" ]

    if track_nn :
        nn_names = [ elm.replace('_NoTrack', '')  for elm in nn_names ]

    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('/PIXEL/PixelClustering/PixelClusNNCalib'):
        # COOL binding
        conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixelClustering/PixelClusNNCalib",
                                    "/PIXEL/PixelClustering/PixelClusNNCalib",className='CondAttrListCollection')

    kwargs=setDefaults(kwargs,
                       NetworkNames = nn_names,
                       WriteKey     ='PixelClusterNN' if not track_nn else 'PixelClusterNNWithTrack')

    if 'NetworkToHistoTool' not in kwargs :
        kwargs = setDefaults( kwargs,
                              NetworkToHistoTool   = getNeuralNetworkToHistoTool() )

    from SiClusterizationTool.SiClusterizationToolConf import InDet__TTrainedNetworkCondAlg
    return InDet__TTrainedNetworkCondAlg(kwargs.pop("name", 'PixelClusterNnCondAlg'), **kwargs)

def getLWTNNCondAlg(**kwargs) :

    # Check for the folder
    from IOVDbSvc.CondDB import conddb
    if not conddb.folderRequested('/PIXEL/PixelClustering/PixelClusNNCalibJSON'):
        # COOL binding
        conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixelClustering/PixelNNCalibJSON",
                                    "/PIXEL/PixelClustering/PixelNNCalibJSON",className='CondAttrListCollection')

    # What we'll store it as
    kwargs=setDefaults(kwargs,
                       WriteKey = 'PixelClusterNNJSON')

    # Set up the algorithm
    from SiClusterizationTool.SiClusterizationToolConf import InDet__LWTNNCondAlg
    return InDet__LWTNNCondAlg(kwargs.pop("name", "LWTNNCondAlg"),**kwargs)

def getPixelClusterNnWithTrackCondAlg(**kwargs) :

    kwargs = setDefaults( kwargs,
                          TrackNetwork = True,
                          name         ='PixelClusterNnWithTrackCondAlg')
    return getPixelClusterNnCondAlg( **kwargs )

def getPixelLorentzAngleTool(name='PixelLorentzAngleTool', **kwargs) :
    the_name = makeName( name, kwargs)
    if the_name != "PixelLorentzAngleTool" :
        raise Exception('There should be only one Pixel Lorentz angle tool configuration named "PixelLorentzAngleTool"')
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, the_name ):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        PixelLorentzAngleToolSetup()
    return getattr(ToolSvc,the_name)

def getSCTLorentzAngleTool(name='SCTLorentzAngleTool', **kwargs) :
    the_name = makeName( name, kwargs)
    if the_name != "SCTLorentzAngleTool" :
        raise Exception('There should be only one SCT Lorentz angle tool configuration named "SCTLorentzAngleTool"')
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
    return SCTLorentzAngleToolSetup().SCTLorentzAngleTool

@makePublicTool
def getNnClusterizationFactory(name='NnClusterizationFactory', **kwargs) :
    the_name = makeName( name, kwargs)
    from SiClusterizationTool.SiClusterizationToolConf import InDet__NnClusterizationFactory

    if 'PixelLorentzAngleTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelLorentzAngleTool = getPixelLorentzAngleTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    useTTrainedNetworks = InDetFlags.useNNTTrainedNetworks()
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
    do_runI = geoFlags.Run() not in ["RUN2", "RUN3"]
    
    if useTTrainedNetworks :
      log.debug("Setting up TTrainedNetworks")
      createAndAddCondAlg( getPixelClusterNnCondAlg,         'PixelClusterNnCondAlg',          GetInputsInfo = do_runI)
      createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,'PixelClusterNnWithTrackCondAlg', GetInputsInfo = do_runI)
    else :

      ######################################
      # Temporary - pixel clustering setup #
      ######################################
      # Allow use of folder that exists but is not yet in global tag.
      # Different names in different DB instances....
      if not ('conddb' in dir()):
        from IOVDbSvc.CondDB import conddb

      if (conddb.dbmc == "OFLP200" or (conddb.dbdata=="OFLP200" and globalflags.DataSource=='data')) :
        conddb.addOverride("/PIXEL/PixelClustering/PixelNNCalibJSON","PixelNNCalibJSON-SIM-RUN2-000-00")
      if ((conddb.dbmc == "CONDBR2" and globalflags.DataSource!='data') or conddb.dbdata == "CONDBR2") :
        conddb.addOverride("/PIXEL/PixelClustering/PixelNNCalibJSON","PixelNNCalibJSON-DATA-RUN2-000-00")
      ## End of temporary code

      log.debug("Setting up lwtnn system")
      createAndAddCondAlg( getLWTNNCondAlg,                  'LWTNNCondAlg')

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,
                          doRunI                             = do_runI,
                          useToT                             = False if do_runI else InDetFlags.doNNToTCalibration(),
                          useRecenteringNNWithoutTracks      = True  if do_runI else False,  # default,
                          useRecenteringNNWithTracks         = False if do_runI else False,  # default,
                          correctLorShiftBarrelWithoutTracks = 0,
                          correctLorShiftBarrelWithTracks    = 0.030 if do_runI else 0.000,  # default,
                          useTTrainedNetworks                = useTTrainedNetworks,
                          NnCollectionReadKey                = 'PixelClusterNN',
                          NnCollectionWithTrackReadKey       = 'PixelClusterNNWithTrack',
                          NnCollectionJSONReadKey            = '' if useTTrainedNetworks else 'PixelClusterNNJSON')
    return InDet__NnClusterizationFactory(name=the_name, **kwargs)

@makePublicTool
def getInDetPixelClusterOnTrackToolBase(name, **kwargs) :
    the_name                    = makeName( name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    from InDetRecExample.InDetJobProperties import InDetFlags
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__PixelClusterOnTrackTool

    if InDetFlags.doCosmics() or InDetFlags.doDBMstandalone():
        kwargs = setDefaults(kwargs,
                             ErrorStrategy    = 0,
                             PositionStrategy = 0)

    from InDetRecExample.InDetKeys import InDetKeys
    kwargs = setDefaults(kwargs,
                         DisableDistortions       = (InDetFlags.doFatras() or InDetFlags.doDBMstandalone()),
                         applyNNcorrection        = ( InDetFlags.doPixelClusterSplitting() and
                                                      InDetFlags.pixelClusterSplittingType() == 'NeuralNet' and not InDetFlags.doSLHC()),
                         NNIBLcorrection          = ( InDetFlags.doPixelClusterSplitting() and
                                                      InDetFlags.pixelClusterSplittingType() == 'NeuralNet' and not InDetFlags.doSLHC()),
                         SplitClusterAmbiguityMap = InDetKeys.SplitClusterAmbiguityMap() + split_cluster_map_extension,
                         RunningTIDE_Ambi         = InDetFlags.doTIDE_Ambi() )

    return InDet__PixelClusterOnTrackTool(the_name , **kwargs)

def getInDetPixelClusterOnTrackToolNNSplitting(name='InDetPixelClusterOnTrackToolNNSplitting', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doPixelClusterSplitting() and InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
        if 'NnClusterizationFactory' not in kwargs :
            kwargs = setDefaults(kwargs, NnClusterizationFactory  = getNnClusterizationFactory())

    return getInDetPixelClusterOnTrackToolBase(name=name, **kwargs)

def getInDetPixelClusterOnTrackTool(name='InDetPixelClusterOnTrackTool', **kwargs) :
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getPixelLorentzAngleTool())
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doDigitalROTCreation() :
        return getInDetPixelClusterOnTrackToolDigital(name=name, **kwargs)
    else:
        return getInDetPixelClusterOnTrackToolNNSplitting(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolPattern(name='InDetPixelClusterOnTrackToolPattern', **kwargs) :
    return getInDetPixelClusterOnTrackTool(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolDigital(name='InDetPixelClusterOnTrackToolDigital', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getPixelLorentzAngleTool())

    if InDetFlags.doDigitalROTCreation():
        kwargs = setDefaults(kwargs,
                             applyNNcorrection = False,
                             NNIBLcorrection   = False,
                             ErrorStrategy     = 2,
                             PositionStrategy  = 1,
                             SplitClusterAmbiguityMap = "")
    else :
        kwargs = setDefaults(kwargs,
                             SplitClusterAmbiguityMap = "")

    return getInDetPixelClusterOnTrackToolBase(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolDBM(name='InDetPixelClusterOnTrackToolDBM', **kwargs) :
    return getInDetPixelClusterOnTrackToolBase(name=name, **setDefaults(kwargs,
                                                                        DisableDistortions = True,
                                                                        applyNNcorrection  = False,
                                                                        NNIBLcorrection    = False,
                                                                        RunningTIDE_Ambi   = False,
                                                                        ErrorStrategy      = 0,
                                                                        PositionStrategy   = 0))

def getInDetBroadPixelClusterOnTrackTool(name='InDetBroadPixelClusterOnTrackTool', **kwargs) :
    return getInDetPixelClusterOnTrackTool(name=name, **setDefaults(kwargs, ErrorStrategy  = 0))

@makePublicTool
def getInDetSCT_ClusterOnTrackTool(name='InDetSCT_ClusterOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getSCTLorentzAngleTool())

    kwargs = setDefaults(kwargs,
                         # CorrectionStrategy = -1,  # no position correction (test for bug #56477)
                         CorrectionStrategy = 0,  # do correct position bias
                         ErrorStrategy      = 2  # do use phi dependent errors
                        )
    from SiClusterOnTrackTool.SiClusterOnTrackToolConf import InDet__SCT_ClusterOnTrackTool
    return InDet__SCT_ClusterOnTrackTool (the_name, **kwargs)

def getInDetBroadSCT_ClusterOnTrackTool(name='InDetBroadSCT_ClusterOnTrackTool', **kwargs) :
    return getInDetSCT_ClusterOnTrackTool(name=name, **setDefaults( kwargs, ErrorStrategy  = 0) )


@makePublicTool
def getInDetBroadTRT_DriftCircleOnTrackTool(name='InDetBroadTRT_DriftCircleOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackNoDriftTimeTool
    return InDet__TRT_DriftCircleOnTrackNoDriftTimeTool(the_name)

def getInDetTRT_DriftCircleOnTrackNoDriftTimeTool(**kwargs) :
    return getInDetBroadTRT_DriftCircleOnTrackTool(**kwargs)


def getLumiCondDataKeyForTRTMuScaling(**kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    isHLT=kwargs.pop('isHLT',False)
    kwargs.pop('namePrefix','')
    if not InDetFlags.doCosmics() and InDetFlags.useMuForTRTErrorScaling():
        # @TODO remove flag useMuForTRTErrorScaling once TRT mu dependent ERROR scaling not needed any more or
        #       LuminosityCondAlgDefault always uses the correct database and   folder
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault,LuminosityCondAlgOnlineDefault
        from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
        lumiAlg = LuminosityCondAlgDefault() if not athenaCommonFlags.isOnline() and not isHLT else LuminosityCondAlgOnlineDefault()
        if lumiAlg is not None :
            return lumiAlg.LuminosityOutputKey
    return ''

# @TODO rename to InDetTRT_DriftCircleOnTrackTool ?
@makePublicTool
def getInDetTRT_DriftCircleOnTrackTool(name='TRT_DriftCircleOnTrackTool', **kwargs) :
    the_name = makeName( name, kwargs)
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    kwargs.pop('isHLT',None)
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')
    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackTool
    kwargs = setDefaults(kwargs,
                         TRTErrorScalingKey = '/Indet/TrkErrorScalingTRT',
                         LumiDataKey        = getLumiCondDataKeyForTRTMuScaling(**hlt_args))
    return InDet__TRT_DriftCircleOnTrackTool(name = the_name, **kwargs)


default_ScaleHitUncertainty = 2.5
@makePublicTool
def getInDetTRT_DriftCircleOnTrackUniversalTool(name='InDetTRT_RefitRotCreator',**kwargs) :
    the_name = makeName( name, kwargs)
    if 'RIOonTrackToolDrift' not in kwargs :
        kwargs = setDefaults(kwargs, RIOonTrackToolDrift = getInDetBroadTRT_DriftCircleOnTrackTool())
    if 'RIOonTrackToolTube' not in kwargs :
        kwargs = setDefaults(kwargs, RIOonTrackToolTube  = getInDetBroadTRT_DriftCircleOnTrackTool())

    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackUniversalTool
    return InDet__TRT_DriftCircleOnTrackUniversalTool(name = the_name, **setDefaults(kwargs,
                                                                                     ScaleHitUncertainty = default_ScaleHitUncertainty))

def getInDetTRT_DriftCircleOnTrackUniversalToolCosmics(name='TRT_DriftCircleOnTrackUniversalTool',**kwargs) :
    return getInDetTRT_DriftCircleOnTrackUniversalTool(name=name,ScaleHitUncertainty=2.)

@makePublicTool
def getInDetRotCreator(name='InDetRotCreator', **kwargs) :
    strip_args=['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix']
    pix_cluster_on_track_args = copyArgs(kwargs,strip_args)
    # note nameSuffix is strupped by makeName
    the_name = makeName( name, kwargs)
    for an_arg in  strip_args:
        kwargs.pop(an_arg,None)
    from InDetRecExample.InDetJobProperties import InDetFlags
    use_broad_cluster_pix = InDetFlags.useBroadPixClusterErrors() and (not InDetFlags.doDBMstandalone())
    use_broad_cluster_sct = InDetFlags.useBroadSCTClusterErrors() and (not InDetFlags.doDBMstandalone())

    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            kwargs = setDefaults( kwargs,
                                  ToolPixelCluster = getInDetBroadPixelClusterOnTrackTool(**pix_cluster_on_track_args))
        else :
            kwargs = setDefaults( kwargs,
                                  ToolPixelCluster = getInDetPixelClusterOnTrackTool(**pix_cluster_on_track_args))

    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_sct :
            kwargs = setDefaults( kwargs, ToolSCT_Cluster = getInDetBroadSCT_ClusterOnTrackTool())
        else :
            kwargs = setDefaults( kwargs, ToolSCT_Cluster = getInDetSCT_ClusterOnTrackTool())

    if 'ToolTRT_DriftCircle' not in kwargs :
        kwargs = setDefaults( kwargs, ToolTRT_DriftCircle = getInDetTRT_DriftCircleOnTrackTool())


    kwargs = setDefaults( kwargs, Mode             = 'indet')

    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    return Trk__RIO_OnTrackCreator(name=the_name, **kwargs)

def getInDetRotCreatorPattern(name='InDetRotCreatorPattern', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolPattern(**pix_cluster_on_track_args))
    return getInDetRotCreator(name=name, **kwargs)


def getInDetRotCreatorDBM(name='InDetRotCreatorDBM', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])
        from InDetRecExample.InDetJobProperties import InDetFlags
        from AthenaCommon.DetFlags              import DetFlags
        if InDetFlags.loadRotCreator() and DetFlags.haveRIO.pixel_on():
            kwargs = setDefaults(kwargs,
                                 ToolPixelCluster = getInDetPixelClusterOnTrackToolDBM(**pix_cluster_on_track_args))
        else :
            kwargs = setDefaults(kwargs,
                                 ToolPixelCluster = getInDetPixelClusterOnTrackTool(**pix_cluster_on_track_args))
    return getInDetRotCreator(name=name, **kwargs)

def getInDetRotCreatorDigital(name='InDetRotCreatorDigital', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolDigital(**pix_cluster_on_track_args))
    return getInDetRotCreator(name=name, **kwargs)

# @TODO rename to InDetBroadRotCreator
def getInDetBroadRotCreator(name='InDetBroadInDetRotCreator', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','RenounceInputHandles','nameSuffix'])
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster    = getInDetBroadPixelClusterOnTrackTool(**pix_cluster_on_track_args))
    if 'ToolSCT_Cluster' not in kwargs :
        kwargs = setDefaults(kwargs,
                             ToolSCT_Cluster     = getInDetBroadSCT_ClusterOnTrackTool())

    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.haveRIO.TRT_on():
        if 'ToolTRT_DriftCircle' not in kwargs :
            kwargs = setDefaults(kwargs,
                                 ToolTRT_DriftCircle = getInDetBroadTRT_DriftCircleOnTrackTool())

    return getInDetRotCreator(name=name, **kwargs)


def getInDetRefitRotCreator(name='InDetRefitRotCreator', **kwargs) :
    ScaleHitUncertainty = kwargs.pop('ScaleHitUncertainty',default_ScaleHitUncertainty)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags

    if InDetFlags.redoTRT_LR():
        if DetFlags.haveRIO.TRT_on():
            if 'ToolTRT_DriftCircle' not in kwargs :
                kwargs = setDefaults(kwargs,
                                     ToolTRT_DriftCircle = getInDetTRT_DriftCircleOnTrackUniversalTool(ScaleHitUncertainty = ScaleHitUncertainty))

    return getInDetRotCreator(name = name, **kwargs)

@makePublicTool
def getInDetUpdator(name = 'InDetUpdator', **kwargs) :
    the_name = makeName( name, kwargs )
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.kalmanUpdator() == "fast" :
        from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk as Updator
    elif InDetFlags.kalmanUpdator() == "weight" :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanWeightUpdator as Updator
    elif InDetFlags.kalmanUpdator() == "smatrix" :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdatorSMatrix as Updator
    elif InDetFlags.kalmanUpdator() == "amg" :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdatorAmg as Updator
    else :
        from TrkMeasurementUpdator.TrkMeasurementUpdatorConf import Trk__KalmanUpdator as Updator
    return Updator(name = the_name, **kwargs)


@makePublicTool
def getInDetNavigator(name='InDetNavigator', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'TrackingGeometrySvc' not in kwargs :
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        kwargs = setDefaults(kwargs, TrackingGeometrySvc = AtlasTrackingGeometrySvc)

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    return Trk__Navigator(name=the_name,**kwargs )


@makePublicTool
def getInDetMaterialEffectsUpdator(name = "InDetMaterialEffectsUpdator", **kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    if not InDetFlags.solenoidOn():
        import AthenaCommon.SystemOfUnits as Units
        kwargs = setDefaults(kwargs,
                             EnergyLoss          = False,
                             ForceMomentum       = True,
                             ForcedMomentumValue = 1000*Units.MeV )
    from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
    return Trk__MaterialEffectsUpdator(name = the_name, **kwargs)

@makePublicTool
def getInDetPropagator(name='InDetPropagator',**kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.propagatorType() == "STEP":
        from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as Propagator
    else:
        from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator

    if InDetFlags.propagatorType() == "RungeKutta":
        kwargs=setDefaults(kwargs,
                           AccuracyParameter = 0.0001,
                           MaxStraightLineStep = .004 ) # Fixes a failed fit
    return Propagator(name = the_name, **kwargs)


@makePublicTool
def getInDetPatternPropagator(name='InDetPatternPropagator',**kwargs) :
    the_name = makeName( name, kwargs)
    from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator as Propagator
    return Propagator(name = the_name, **kwargs)

@makePublicTool
def getInDetPatternUpdator(name='InDetPatternUpdator',**kwargs) :
    the_name = makeName( name, kwargs)
    from TrkMeasurementUpdator_xk.TrkMeasurementUpdator_xkConf import Trk__KalmanUpdator_xk
    return Trk__KalmanUpdator_xk(name = the_name, **kwargs)


@makePublicTool
def getInDetGsfMeasurementUpdator(name='InDetGsfMeasurementUpdator', **kwargs) :
    the_name = makeName( name, kwargs )
    if 'Updator' not in kwargs  :
        kwargs=setDefaults(kwargs, Updator = getInDetUpdator() )
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMeasurementUpdator
    return Trk__GsfMeasurementUpdator( name = the_name, **kwargs )


@makePublicTool
def getInDetGsfMaterialUpdator(name='InDetGsfMaterialUpdator', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'MaximumNumberOfComponents' not in kwargs :
        kwargs=setDefaults(kwargs, MaximumNumberOfComponents = 12)

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
    return Trk__GsfMaterialMixtureConvolution (name = the_name, **kwargs)


@makePublicTool
def getInDetGsfExtrapolator(name='InDetGsfExtrapolator', **kwargs) :
    the_name = makeName(name,kwargs)
    if 'Propagators' not in kwargs :
        kwargs=setDefaults(kwargs, Propagators = [ getInDetPropagator() ] )

    if 'Navigator' not in kwargs :
        kwargs=setDefaults(kwargs, Navigator   =  getInDetNavigator() )

    if 'GsfMaterialConvolution' not in kwargs :
        kwargs=setDefaults(kwargs, GsfMaterialConvolution        = getInDetGsfMaterialUpdator())

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
    return Trk__GsfExtrapolator(name = the_name, **setDefaults(kwargs,
                                                               SearchLevelClosestParameters  = 10,
                                                               StickyConfiguration           = True,
                                                               SurfaceBasedMaterialEffects   = False ))

@makePublicTool
def getPRDtoTrackMapTool(name='PRDtoTrackMapTool',**kwargs) :
    the_name = makeName( name, kwargs)
    from TrkAssociationTools.TrkAssociationToolsConf import Trk__PRDtoTrackMapTool
    return Trk__PRDtoTrackMapTool( name=the_name, **kwargs)

@makePublicTool
def getInDetPRDtoTrackMapToolGangedPixels(name='PRDtoTrackMapToolGangedPixels',**kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetKeys                       import InDetKeys
    kwargs = setDefaults( kwargs,
                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                          addTRToutliers                 = True)

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRDtoTrackMapToolGangedPixels
    return InDet__InDetPRDtoTrackMapToolGangedPixels( name=the_name, **kwargs)


def getInDetTrigPRDtoTrackMapToolGangedPixels(name='InDetTrigPRDtoTrackMapToolGangedPixels',**kwargs) :
    return getInDetPRDtoTrackMapToolGangedPixels(name,
                                                 **setDefaults(kwargs,
                                                               PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap",
                                                               addTRToutliers                 = False))

def getInDetTrackPRD_Association(name='InDetTrackPRD_Association', **kwargs) :
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    if kwargs.get('TracksName',None) is None :
        raise Exception('Not TracksName argument provided')

    kwargs = setDefaults( kwargs,
                          AssociationTool    = getInDetPRDtoTrackMapToolGangedPixels() \
                                                    if 'AssociationTool' not in kwargs else None,
                          AssociationMapName = prefix+'PRDtoTrackMap'+suffix)

    from InDetTrackPRD_Association.InDetTrackPRD_AssociationConf import InDet__InDetTrackPRD_Association
    alg = InDet__InDetTrackPRD_Association(name = the_name, **kwargs)
    return alg


@makePublicTool
def getConstPRD_AssociationTool(name='ConstPRD_AssociationTool',**kwargs) :
    the_name,prefix,suffix=makeNameGetPreAndSuffix(name,kwargs)

    kwargs = setDefaults( kwargs,
                          SetupCorrect     = True,
                          PRDtoTrackMap    = prefix+'PRDtoTrackMap'+suffix)

    from TrkAssociationTools.TrkAssociationToolsConf import Trk__PRD_AssociationTool
    return Trk__PRD_AssociationTool(name=the_name, **kwargs)

@makePublicTool
def getInDetPrdAssociationTool(name='InDetPrdAssociationTool',**kwargs) :
    '''
    Provide an instance for all clients in which the tool is only set in c++
    '''
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetKeys                       import InDetKeys
    kwargs = setDefaults( kwargs,
                          PixelClusterAmbiguitiesMapName = InDetKeys.GangedPixelMap(),
                          addTRToutliers                 = True)

    from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
    return InDet__InDetPRD_AssociationToolGangedPixels(name=the_name, **kwargs)

def getInDetTrigPrdAssociationTool(name='InDetTrigPrdAssociationTool_setup',**kwargs) :
    return getInDetPrdAssociationTool(name,
                                      **setDefaults(kwargs,
                                                    PixelClusterAmbiguitiesMapName = "TrigPixelClusterAmbiguitiesMap",
                                                    addTRToutliers                 = False))


def getInDetPrdAssociationTool_setup(name='InDetPrdAssociationTool_setup',**kwargs) :
    '''
    Provide an instance for all clients which set the tool explicitely
    '''
    return getInDetPrdAssociationTool(name, **setDefaults(kwargs, SetupCorrect                   = True) )

def getInDetPixelConditionsSummaryTool() :
    from AthenaCommon.GlobalFlags import globalflags
    from InDetRecExample.InDetJobProperties import InDetFlags
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool("PixelConditionsSummaryTool",
                                                                 UseByteStream=(globalflags.DataSource=='data'))
    if InDetFlags.usePixelDCS():
        pixelConditionsSummaryToolSetup.IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ]
        pixelConditionsSummaryToolSetup.IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]

    return pixelConditionsSummaryToolSetup

@makePublicTool
def getInDetTestPixelLayerTool(name = "InDetTestPixelLayerTool", **kwargs) :
    the_name = makeName( name, kwargs)
    if 'PixelSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelSummaryTool = getInDetPixelConditionsSummaryTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,
                          CheckActiveAreas = InDetFlags.checkDeadElementsOnTrack(),
                          CheckDeadRegions = InDetFlags.checkDeadElementsOnTrack(),
                          CheckDisabledFEs = InDetFlags.checkDeadElementsOnTrack())

    from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
    return InDet__InDetTestPixelLayerTool(name = the_name, **kwargs)


# # set up the propagator for outside ID (A.S. needed as a fix for 14.5.0 )
# @makePublicTool
# def getInDetStepPropagator(name='InDetStepPropagator',**kwargs) :
#     from TrkExSTEP_Propagator.TrkExSTEP_PropagatorConf import Trk__STEP_Propagator as StepPropagator
#     return StepPropagator(name = name, **kwargs)

@makePublicTool
def getInDetMultipleScatteringUpdator(name='InDetMultipleScatteringUpdator', **kwargs) :
    the_name = makeName( name, kwargs)
    from TrkExTools.TrkExToolsConf import Trk__MultipleScatteringUpdator
    return Trk__MultipleScatteringUpdator(name = the_name, **setDefaults( kwargs, UseTrkUtils = False))

@makePublicTool
def getInDetExtrapolator(name='InDetExtrapolator', **kwargs) :
    the_name = makeName( name, kwargs)

    if 'Propagators' not in kwargs :
        kwargs = setDefaults( kwargs, Propagators = [ getInDetPropagator() ] ) # [ InDetPropagator, InDetStepPropagator ],
    propagator= kwargs.get('Propagators')[0].name() if  kwargs.get('Propagators',None) is not None and len(kwargs.get('Propagators',None))>0 else None

    if 'MaterialEffectsUpdators' not in kwargs :
        kwargs = setDefaults( kwargs, MaterialEffectsUpdators = [ getInDetMaterialEffectsUpdator() ] )
    material_updator= kwargs.get('MaterialEffectsUpdators')[0].name() if  kwargs.get('MaterialEffectsUpdators',None) is not None and len(kwargs.get('MaterialEffectsUpdators',None))>0  else None

    if 'Navigator' not in kwargs :
        kwargs = setDefaults( kwargs, Navigator               = getInDetNavigator())

    sub_propagators = []
    sub_updators    = []

    # -------------------- set it depending on the geometry ----------------------------------------------------
    # default for ID is (Rk,Mat)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for Calo is (Rk,MatLandau)
    sub_propagators += [ propagator ]
    sub_updators    += [ material_updator ]

    # default for MS is (STEP,Mat)
    #  sub_propagators += [ InDetStepPropagator.name() ]
    sub_updators    += [ material_updator ]
    # @TODO should check that all sub_propagators and sub_updators are actually defined.

    kwargs = setDefaults( kwargs,
                          SubPropagators          = sub_propagators,
                          SubMEUpdators           = sub_updators)

    from TrkExTools.TrkExToolsConf import Trk__Extrapolator
    return Trk__Extrapolator(name = the_name, **kwargs)


# @TODO move configuration of InDetSCT_ConditionsSummaryTool to a function
def_InDetSCT_ConditionsSummaryTool=None
def getInDetSCT_ConditionsSummaryTool() :
    return def_InDetSCT_ConditionsSummaryTool

@makePublicTool
def getInDetBoundaryCheckTool(name="InDetBoundarySearchTool", **kwargs):
    the_name = makeName(name, kwargs)
    from AthenaCommon.DetFlags import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags

    if 'SctSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, SctSummaryTool   = getInDetSCT_ConditionsSummaryTool()  if DetFlags.haveRIO.SCT_on()   else None)

    if 'PixelLayerTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelLayerTool   = getInDetTestPixelLayerTool())

    kwargs = setDefaults(
        kwargs,
        UsePixel=DetFlags.haveRIO.pixel_on(),
        UseSCT=DetFlags.haveRIO.SCT_on(),
    )

    from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
    return InDet__InDetBoundaryCheckTool(name=the_name, **kwargs)

@makePublicTool
def getInDetHoleSearchTool(name = 'InDetHoleSearchTool', **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags    import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags

    if 'Extrapolator' not in kwargs :
        kwargs = setDefaults( kwargs, Extrapolator     = getInDetExtrapolator())

    if 'BoundaryCheckTool' not in kwargs :
        kwargs = setDefaults( kwargs, BoundaryCheckTool= getInDetBoundaryCheckTool())

    if InDetFlags.doCosmics :
        kwargs = setDefaults( kwargs, Cosmics = True)

    kwargs = setDefaults( kwargs,
                          CountDeadModulesAfterLastHit = True)

    from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
    return InDet__InDetTrackHoleSearchTool(name = the_name, **kwargs)


@makePublicTool
def getInDetPixelToTPIDTool(name = "InDetPixelToTPIDTool", **kwargs) :
    the_name = makeName( name, kwargs)

    from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
    return InDet__PixelToTPIDTool(name = the_name, **kwargs)

@makePublicTool
def getInDetRecTestBLayerTool(name='InDetRecTestBLayerTool', **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags    import DetFlags
    if not DetFlags.haveRIO.pixel_on() :
        return None

    if 'Extrapolator' not in kwargs :
        kwargs = setDefaults( kwargs, Extrapolator     = getInDetExtrapolator())

    if 'PixelSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelSummaryTool = getInDetPixelConditionsSummaryTool())

    from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
    return InDet__InDetTestBLayerTool(name=the_name, **kwargs)

@makePublicTool
def getInDetTRTStrawStatusSummaryTool(name = "InDetTRT_StrawStatusSummaryTool", **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.GlobalFlags import globalflags
    kwargs = setDefaults( kwargs, isGEANT4 = (globalflags.DataSource == 'geant4'))
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
    return TRT_StrawStatusSummaryTool(name = the_name, **kwargs )

@makePublicTool
def getInDetTRTCalDbTool(name = "InDetTRT_CalDbTool", **kwargs) :
    the_name = makeName( name, kwargs)
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
    return TRT_CalDbTool(name = the_name, **kwargs)

@makePublicTool
def getInDetTRT_LocalOccupancy(name ="InDet_TRT_LocalOccupancy", **kwargs) :
    the_name = makeName( name, kwargs)
    if 'TRTCalDbTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRTCalDbTool = getInDetTRTCalDbTool() )

    if 'TRTStrawStatusSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRTStrawStatusSummaryTool = getInDetTRTStrawStatusSummaryTool() )

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
    return InDet__TRT_LocalOccupancy(name=the_name, **setDefaults( kwargs, isTrigger = False) )

@makePublicTool
def getInDetTRT_dEdxTool(name = "InDetTRT_dEdxTool", **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags
    if not DetFlags.haveRIO.TRT_on() or InDetFlags.doSLHC() or InDetFlags.doHighPileup() \
            or  InDetFlags.useExistingTracksAsInput(): # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
        return None

    from AthenaCommon.GlobalFlags import globalflags
    kwargs = setDefaults( kwargs, TRT_dEdx_isData = (globalflags.DataSource == 'data'))

    if 'TRT_LocalOccupancyTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_LocalOccupancyTool = getInDetTRT_LocalOccupancy())

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import TRT_ToT_dEdx
    return TRT_ToT_dEdx(name = the_name, **kwargs)

@makePublicTool
def getInDetTRT_ElectronPidTool(name = "InDetTRT_ElectronPidTool", **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags
    if not DetFlags.haveRIO.TRT_on() or  InDetFlags.doSLHC() or  InDetFlags.doHighPileup() \
            or  InDetFlags.useExistingTracksAsInput(): # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
        return None

    if 'TRTStrawSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRTStrawSummaryTool = getInDetTRTStrawStatusSummaryTool())

    if 'TRT_LocalOccupancyTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_LocalOccupancyTool = getInDetTRT_LocalOccupancy())

    if 'TRT_ToT_dEdx_Tool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_ToT_dEdx_Tool = getInDetTRT_dEdxTool())

    from AthenaCommon.GlobalFlags import globalflags
    kwargs = setDefaults( kwargs, isData = (globalflags.DataSource == 'data'))

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_ElectronPidToolRun2
    return InDet__TRT_ElectronPidToolRun2(name = the_name, **kwargs)

@makePublicTool
def getInDetSummaryHelper(name='InDetSummaryHelper',**kwargs) :
    the_name = makeName( name, kwargs)
    isHLT=kwargs.pop("isHLT",False)
    # Configrable version of loading the InDetTrackSummaryHelperTool
    #
    if 'AssoTool' not in kwargs :
        kwargs = setDefaults( kwargs, AssoTool = getInDetPrdAssociationTool_setup()   if not isHLT else   getInDetTrigPrdAssociationTool())
    if 'HoleSearch' not in kwargs :
        kwargs = setDefaults( kwargs, HoleSearch = getInDetHoleSearchTool()) # @TODO trigger specific hole search tool ?

    from AthenaCommon.DetFlags    import DetFlags
    if not DetFlags.haveRIO.TRT_on() :
        kwargs = setDefaults( kwargs, TRTStrawSummarySvc = "")

    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags
    kwargs = setDefaults( kwargs,
                          PixelToTPIDTool = None,         # we don't want to use those tools during pattern
                          TestBLayerTool  = None,         # we don't want to use those tools during pattern
                          # PixelToTPIDTool = InDetPixelToTPIDTool,
                          # TestBLayerTool  = InDetRecTestBLayerTool,
                          RunningTIDE_Ambi = InDetFlags.doTIDE_Ambi(),
                          DoSharedHits    = False,
                          usePixel        = DetFlags.haveRIO.pixel_on(),
                          useSCT          = DetFlags.haveRIO.SCT_on(),
                          useTRT          = DetFlags.haveRIO.TRT_on())

    from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
    return InDet__InDetTrackSummaryHelperTool(name = the_name,**kwargs)

def getInDetSummaryHelperNoHoleSearch(name='InDetSummaryHelperNoHoleSearch',**kwargs) :
    if 'HoleSearch' not in kwargs :
        kwargs = setDefaults( kwargs, HoleSearch = None)
    return getInDetSummaryHelper(name,**kwargs)


def getInDetSummaryHelperSharedHits(name='InDetSummaryHelperSharedHits',**kwargs) :

    if 'PixelToTPIDTool' not in kwargs :
        kwargs = setDefaults( kwargs,          PixelToTPIDTool = getInDetPixelToTPIDTool())

    if 'TestBLayerTool' not in kwargs :
        kwargs = setDefaults( kwargs,          TestBLayerTool  = getInDetRecTestBLayerTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,     DoSharedHits    = InDetFlags.doSharedHits())

    from AthenaCommon.DetFlags    import DetFlags
    if DetFlags.haveRIO.TRT_on() :
        from InDetRecExample.InDetJobProperties import InDetFlags
        kwargs = setDefaults( kwargs, DoSharedHitsTRT = InDetFlags.doSharedHits())

    return getInDetSummaryHelper(name, **kwargs)


@makePublicTool
def getInDetTrackSummaryTool(name='InDetTrackSummaryTool',**kwargs) :
    # makeName will remove the namePrefix in suffix from kwargs, so copyArgs has to be first
    hlt_args = copyArgs(kwargs,['isHLT','namePrefix'])
    id_helper_args = copyArgs(kwargs,['ClusterSplitProbabilityName','RenounceInputHandles','namePrefix','nameSuffix']) if 'ClusterSplitProbabilityName' in kwargs else {}
    kwargs.pop('ClusterSplitProbabilityName',None)
    kwargs.pop('RenounceInputHandles',None)
    kwargs.pop('isHLT',None)
    the_name = makeName( name, kwargs)
    do_holes=kwargs.get("doHolesInDet",True)
    if do_holes :
        id_helper_args.update(hlt_args)
    if 'InDetSummaryHelperTool' not in kwargs :
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = getInDetSummaryHelper(**id_helper_args) if do_holes else getInDetSummaryHelperNoHoleSearch(**id_helper_args))

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs = setDefaults(kwargs,
                         doSharedHits           = False,
                         doHolesInDet           = do_holes,
                         TRT_ElectronPidTool    = None,         # we don't want to use those tools during pattern
                         TRT_ToT_dEdxTool       = None,         # dito
                         PixelToTPIDTool        = None)         # we don't want to use those tools during pattern
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    return Trk__TrackSummaryTool(name = the_name, **kwargs)

def getInDetTrackSummaryToolNoHoleSearch(name='InDetTrackSummaryToolNoHoleSearch',**kwargs) :
    return getInDetTrackSummaryTool(name, **setDefaults(kwargs, doHolesInDet           = False))

def getInDetTrackSummaryToolSharedHits(name='InDetTrackSummaryToolSharedHits',**kwargs) :

    if 'InDetSummaryHelperTool' not in kwargs :
        copy_args=['ClusterSplitProbabilityName','RenounceInputHandles','namePrefix','nameSuffix']
        do_holes=kwargs.get("doHolesInDet",True)
        if do_holes :
            copy_args += ['isHLT']
        id_helper_args = copyArgs(kwargs,copy_args) if 'ClusterSplitProbabilityName' in kwargs else {}
        kwargs.pop('ClusterSplitProbabilityName',None)
        kwargs.pop('RenounceInputHandles',None)
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = getInDetSummaryHelperSharedHits(**id_helper_args))

    if 'TRT_ElectronPidTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_ElectronPidTool    = getInDetTRT_ElectronPidTool())

    if 'TRT_ToT_dEdxTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_ToT_dEdxTool       = getInDetTRT_dEdxTool())

    if 'PixelToTPIDTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelToTPIDTool        = getInDetPixelToTPIDTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults(kwargs,
                         doSharedHits           = InDetFlags.doSharedHits(),
                         minTRThitsForTRTdEdx   = 1)    # default is 1

    return getInDetTrackSummaryTool( name, **kwargs)

def getInDetTrackSummaryToolTRTTracks(name='InDetTrackSummaryToolTRTTracks',**kwargs) :
    # @TODO should switch off PixelToTPID, shared hits (setDefaults(kwargs,doSharedHits=False))
    return getInDetTrackSummaryToolSharedHits(name, **setDefaults(kwargs,doSharedHits=True))

def getInDetTrigTrackSummaryTool(name='InDetTrackSummaryTool',**kwargs) :
    return getInDetTrackSummaryTool(name,**setDefaults(kwargs,
                                                       namePrefix = "InDetTrig",
                                                       isHLT      = True))


@makePublicTool
def getInDetTRT_ExtensionToolCosmics(name='InDetTRT_ExtensionToolCosmics',**kwargs) :
    the_name = makeName( name, kwargs)

    if 'Propagator' not in kwargs :
        kwargs=setDefaults(kwargs, Propagator            = getInDetPropagator())

    if 'Extrapolator' not in kwargs :
        kwargs=setDefaults(kwargs, Extrapolator          = getInDetExtrapolator())

    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs=setDefaults(kwargs, RIOonTrackToolYesDr   = getInDetTRT_DriftCircleOnTrackUniversalToolCosmics())

    if 'RIOonTrackToolNoDr' not in kwargs :
        kwargs=setDefaults(kwargs, RIOonTrackToolNoDr    = getInDetTRT_DriftCircleOnTrackNoDriftTimeTool())

    from InDetRecExample.InDetKeys import InDetKeys
    kwargs = setDefaults(kwargs,
                         TRT_ClustersContainer = InDetKeys.TRT_DriftCircles(),
                         SearchNeighbour       = False,  # needs debugging!!!
                         RoadWidth             = 10.)

    from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionToolCosmics
    return InDet__TRT_TrackExtensionToolCosmics(name                  = the_name, **kwargs)

def getInDetTRT_ExtensionToolPhase(name='InDetTRT_ExtensionToolPhase', **kwargs) :
    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs=setDefaults(kwargs, RIOonTrackToolYesDr   = getInDetTRT_DriftCircleOnTrackUniversalTool())

    from InDetRecExample.InDetKeys import InDetKeys
    return getInDetTRT_ExtensionToolCosmics( name,
                                             **setDefaults(kwargs,
                                                           TRT_ClustersContainer = InDetKeys.TRT_DriftCirclesUncalibrated(),
                                                           RoadWidth             = 20.))


@makePublicTool
def getInDetTRTDriftCircleCutForPatternReco(name='InDetTRTDriftCircleCutForPatternReco', TrackingCuts=None, **kwargs) :
    the_name = makeName( name, kwargs)

    InDetNewTrackingCuts = TrackingCuts if TrackingCuts is not None  else getInDetNewTrackingCuts()
    from AthenaCommon.DetFlags import DetFlags
    kwargs=setDefaults(kwargs,
                       MinOffsetDCs           = 5,
                       UseNewParameterization = InDetNewTrackingCuts.useNewParameterizationTRT(),  # Use new parameterization only for high lumi
                       UseActiveFractionSvc   = DetFlags.haveRIO.TRT_on())

    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetTrtDriftCircleCutTool
    return InDet__InDetTrtDriftCircleCutTool(the_name, **kwargs)

@makePublicTool
def getInDetTRT_RoadMaker(name='InDetTRT_RoadMaker',**kwargs) :
    the_name = makeName( name, kwargs)
    kwargs=setDefaults(kwargs,
                       RoadWidth             = 20.,
                       PropagatorTool        = getInDetPatternPropagator())
    from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadMaker_xk
    return InDet__TRT_DetElementsRoadMaker_xk(the_name, **kwargs)


@makePublicTool
def getInDetTRT_TrackExtensionTool_xk(name='InDetTRT_ExtensionTool', TrackingCuts=None, **kwargs) :
    the_name = makeName( name, kwargs)

    InDetNewTrackingCuts = TrackingCuts if TrackingCuts is not None else getInDetNewTrackingCuts()
    if 'PropagatorTool' not in kwargs :
        kwargs=setDefaults(kwargs, PropagatorTool      = getInDetPatternPropagator())

    if 'UpdatorTool' not in kwargs :
        kwargs=setDefaults(kwargs, UpdatorTool         = getInDetPatternUpdator())

    if 'DriftCircleCutTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           DriftCircleCutTool  = getInDetTRTDriftCircleCutForPatternReco(TrackingCuts=InDetNewTrackingCuts))

    if 'RIOonTrackToolYesDr' not in kwargs :
        kwargs=setDefaults(kwargs, RIOonTrackToolYesDr = getInDetTRT_DriftCircleOnTrackTool())

    if 'RoadTool' not in kwargs :
        kwargs=setDefaults(kwargs, RoadTool            = getInDetTRT_RoadMaker())

    from InDetRecExample.InDetJobProperties import InDetFlags
    from InDetRecExample.InDetKeys import InDetKeys
    kwargs=setDefaults(kwargs,
                       TRT_ClustersContainer = InDetKeys.TRT_DriftCircles(),
                       TrtManagerLocation    = InDetKeys.TRT_Manager(),
                       UseDriftRadius        = not InDetFlags.noTRTTiming(),
                       MinNumberDriftCircles = InDetNewTrackingCuts.minTRTonTrk(),
                       ScaleHitUncertainty   = 2.,
                       RoadWidth             = 20.,
                       UseParameterization   = InDetNewTrackingCuts.useParameterizedTRTCuts(),
                       maxImpactParameter    = 500 if InDetFlags.doBeamHalo() or InDetFlags.doBeamGas() else 50) # single beam running, open cuts
    if (InDetNewTrackingCuts.RoISeededBackTracking()):
        kwargs=setDefaults(kwargs,
                           minTRTSegmentpT   = InDetNewTrackingCuts.minSecondaryPt()) #50% of the calo roi cluster Et requirement for RoISeededBackTracking

    from TRT_TrackExtensionTool_xk.TRT_TrackExtensionTool_xkConf import InDet__TRT_TrackExtensionTool_xk
    return InDet__TRT_TrackExtensionTool_xk(the_name, **kwargs)

@makePublicTool
def getInDetWeightCalculator(name='InDetWeightCalculator',**kwargs) :
    the_name = makeName( name, kwargs)
    from TrkDeterministicAnnealingFilter.TrkDeterministicAnnealingFilterConf import Trk__DAF_SimpleWeightCalculator
    return Trk__DAF_SimpleWeightCalculator( name = the_name, **kwargs)


@makePublicTool
def getInDetCompetingTRT_DC_Tool(name='InDetCompetingTRT_DC_Tool',**kwargs) :
    the_name = makeName( name, kwargs)

    if 'Extrapolator' not in kwargs :
        kwargs=setDefaults(kwargs, Extrapolator                          = getInDetExtrapolator())

    if 'ToolForWeightCalculation' not in kwargs :
        kwargs=setDefaults(kwargs, ToolForWeightCalculation              = getInDetWeightCalculator())

    if 'ToolForTRT_DriftCircleOnTrackCreation' not in kwargs :
        kwargs=setDefaults(kwargs, ToolForTRT_DriftCircleOnTrackCreation = getInDetTRT_DriftCircleOnTrackTool())

    from InDetCompetingRIOsOnTrackTool.InDetCompetingRIOsOnTrackToolConf import InDet__CompetingTRT_DriftCirclesOnTrackTool
    return InDet__CompetingTRT_DriftCirclesOnTrackTool( the_name, **kwargs)


@makePublicTool
def getInDetTRT_TrackExtensionTool_DAF(name='TRT_TrackExtensionTool_DAF',**kwargs) :
    the_name = makeName( name, kwargs)
    if 'CompetingDriftCircleTool' not in kwargs :
        kwargs=setDefaults(kwargs, CompetingDriftCircleTool    = getInDetCompetingTRT_DC_Tool())

    if 'PropagatorTool' not in kwargs :
        kwargs=setDefaults(kwargs, PropagatorTool              = getInDetPatternPropagator())

    if 'RoadTool' not in kwargs :
        kwargs=setDefaults(kwargs, RoadTool                    = getInDetTRT_RoadMaker())

    from InDetRecExample.InDetKeys import InDetKeys
    kwargs = setDefaults(kwargs, TRT_DriftCircleContainer = InDetKeys.TRT_DriftCircles())

    from TRT_TrackExtensionTool_DAF.TRT_TrackExtensionTool_DAFConf import InDet__TRT_TrackExtensionTool_DAF
    return InDet__TRT_TrackExtensionTool_DAF(the_name,**kwargs)


def getInDetTRT_ExtensionTool(TrackingCuts=None, **kwargs) :
    # @TODO set all names to InDetTRT_ExtensionTool ?
    from InDetRecExample.InDetJobProperties import InDetFlags
    if (InDetFlags.trtExtensionType() == 'xk') or (not InDetFlags.doNewTracking()) :
        if InDetFlags.doCosmics():
            return getInDetTRT_ExtensionToolCosmics(**kwargs)
        else :
            return getInDetTRT_TrackExtensionTool_xk(TrackingCuts=TrackingCuts, **kwargs)
    elif InDetFlags.trtExtensionType() == 'DAF' :
        return getInDetTRT_TrackExtensionTool_DAF('InDetTRT_ExtensionTool',**kwargs)

def getTRT_DetElementsRoadCondAlg(**kwargs):
    the_name=kwargs.pop("name","InDet__TRT_DetElementsRoadCondAlg_xk")
    from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadCondAlg_xk
    return InDet__TRT_DetElementsRoadCondAlg_xk(the_name, **kwargs)

def getInDetROIInfoVecCondAlg(name='InDetROIInfoVecCondAlg',**kwargs) :
    the_name = makeName(name, kwargs)
    from InDetRecExample.InDetKeys import InDetKeys
    kwargs=setDefaults(kwargs,
                       InputEmClusterContainerName = InDetKeys.CaloClusterROIContainer(),
                       WriteKey                    = kwargs.get("namePrefix","")+"ROIInfoVec"+kwargs.get("nameSuffix",""),
                       minPtEM                     = 5000.  # in MeV
                       )
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import ROIInfoVecAlg
    return ROIInfoVecAlg(the_name,**kwargs)

@makePublicTool
def getInDetAmbiScoringToolBase(name='InDetAmbiScoringTool', **kwargs) :
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    the_name=makeName(name,kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags
    have_calo_rois = InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        alg=createAndAddEventAlg(getInDetROIInfoVecCondAlg,"InDetROIInfoVecCondAlg")
        kwargs=setDefaults(kwargs, CaloROIInfoName = alg.WriteKey )
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    return InDet__InDetAmbiScoringTool(the_name,
                                       **setDefaults(kwargs,
                                                     Extrapolator            = getInDetExtrapolator(),
                                                     SummaryTool             = getInDetTrackSummaryTool(),
                                                     DriftCircleCutTool      = getInDetTRTDriftCircleCutForPatternReco(),
                                                     useAmbigFcn             = True,  # this is NewTracking
                                                     useTRT_AmbigFcn         = False,
                                                     maxZImp                 = NewTrackingCuts.maxZImpact(),
                                                     maxEta                  = NewTrackingCuts.maxEta(),
                                                     usePixel                = NewTrackingCuts.usePixel(),
                                                     useSCT                  = NewTrackingCuts.useSCT(),
                                                     doEmCaloSeed            = have_calo_rois)
                                       )

def getInDetAmbiScoringTool(NewTrackingCuts, name='InDetAmbiScoringTool', **kwargs) :
    return getInDetAmbiScoringToolBase(name+NewTrackingCuts.extension(),
                                       **setDefaults( kwargs,
                                                      NewTrackingCuts         = NewTrackingCuts,
                                                      useAmbigFcn             = True,  # this is NewTracking
                                                      useTRT_AmbigFcn         = False,
                                                      minTRTonTrk             = 0,
                                                      minTRTPrecisionFraction = 0,
                                                      minPt                   = NewTrackingCuts.minPT(),
                                                      maxRPhiImp              = NewTrackingCuts.maxPrimaryImpact(),
                                                      minSiClusters           = NewTrackingCuts.minClusters(),
                                                      minPixel                = NewTrackingCuts.minPixel(),
                                                      maxSiHoles              = NewTrackingCuts.maxHoles(),
                                                      maxPixelHoles           = NewTrackingCuts.maxPixelHoles(),
                                                      maxSCTHoles             = NewTrackingCuts.maxSCTHoles(),
                                                      maxDoubleHoles          = NewTrackingCuts.maxDoubleHoles()))

@makePublicTool
def getInDetNNScoringToolBase(name='InDetNNScoringTool', **kwargs) :
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    the_name=makeName(name,kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags
    have_calo_rois = InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        alg=createAndAddEventAlg(getInDetROIInfoVecCondAlg,"InDetROIInfoVecCondAlg")
        kwargs=setDefaults(kwargs, CaloROIInfoName = alg.WriteKey )
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetNNScoringTool
    return InDet__InDetNNScoringTool(the_name,
                                       **setDefaults(kwargs,
                                                     nnCutConfig             = "dev/TrackingCP/LRTAmbiNetwork/20200727_225401/nn-config.json",
                                                     nnCutThreshold          = InDetFlags.nnCutLargeD0Threshold(),
                                                     Extrapolator            = getInDetExtrapolator(),
                                                     SummaryTool             = getInDetTrackSummaryTool(),
                                                     DriftCircleCutTool      = getInDetTRTDriftCircleCutForPatternReco(),
                                                     useAmbigFcn             = True,  # this is NewTracking
                                                     useTRT_AmbigFcn         = False,
                                                     maxZImp                 = NewTrackingCuts.maxZImpact(),
                                                     maxEta                  = NewTrackingCuts.maxEta(),
                                                     usePixel                = NewTrackingCuts.usePixel(),
                                                     useSCT                  = NewTrackingCuts.useSCT(),
                                                     doEmCaloSeed            = have_calo_rois)
                                       )

def getInDetNNScoringTool(NewTrackingCuts, name='InDetNNScoringTool', **kwargs) :
    return getInDetNNScoringToolBase(name+NewTrackingCuts.extension(),
                                       **setDefaults( kwargs,
                                                      NewTrackingCuts         = NewTrackingCuts,
                                                      useAmbigFcn             = True,  # this is NewTracking
                                                      useTRT_AmbigFcn         = False,
                                                      minTRTonTrk             = 0,
                                                      minTRTPrecisionFraction = 0,
                                                      minPt                   = NewTrackingCuts.minPT(),
                                                      maxRPhiImp              = NewTrackingCuts.maxPrimaryImpact(),
                                                      minSiClusters           = NewTrackingCuts.minClusters(),
                                                      minPixel                = NewTrackingCuts.minPixel(),
                                                      maxSiHoles              = NewTrackingCuts.maxHoles(),
                                                      maxPixelHoles           = NewTrackingCuts.maxPixelHoles(),
                                                      maxSCTHoles             = NewTrackingCuts.maxSCTHoles(),
                                                      maxDoubleHoles          = NewTrackingCuts.maxDoubleHoles()))

def getInDetTRT_SeededScoringTool(NewTrackingCuts, name='InDetTRT_SeededScoringTool',**kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    return getInDetAmbiScoringToolBase('InDetTRT_SeededScoringTool',
                                       **setDefaults(kwargs,
                                                     NewTrackingCuts         = NewTrackingCuts,
                                                     useAmbigFcn             = not InDetFlags.doNewTracking(), # full search => use NewT
                                                     useTRT_AmbigFcn         = InDetFlags.doNewTracking(),     # full search => use NewT
                                                     minTRTonTrk             = NewTrackingCuts.minSecondaryTRTonTrk(),
                                                     minTRTPrecisionFraction = NewTrackingCuts.minSecondaryTRTPrecFrac(),
                                                     minPt                   = NewTrackingCuts.minSecondaryPt(),
                                                     maxRPhiImp              = NewTrackingCuts.maxSecondaryImpact(),
                                                     minSiClusters           = NewTrackingCuts.minSecondaryClusters(),
                                                     maxSiHoles              = NewTrackingCuts.maxSecondaryHoles(),
                                                     maxPixelHoles           = NewTrackingCuts.maxSecondaryPixelHoles(),
                                                     maxSCTHoles             = NewTrackingCuts.maxSecondarySCTHoles(),
                                                     maxDoubleHoles          = NewTrackingCuts.maxSecondaryDoubleHoles()))


def getInDetExtenScoringTool(NewTrackingCuts,name='InDetExtenScoringTool', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.trackFitterType() in ['KalmanFitter', 'KalmanDNAFitter', 'ReferenceKalmanFitter']:
        kwargs=setDefaults(kwargs, minTRTPrecisionFraction = 0.2)
    return getInDetAmbiScoringTool(NewTrackingCuts,
                                   name,
                                   **setDefaults(kwargs,
                                                 minTRTonTrk             = NewTrackingCuts.minTRTonTrk(),
                                                 minTRTPrecisionFraction = NewTrackingCuts.minTRTPrecFrac()))

@makePublicTool
def getInDetCosmicsScoringToolBase(name='InDetCosmicsScoringTool', **kwargs) :
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    the_name=makeName(name,kwargs)
    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetCosmicScoringTool
    return InDet__InDetCosmicScoringTool(the_name,
                                         **setDefaults(kwargs,
                                                       nWeightedClustersMin = NewTrackingCuts.nWeightedClustersMin(),
                                                       minTRTHits           = 0,
                                                       SummaryTool          = getInDetTrackSummaryTool()))

def getInDetCosmicsScoringTool(NewTrackingCuts,name='InDetCosmicsScoringTool', **kwargs) :
    return getInDetCosmicsScoringToolBase(name+NewTrackingCuts.extension(),
                                          **setDefaults(kwargs, NewTrackingCuts=NewTrackingCuts))

def getInDetCosmicExtenScoringTool(NewTrackingCuts, name='InDetCosmicExtenScoringTool',**kwargs) :
    return getInDetCosmicsScoringToolBase('InDetCosmicExtenScoringTool',
                                          **setDefaults(kwargs,
                                                        NewTrackingCuts      = NewTrackingCuts,
                                                        nWeightedClustersMin = 0,
                                                        minTRTHits           = NewTrackingCuts.minTRTonTrk()))

def getInDetCosmicScoringTool_TRT(NewTrackingCuts, name='InDetCosmicExtenScoringTool',**kwargs) :
    return getInDetCosmicExtenScoringTool(NewTrackingCuts,
                                          'InDetCosmicScoringTool_TRT',
                                          **setDefaults(kwargs,
                                                        minTRTHits  = NewTrackingCuts.minSecondaryTRTonTrk(),
                                                        SummaryTool = getInDetTrackSummaryToolNoHoleSearch()))

def getSolenoidParametrizationCondAlg(name='SolenoidParametrizationCondAlg',**kwargs) :
    the_name=makeName(name,kwargs)
    # @TODO require that the magnetid field is setup.
    from TrkExSolenoidalIntersector.TrkExSolenoidalIntersectorConf import Trk__SolenoidParametrizationCondAlg
    return Trk__SolenoidParametrizationCondAlg(the_name, **setDefaults(kwargs,
                                                                       AtlasFieldCacheCondObj = 'fieldCondObj',
                                                                       WriteKey               = 'SolenoidParametrization' ))

def getSolenoidalIntersector(name="SolenoidalIntersector", **kwargs) :
    the_name=makeName(name,kwargs)
    createAndAddCondAlg(getSolenoidParametrizationCondAlg, "SolenoidParametrizationCondAlg")
    from TrkExSolenoidalIntersector.TrkExSolenoidalIntersectorConf import Trk__SolenoidalIntersector
    return Trk__SolenoidalIntersector(the_name, **setDefaults(kwargs, SolenoidParameterizationKey = 'SolenoidParametrization'))

def hasSplitProb(key) :
    # @TODO find better solution,
    import re
    pat=re.compile('.*Dense.*')
    from AthenaCommon.AppMgr import ToolSvc
    for a_tool in ToolSvc.getChildren() :
        if pat.match( a_tool.getFullName() ) != None :
            print ('DEBUG split prob probabily set by %s' % a_tool.getFullName() )
            return True

    from RecExConfig.AutoConfiguration import IsInInputFile
    if IsInInputFile('Trk::ClusterSplitProbabilityContainer',key) :
        print ('DEBUG split prob  %s in inputfile ' % key )
        return True

    print ('DEBUG split prob is not set.' )
    return False

def combinedClusterSplitProbName() :
  # precisely mimics the configuration in InDetRec_jobOptions
  # chaings in InDetRec_jobOptions to the ClusterSplitProbContainer also have to be implemented here
  # @TODO find a better way to provide the final name of ClusterSplitProbContainer used for the combined InDetTrackParticles
  from AthenaCommon.BeamFlags import jobproperties
  from InDetRecExample.InDetJobProperties import InDetFlags
  CombinedInDetClusterSplitProbContainer = ''
  ClusterSplitProbContainer=''
  if InDetFlags.Enabled():
    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    # --- ensure that Calo clustering is running if we run in calo seeded mode 
    if ('InDetNewTrackingCuts' not in dir()):
      if InDetFlags.doDBMstandalone():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
      elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
      elif InDetFlags.doVtxBeamSpot():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxBeamSpot")
      elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
      elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
      elif InDetFlags.doSLHC():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("SLHC")
      elif InDetFlags.doIBL():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("IBL")
      elif InDetFlags.doHighPileup():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HighPileup")
      elif InDetFlags.doMinBias():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("MinBias")
      elif InDetFlags.doDVRetracking():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("R3LargeD0")
      else:
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
    if InDetFlags.doTrackSegmentsPixel():
      if ('InDetNewTrackingCutsPixel' not in dir()):
        InDetNewTrackingCutsPixel = ConfiguredNewTrackingCuts("Pixel")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsPixel.extension()
      # @TODO CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doTrackSegmentsSCT():
      if ('InDetNewTrackingCutsSCT' not in dir()):
        InDetNewTrackingCutsSCT = ConfiguredNewTrackingCuts("SCT")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsSCT.extension()
      # @TODO CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doTRTPhaseCalculation() and not jobproperties.Beam.beamType()=="collisions" and InDetFlags.doNewTracking():
        ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCuts.extension()
        CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doNewTracking() and (not InDetFlags.doTRTPhaseCalculation() or jobproperties.Beam.beamType()=="collisions"):
        ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCuts.extension()
        CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doBackTracking():
      ClusterSplitProbContainer = 'InDetTRT_SeededAmbiguityProcessorSplitProb'+InDetNewTrackingCuts.extension()
      CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    ClusterSplitProbContainerLargeD0=''
    if InDetFlags.doLargeD0() or InDetFlags.doR3LargeD0() or InDetFlags.doLowPtLargeD0():
      if ('InDetNewTrackingCutsLargeD0' not in dir()):
        if InDetFlags.doLowPtLargeD0():
          InDetNewTrackingCutsLargeD0 = ConfiguredNewTrackingCuts("LowPtLargeD0")
        elif InDetFlags.doR3LargeD0():
          InDetNewTrackingCutsLargeD0 = ConfiguredNewTrackingCuts("R3LargeD0")
        else:
          InDetNewTrackingCutsLargeD0 = ConfiguredNewTrackingCuts("LargeD0")
      ClusterSplitProbContainerLargeD0 = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsLargeD0.extension()
      if not InDetFlags.storeSeparateLargeD0Container():
        ClusterSplitProbContainer = ClusterSplitProbContainerLargeD0
        CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainerLargeD0
    if InDetFlags.doLowPt():
      if ('InDetNewTrackingCutsLowPt' not in dir()):
        InDetNewTrackingCutsLowPt = ConfiguredNewTrackingCuts("LowPt")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsLowPt.extension()
      CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doVeryLowPt():
      if ('InDetNewTrackingCutsVeryLowPt' not in dir()):
        InDetNewTrackingCutsVeryLowPt = ConfiguredNewTrackingCuts("VeryLowPt")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsVeryLowPt.extension()
      CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doTRTStandalone():
      CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doForwardTracks() and InDetFlags.doSLHC():
      if InDetFlags.doSLHCVeryForward():
       if ('InDetNewTrackingCutsForwardTracks' not in dir()):
         InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("VeryForwardSLHCTracks")
         ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsForwardTracks.extension()
         CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
      else:
       if ('InDetNewTrackingCutsForwardTracks' not in dir()):
        InDetNewTrackingCutsForwardTracks = ConfiguredNewTrackingCuts("ForwardSLHCTracks")
        ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsForwardTracks.extension()
        CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doSLHCConversionFinding() and InDetFlags.doSLHC():
      if ('InDetNewTrackingCutsSLHCConversionFinding' not in dir()):
        InDetNewTrackingCutsSLHCConversionFinding = ConfiguredNewTrackingCuts("SLHCConversionFinding")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsSLHCConversionFinding.extension()
      CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doBeamGas():
      if ('InDetNewTrackingCutsBeamGas' not in dir()):
        InDetNewTrackingCutsBeamGas = ConfiguredNewTrackingCuts("BeamGas")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsBeamGas.extension()
      if jobproperties.Beam.beamType() == "singlebeam":
        CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer
    if InDetFlags.doCosmics() and InDetFlags.doNewTracking() : # @TODO should also consider: and len(InputCombinedInDetTracks) > 1:
      CombinedInDetClusterSplitProbContainer = ''  # @TODO really no split prob container ?
    if InDetFlags.doNewTrackingPattern() or InDetFlags.doBeamHalo():
      if InDetFlags.useExistingTracksAsInput():
          pass # CombinedInDetClusterSplitProbContainer = ClusterSplitProbContainer # @TODO handle cluster splitting probability ?
      if InDetFlags.doDBMstandalone():
          CombinedInDetClusterSplitProbContainer=''
  return CombinedInDetClusterSplitProbContainer if hasSplitProb(CombinedInDetClusterSplitProbContainer) else ''

def pixelClusterSplitProbName() :
    ClusterSplitProbContainer=combinedClusterSplitProbName()
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doTrackSegmentsDisappearing():
      if ('InDetNewTrackingCutsDisappearing' not in dir()):
        print ("InDetRec_jobOptions: InDetNewTrackingCutsDisappearing not set before - import them now")
        from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
        InDetNewTrackingCutsDisappearing = ConfiguredNewTrackingCuts("Disappearing")
      ClusterSplitProbContainer = 'InDetAmbiguityProcessorSplitProb'+InDetNewTrackingCutsDisappearing.extension()
    return ClusterSplitProbContainer if hasSplitProb(ClusterSplitProbContainer) else ''

