# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.Logging import logging
log = logging.getLogger('TrackingCommon')

use_tracking_geometry_cond_alg = True

def createAndAddCondAlg(creator, the_name, **kwargs) :
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AlgSequence import AthSequencer
    cond_seq=AthSequencer("AthCondSeq")
    for seq in [AlgSequence(),cond_seq] :
        log.debug('createAndAddCondAlg match ?  %s == %s ? %s ', dir(seq), the_name, hasattr(seq,the_name) )
        if hasattr(seq,the_name) :
            if seq.getName() != "AthCondSeq" :
                raise Exception('Algorithm already in a sequnece but not the conditions seqence')
            return getattr(seq,the_name)
    alg = creator(**kwargs)
    cond_seq += alg
    return alg

def getCondAlg(the_name) :
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AlgSequence import AthSequencer
    cond_seq=AthSequencer("AthCondSeq")
    for seq in [AlgSequence(),cond_seq] :
        log.debug('createAndAddCondAlg match ?  %s == %s ? %s ', dir(seq), the_name, hasattr(seq,the_name) )
        if hasattr(seq,the_name) :
            if seq.getName() != "AthCondSeq" :
                raise Exception('Algorithm already in a sequnece but not the conditions seqence')
            return getattr(seq,the_name)
    return None

def addCondAlg(cond_alg) :
    if cond_alg is not None :
        from AthenaCommon.AlgSequence import AthSequencer
        cond_seq=AthSequencer("AthCondSeq")
        cond_seq += cond_alg

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
                log.debug ('Add to ToolSvc %s' % (tool.name()) )
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
    if InDetFlags.doBLS():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("BLS")
    elif InDetFlags.doDBMstandalone():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
    elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
    elif InDetFlags.doVtxBeamSpot():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxBeamSpot")
    elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
    elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
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

    if do_runI and not useTTrainedNetworks:
      log.debug("useNNTTrainedNetworks must be True for Run I. Contact CTIDE for questions.")
      useTTrainedNetworks = True
    
    if useTTrainedNetworks :
      log.debug("Setting up TTrainedNetworks")
      createAndAddCondAlg( getPixelClusterNnCondAlg,         'PixelClusterNnCondAlg',          GetInputsInfo = do_runI)
      createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,'PixelClusterNnWithTrackCondAlg', GetInputsInfo = do_runI)
    else :
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
                                                      InDetFlags.pixelClusterSplittingType() == 'NeuralNet'),
                         NNIBLcorrection          = ( InDetFlags.doPixelClusterSplitting() and
                                                      InDetFlags.pixelClusterSplittingType() == 'NeuralNet'),
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
    strip_args=['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix']
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
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolPattern(**pix_cluster_on_track_args))
    return getInDetRotCreator(name=name, **kwargs)


def getInDetRotCreatorDBM(name='InDetRotCreatorDBM', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
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
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolDigital(**pix_cluster_on_track_args))
    return getInDetRotCreator(name=name, **kwargs)

# @TODO rename to InDetBroadRotCreator
def getInDetBroadRotCreator(name='InDetBroadInDetRotCreator', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        pix_cluster_on_track_args = copyArgs(kwargs,['SplitClusterMapExtension','ClusterSplitProbabilityName','nameSuffix'])
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


def getTrackingGeometryCondAlg(name="AtlasTrackingGeometryCondAlg",**kwargs) :
    the_name = makeName( name, kwargs )
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import ConfiguredTrackingGeometryCondAlg
    alg = ConfiguredTrackingGeometryCondAlg(the_name)
    return alg


@makePublicTool
def getAtlasNavigator(name='AtlasNavigator', **kwargs) :
    the_name = makeName( name, kwargs)
    if use_tracking_geometry_cond_alg and 'TrackingGeometryKey' not in kwargs :
        cond_alg = createAndAddCondAlg(getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
        kwargs = setDefaults(kwargs, TrackingGeometryKey=cond_alg.TrackingGeometryWriteKey)
    elif 'TrackingGeometrySvc' not in kwargs :
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        kwargs = setDefaults(kwargs, TrackingGeometrySvc = AtlasTrackingGeometrySvc)

    from TrkExTools.TrkExToolsConf import Trk__Navigator
    return Trk__Navigator(name=the_name,**kwargs)

def getInDetNavigator(name='InDetNavigator', **kwargs) :
    return getAtlasNavigator(name,**kwargs)

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
def getTrkMaterialProviderTool(name='TrkMaterialProviderTool',**kwargs) :
    the_name = makeName(name,kwargs)
    if use_tracking_geometry_cond_alg and 'TrackingGeometryReadKey' not in kwargs :
        cond_alg = createAndAddCondAlg(getTrackingGeometryCondAlg, "AtlasTrackingGeometryCondAlg", name="AtlasTrackingGeometryCondAlg")
        kwargs = setDefaults(kwargs, TrackingGeometryReadKey=cond_alg.TrackingGeometryWriteKey)
    else :
        from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc
        kwargs = setDefaults(kwargs, TrackingGeometrySvc = AtlasTrackingGeometrySvc)

    from TrkMaterialProvider.TrkMaterialProviderConf import Trk__TrkMaterialProviderTool
    return Trk__TrkMaterialProviderTool( name = the_name, **kwargs)

def getInDetTrkMaterialProviderTool(name='InDetTrkMaterialProviderTool',**kwargs) :
    return getTrkMaterialProviderTool(name,**kwargs)

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

def getInDetPixelConditionsSummaryTool(name = "PixelConditionsSummaryTool",**kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    from RecExConfig.AutoConfiguration import IsInInputFile
    has_bytestream_errors= globalflags.DataSource=='data' and (IsInInputFile('IDCInDetBSErrContainer','PixelByteStreamErrs')
                                                               or globalflags.InputFormat() == 'bytestream' )

    pixelConditionsSummaryToolSetup = PixelConditionsSummaryTool(the_name,
                                                                 **setDefaults(kwargs,
                                                                               UseByteStreamFEI4=has_bytestream_errors,
                                                                               UseByteStreamFEI3=has_bytestream_errors) )
    if InDetFlags.usePixelDCS():
        pixelConditionsSummaryToolSetup.IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ]
        pixelConditionsSummaryToolSetup.IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]

    return pixelConditionsSummaryToolSetup

def getInDetPixelConditionsSummaryToolActiveOnly(name = "PixelConditionsSummaryToolActiveOnly",**kwargs) :
    return getInDetPixelConditionsSummaryTool(name, **setDefaults(kwargs, ActiveOnly = True))


def getPixelConditionsSummaryToolNoByteStreamErrorsActiveOnly(name = "PixelConditionsSummaryToolNoByteStreamErrorsActiveOnly", **kwargs) :
    return getInDetPixelConditionsSummaryTool(name, **setDefaults(kwargs,
                                                                  ActiveOnly        = True,
                                                                  UseByteStreamFEI4 = False,
                                                                  UseByteStreamFEI3 = False,
                                                                  UseByteStreamRD53 = False))

def getPixelActiveDetectorElementStatusTool(name = "PixelActiveDetectorElementStatusTool",**kwargs) :
    from PixelConditionsTools.PixelConditionsToolsConf import PixelActiveDetectorElementStatusTool
    return PixelActiveDetectorElementStatusTool(name, **setDefaults(kwargs,
                                                                    IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]) )

def getPixelByteStreamErrorDetectorElementStatusTool(name = "PixelByteStreamErrorDetectorElementStatusTool",**kwargs) :
    the_name = makeName( name, kwargs)
    from RecExConfig.AutoConfiguration import IsInInputFile
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    has_bytestream_errors= globalflags.DataSource=='data' \
                           and not (globalflags.isOverlay() and  overlayFlags.isDataOverlay()) \
                           and (IsInInputFile('IDCInDetBSErrContainer','PixelByteStreamErrs')
                                              or globalflags.InputFormat() == 'bytestream' )
    if has_bytestream_errors :
        from PixelConditionsTools.PixelConditionsToolsConf import PixelByteStreamErrorDetectorElementStatusTool
        return PixelByteStreamErrorDetectorElementStatusTool(the_name, **setDefaults(kwargs,
                                                                                     PixelByteStreamErrs = "PixelByteStreamErrs",
                                                                                     UseByteStreamFEI4   = True,
                                                                                     UseByteStreamFEI3   = True,
                                                                                     ActiveOnly          = False) )
    else :
        kwargs.pop("ActiveOnly",False)
        from PixelConditionsTools.PixelConditionsToolsConf import PixelDetectorElementStatusCloneTool
        return PixelDetectorElementStatusCloneTool(the_name,**kwargs)


def getPixelByteStreamErrorDetectorElementStatusToolActiveOnly(name = "PixelByteStreamErrorDetectorElementStatusToolActiveOnly",**kwargs) :
    return getPixelByteStreamErrorDetectorElementStatusTool(name, **setDefaults(kwargs, ActiveOnly = True) )

@makePublicTool
def getInDetTestPixelLayerTool(name = "InDetTestPixelLayerTool", **kwargs) :
    the_name = makeName( name, kwargs)
    if 'PixelSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelSummaryTool = getInDetPixelConditionsSummaryTool())

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,
                          CheckActiveAreas = InDetFlags.checkDeadElementsOnTrack(),
                          CheckDeadRegions = InDetFlags.checkDeadElementsOnTrack(),
                          CheckDisabledFEs = InDetFlags.checkDeadElementsOnTrack()
                          ,PixelDetElStatus   = "PixelDetectorElementStatus"
                          ,PixelReadoutManager = "PixelReadoutManager"
                           )  # @TODO Should use method here


    from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
    return InDet__InDetTestPixelLayerTool(name = the_name, **kwargs)

def getInDetTrigTestPixelLayerTool(name = "InDetTrigTestPixelLayerTool", **kwargs) :
    return getInDetTestPixelLayerTool(name, **setDefaults( kwargs, PixelDetElStatus   = "") )

@makePublicTool
def getInDetTestPixelLayerToolInner(name='InDetTestPixelLayerToolInner', **kwargs):
    kwargs = setDefaults(kwargs,
                         CheckActiveAreas = False,
                         CheckDeadRegions = False,
                         CheckDisabledFEs = False,
                         OuterRadius = 100.)  # To allow for extrapolation up to B-layer = next-to-innermost
    return getInDetTestPixelLayerTool(name, **kwargs)

@makePublicTool
def getInDetTrigTestPixelLayerToolInner(name='InDetTrigTestPixelLayerToolInner', **kwargs):
    kwargs = setDefaults(kwargs,
                         CheckActiveAreas = True,
                         PixelDetElStatus = "")

    if 'Extrapolator' not in kwargs :
        kwargs = setDefaults( kwargs, Extrapolator     = getInDetExtrapolator())

    return getInDetTestPixelLayerToolInner(name, **kwargs)

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

@makePublicTool
def getInDetTrackToVertexTool(name='TrackToVertex', **kwargs) :
    the_name = makeName( name, kwargs)
    if 'Extrapolator' not in kwargs :
        from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
        kwargs=setDefaults(kwargs,Extrapolator = AtlasExtrapolator())

    from TrackToVertex.TrackToVertexConf import Reco__TrackToVertex
    return Reco__TrackToVertex(the_name,**kwargs)

def getPixelDetectorElementStatusCondAlgActiveOnly(name = "PixelDetectorElementStatusCondAlgNoByteStreamErrorActiveOnly", **kwargs) :
    '''
    Condition alg to precompute the pixel detector element status.
    this algo does not consider the DCS status (and the byte stream errors which are event data)
    '''
    the_name = makeName(name, kwargs)
    cond_alg = getCondAlg(the_name)
    if cond_alg is not None :
        return cond_alg

    if 'ConditionsSummaryTool' not in kwargs :
       kwargs = setDefaults( kwargs, ConditionsSummaryTool   = getPixelConditionsSummaryToolNoByteStreamErrorsActiveOnly())
    kwargs = setDefaults( kwargs, WriteKey = "PixelDetectorElementStatusNoByteStreamActiveOnly")

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SiDetectorElementStatusCondAlg
    return InDet__SiDetectorElementStatusCondAlg(the_name, **kwargs)

def getPixelDetectorElementStatusCondAlg(name = "PixelDetectorElementStatusCondAlgNoByteStreamError", **kwargs) :
    '''
    Condition alg to precompute the create pixel detector element status which includes the DCS status
    this algo does not consider the byte stream errors which are event data
    '''
    the_name = makeName(name, kwargs)
    cond_alg = getCondAlg(the_name)
    if cond_alg is not None :
        return cond_alg

    addCondAlg( getPixelDetectorElementStatusCondAlgActiveOnly() )

    if 'ConditionsSummaryTool' not in kwargs :
       kwargs = setDefaults( kwargs, ConditionsSummaryTool = getPixelActiveDetectorElementStatusTool(PixelDetElStatusCondDataBaseKey="PixelDetectorElementStatusNoByteStreamActiveOnly"))
    kwargs = setDefaults( kwargs, WriteKey = "PixelDetectorElementStatusNoByteStream")

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SiDetectorElementStatusCondAlg
    return InDet__SiDetectorElementStatusCondAlg(the_name, **kwargs)

def getPixelDetectorElementStatusAlg(name = "PixelDetectorElementStatusAlg", **kwargs) :
    '''
    Event alg which extends the pixel detector element status conditions data by the bytestream errors.
    '''

    the_name = makeName(name, kwargs)
    active_only = kwargs.pop("ActiveOnly", False)
    if 'ConditionsSummaryTool' not in kwargs and not active_only :
        element_status_input=None
        from InDetRecExample.InDetJobProperties import InDetFlags
        if InDetFlags.usePixelDCS() :
            addCondAlg( getPixelDetectorElementStatusCondAlg() )
            element_status_input="PixelDetectorElementStatusNoByteStream"
        else :
            # without DCS PixelDetectorElementStatusNoByteStream and PixelDetectorElementStatusNoByteStreamActiveOnly
            # are identically
            addCondAlg( getPixelDetectorElementStatusCondAlgActiveOnly() )
            element_status_input="PixelDetectorElementStatusNoByteStreamActiveOnly"
        kwargs.setdefault("ConditionsSummaryTool",
                          getPixelByteStreamErrorDetectorElementStatusTool( PixelDetElStatusCondDataBaseKey=element_status_input) )

    elif 'ConditionsSummaryTool' not in kwargs and active_only :
       addCondAlg( getPixelDetectorElementStatusCondAlgActiveOnly() )
       kwargs.setdefault("ConditionsSummaryTool",
                         getPixelByteStreamErrorDetectorElementStatusToolActiveOnly( PixelDetElStatusCondDataBaseKey="PixelDetectorElementStatusNoByteStreamActiveOnly"))

    kwargs = setDefaults( kwargs, WriteKey = "PixelDetectorElementStatus")

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SiDetectorElementStatusAlg
    return InDet__SiDetectorElementStatusAlg(the_name, **kwargs)

def getPixelDetectorElementStatusAlgActiveOnly(name = "PixelDetectorElementStatusAlgActiveOnly", **kwargs) :
    '''
    Event alg which extends the pixel detector element status conditions data which does not consider the DCS status by the bytestream errors.
    This alg however does only consider errors concerning the module activity, not general errors.
    '''
    return getPixelDetectorElementStatusAlg( name       = name,
                                           WriteKey   = "PixelDetectorElementStatusActiveOnly",
                                           ActiveOnly = True)


# @TODO move configuration of InDetSCT_ConditionsSummaryTool to a function
def_InDetSCT_ConditionsSummaryToolWithoutFlagged=None
def getInDetSCT_ConditionsSummaryToolWithoutFlagged() :
    return def_InDetSCT_ConditionsSummaryToolWithoutFlagged

def_InDetSCT_ConditionsSummaryTool=None
def getInDetSCT_ConditionsSummaryTool() :
    return def_InDetSCT_ConditionsSummaryTool

def getInDetSCT_FlaggedConditionTool(name='InDetSCT_FlaggedConditionTool', **kwargs) :
    the_name = makeName(name, kwargs)
    kwargs = setDefaults(kwargs, SCT_FlaggedCondData = "SCT_FlaggedCondData" )

    from SCT_ConditionsTools.SCT_ConditionsToolsConf import  SCT_FlaggedConditionTool
    return SCT_FlaggedConditionTool(the_name, **kwargs)

def getInDetSCT_DetectorElementStatusCondDataTool(name="InDetSCT_DetectorElementStatusCondDataTool", **kwargs) :
    '''
    Helper method to create from an existing InDetSCT_ConditionsSummaryTool
    a new version without the flagged and bytestream error tools
    '''
    the_name = makeName(name, kwargs)
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    tool_setup = SCT_ConditionsSummaryToolSetup(the_name)
    tool_setup.setup()
    the_summary_tool = tool_setup.getTool()

    if "ConditionsTools" not in kwargs :
        import re
        pattern = re.compile(".*SCT_ByteStreamErrorsTool.*")
        condTools = []
        input_cond_tools = getInDetSCT_ConditionsSummaryToolWithoutFlagged().ConditionsTools
        for condToolHandle in input_cond_tools:
            condTool = condToolHandle
            if condTool not in condTools:
                if pattern.match(condTool.getFullName()) is None :
                    condTools.append(condTool)
        the_summary_tool.ConditionsTools = condTools
    else :
        the_summary_tool.ConditionsTools = kwargs.pop("ConditionsTools")
    return the_summary_tool

def getInDetSCT_DetectorElementStatusAddByteStreamErrorsTool(name ='InDetSCT_DetectorElementStatusAddByteStreamErrorsTool', **kwargs) :
    the_name = makeName(name, kwargs)
    from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
    from SCT_ConditionsTools.SCT_ConditionsToolsHelper import getSCT_ByteStreamErrorsTool

    if "ConditionsTools" not in kwargs :
        from RecExConfig.AutoConfiguration import IsInInputFile
        has_bytestream_errors= globalflags.DataSource=='data' \
                               and (IsInInputFile('IDCInDetBSErrContainer','SCT_ByteStreamErrs')
                                    or globalflags.InputFormat() == 'bytestream' )

        kwargs = setDefaults(kwargs, ConditionsTools = [getSCT_ByteStreamErrorsTool()] if has_bytestream_errors else [])

    kwargs = setDefaults(kwargs,
                         SCTDetEleCollKey               = "SCT_DetectorElementCollection")

    return SCT_ConditionsSummaryTool(the_name, **kwargs)

# to be used by the SCTDetectorElementStatusAlgW algorithm to add the SCT_FlaggedCondData to the status info without this data
def getInDetSCT_DetectorElementStatusAddFlaggedTool(name = 'InDetSCT_ConditionsSummaryToolAddFlagged', **kwargs) :
    the_name = makeName(name, kwargs)
    from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool

    if "ConditionsTools" not in kwargs :
        kwargs = setDefaults(kwargs, ConditionsTools = [getInDetSCT_FlaggedConditionTool()])

    # @RODO should make sure that conditions algorithm is configured
    kwargs = setDefaults(kwargs,
                         SCTDetEleCollKey               = "SCT_DetectorElementCollection",
                         SCTDetElStatusEventDataBaseKey = "SCTDetectorElementStatusWithoutFlagged")
    return SCT_ConditionsSummaryTool(the_name, **kwargs)

def getInDetSCT_ConditionsSummaryToolClone(name = 'InDetSCT_ConditionsSummaryToolClone', **kwargs) :
    the_name = makeName(name, kwargs)
    from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool

    if "ConditionsTools" not in kwargs :
        kwargs = setDefaults(kwargs, ConditionsTools = [])

    # @RODO should make sure that conditions algorithm is configured
    kwargs = setDefaults(kwargs,
                         SCTDetEleCollKey               = "SCT_DetectorElementCollection")
    return SCT_ConditionsSummaryTool(the_name, **kwargs)

# SCTDetectorElementStatusAlg which creates the status data to be used in the SCT_Clusterization
def getSCTDetectorElementStatusCondAlg(name="SCTDetectorElementStatusCondAlg",**kwargs) :
    the_name = makeName(name, kwargs)
    cond_alg = getCondAlg(the_name)
    if cond_alg is not None :
        return cond_alg

    if 'ConditionsSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, ConditionsSummaryTool   = getInDetSCT_DetectorElementStatusCondDataTool() )
    kwargs = setDefaults( kwargs, WriteKey = "SCTDetectorElementStatusCondData")

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SiDetectorElementStatusCondAlg
    return InDet__SiDetectorElementStatusCondAlg(the_name, **kwargs)

def getSCTDetectorElementStatusAlgWithoutFlagged(name="SCTDetectorElementStatusAlgWithoutFlagged",**kwargs) :
    '''
    Algorithm which just creates event data from conditions data.
    '''
    the_name = makeName(name, kwargs)
    addCondAlg( getSCTDetectorElementStatusCondAlg() )

    if 'ConditionsSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, ConditionsSummaryTool   = getInDetSCT_DetectorElementStatusAddByteStreamErrorsTool(
                                                                  SCTDetElStatusCondDataBaseKey  = "SCTDetectorElementStatusCondData",
                                                                  SCTDetElStatusEventDataBaseKey = ""))

    kwargs = setDefaults( kwargs,
                          WriteKey                      = "SCTDetectorElementStatusWithoutFlagged")

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SiDetectorElementStatusAlg
    return InDet__SiDetectorElementStatusAlg(the_name, **kwargs)

# SCTDetectorElementStatusAlg which creates the status data to be used everywhere but the SCT_Clusterization
def getSCTDetectorElementStatusAlg(name = "SCTDetectorElementStatusAlg", **kwargs) :
    the_name = makeName(name, kwargs)
    createAndAddEventAlg( getSCTDetectorElementStatusAlgWithoutFlagged, "SCTDetectorElementStatusAlgWithoutFlagged" )

    if 'ConditionsSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs,
                              ConditionsSummaryTool   = getInDetSCT_DetectorElementStatusAddFlaggedTool(
                                                          SCTDetElStatusCondDataBaseKey  = "",
                                                          SCTDetElStatusEventDataBaseKey = "SCTDetectorElementStatusWithoutFlagged"))

    kwargs = setDefaults( kwargs,
                          WriteKey                      = "SCTDetectorElementStatus")

    from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SiDetectorElementStatusAlg
    return InDet__SiDetectorElementStatusAlg(the_name, **kwargs)


@makePublicTool
def getInDetBoundaryCheckTool(name="InDetBoundarySearchTool", **kwargs):
    the_name = makeName(name, kwargs)
    from AthenaCommon.DetFlags import DetFlags

    if 'SctSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, SctSummaryTool   = getInDetSCT_ConditionsSummaryTool()  if DetFlags.haveRIO.SCT_on()   else None)

    if 'PixelLayerTool' not in kwargs :
        kwargs = setDefaults( kwargs, PixelLayerTool   = getInDetTestPixelLayerTool())

    kwargs = setDefaults(
        kwargs,
        UsePixel=DetFlags.haveRIO.pixel_on(),
        UseSCT=DetFlags.haveRIO.SCT_on(),
        SCTDetElStatus="SCTDetectorElementStatus"
    )

    from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
    return InDet__InDetBoundaryCheckTool(name=the_name, **kwargs)

@makePublicTool
def getInDetHoleSearchTool(name = 'InDetHoleSearchTool', **kwargs) :
    the_name = makeName( name, kwargs)
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
def getInDetTRTStrawStatusSummaryTool(name = "InDetTRT_StrawStatusSummaryTool", **kwargs) :
    the_name = makeName( name, kwargs)
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

    if 'isTrigger' not in kwargs :
        kwargs = setDefaults( kwargs, isTrigger = False )

    from TRT_ElectronPidTools.TRT_ElectronPidToolsConf import InDet__TRT_LocalOccupancy
    return InDet__TRT_LocalOccupancy(name = the_name, **kwargs)

@makePublicTool
def getInDetTRT_dEdxTool(name = "InDetTRT_dEdxTool", **kwargs) :
    the_name = makeName( name, kwargs)
    from AthenaCommon.DetFlags import DetFlags
    from InDetRecExample.InDetJobProperties import InDetFlags
    if not DetFlags.haveRIO.TRT_on() or InDetFlags.doHighPileup() \
            or  InDetFlags.useExistingTracksAsInput(): # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
        return None

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
    if not DetFlags.haveRIO.TRT_on() or  InDetFlags.doHighPileup() \
            or  InDetFlags.useExistingTracksAsInput(): # TRT_RDOs (used by the TRT_LocalOccupancy tool) are not present in ESD
        return None

    if 'TRTStrawSummaryTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRTStrawSummaryTool = getInDetTRTStrawStatusSummaryTool())

    if 'TRT_LocalOccupancyTool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_LocalOccupancyTool = getInDetTRT_LocalOccupancy())

    if 'TRT_ToT_dEdx_Tool' not in kwargs :
        kwargs = setDefaults( kwargs, TRT_ToT_dEdx_Tool = getInDetTRT_dEdxTool())
    
    kwargs = setDefaults( kwargs, CalculateNNPid = InDetFlags.doTRTPIDNN() )

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
    id_helper_args = copyArgs(kwargs,['ClusterSplitProbabilityName','namePrefix','nameSuffix']) if 'ClusterSplitProbabilityName' in kwargs else {}
    kwargs.pop('ClusterSplitProbabilityName',None)
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
                         doHolesInDet           = do_holes)         # we don't want to use those tools during pattern
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    return Trk__TrackSummaryTool(name = the_name, **kwargs)

def getInDetTrackSummaryToolNoHoleSearch(name='InDetTrackSummaryToolNoHoleSearch',**kwargs) :
    return getInDetTrackSummaryTool(name, **setDefaults(kwargs, doHolesInDet           = False))

def getInDetTrackSummaryToolSharedHits(name='InDetTrackSummaryToolSharedHits',**kwargs) :

    if 'InDetSummaryHelperTool' not in kwargs :
        copy_args=['ClusterSplitProbabilityName','namePrefix','nameSuffix']
        do_holes=kwargs.get("doHolesInDet",True)
        if do_holes :
            copy_args += ['isHLT']
        id_helper_args = copyArgs(kwargs,copy_args) if 'ClusterSplitProbabilityName' in kwargs else {}
        kwargs.pop('ClusterSplitProbabilityName',None)
        kwargs = setDefaults( kwargs, InDetSummaryHelperTool = getInDetSummaryHelperSharedHits(**id_helper_args))

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults(kwargs,
                         doSharedHits           = InDetFlags.doSharedHits())

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
        if kwargs.get('UseParameterization',InDetNewTrackingCuts.useParameterizedTRTCuts()) :
            kwargs=setDefaults(kwargs,
                               DriftCircleCutTool  = getInDetTRTDriftCircleCutForPatternReco(TrackingCuts=InDetNewTrackingCuts))
        else :
            kwargs=setDefaults(kwargs, DriftCircleCutTool  = '')

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


def getInDetTRT_ExtensionTool(TrackingCuts=None, **kwargs) :
    # @TODO set all names to InDetTRT_ExtensionTool ?
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doCosmics():
        return getInDetTRT_ExtensionToolCosmics(**kwargs)
    else :
        return getInDetTRT_TrackExtensionTool_xk(TrackingCuts=TrackingCuts, **kwargs)

def getTRT_DetElementsRoadCondAlg(**kwargs):
    the_name=kwargs.pop("name","InDet__TRT_DetElementsRoadCondAlg_xk")
    from TRT_DetElementsRoadTool_xk.TRT_DetElementsRoadTool_xkConf import InDet__TRT_DetElementsRoadCondAlg_xk
    return InDet__TRT_DetElementsRoadCondAlg_xk(the_name, **kwargs)

@makePublicTool
def getInDetAmbiScoringToolBase(name='InDetAmbiScoringTool', **kwargs) :
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    the_name=makeName(name,kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags
    have_calo_rois = InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        kwargs=setDefaults(kwargs, EMROIPhiRZContainer = "InDetCaloClusterROIPhiRZ5GeV")
    if 'DriftCircleCutTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           DriftCircleCutTool      = getInDetTRTDriftCircleCutForPatternReco())

    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    return InDet__InDetAmbiScoringTool(the_name,
                                       **setDefaults(kwargs,
                                                     Extrapolator            = getInDetExtrapolator(),
                                                     SummaryTool             = getInDetTrackSummaryTool(),
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

def getInDetAmbiScoringToolSi(NewTrackingCuts, name='InDetAmbiScoringToolSi', **kwargs) :
    return getInDetAmbiScoringTool(NewTrackingCuts,
                                   name,
                                   **setDefaults( kwargs,
                                                  DriftCircleCutTool      =  ''))

@makePublicTool
def getInDetNNScoringToolBase(name='InDetNNScoringTool', **kwargs) :
    NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    the_name=makeName(name,kwargs)
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.DetFlags              import DetFlags
    have_calo_rois = InDetFlags.doBremRecovery() and InDetFlags.doCaloSeededBrem() and DetFlags.detdescr.Calo_allOn()
    if have_calo_rois :
        kwargs=setDefaults(kwargs, EMROIPhiRZContainer = "InDetCaloClusterROIPhiRZ5GeV")

    if 'DriftCircleCutTool' not in kwargs :
        kwargs=setDefaults(kwargs,
                           DriftCircleCutTool      = getInDetTRTDriftCircleCutForPatternReco())

    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetNNScoringTool
    return InDet__InDetNNScoringTool(the_name,
                                       **setDefaults(kwargs,
                                                     nnCutConfig             = "dev/TrackingCP/LRTAmbiNetwork/20200727_225401/nn-config.json",
                                                     nnCutThreshold          = InDetFlags.nnCutLargeD0Threshold(),
                                                     Extrapolator            = getInDetExtrapolator(),
                                                     SummaryTool             = getInDetTrackSummaryTool(),
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

def getInDetNNScoringToolSi(NewTrackingCuts, name='InDetNNScoringToolSi', **kwargs) :
    return getInDetNNScoringTool(NewTrackingCuts,
                                 name+NewTrackingCuts.extension(),
                                 **setDefaults( kwargs,
                                                DriftCircleCutTool      = ''))

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

def searchProb(prob_val) :
    def iterateComp() :
       from AthenaCommon.AppMgr import ToolSvc
       from AthenaCommon.Configurable import ConfigurableAlgTool

       for a_tool in ToolSvc.getChildren() :
           yield a_tool
       from AthenaCommon.AlgSequence import AlgSequence
       topSequence = AlgSequence()
       for an_alg in topSequence.getChildren() :
           yield an_alg
           for name,prop in an_alg.getProperties().items() :
               if isinstance(prop,ConfigurableAlgTool) and not prop.isInToolSvc() :
                   yield prop

    from GaudiKernel.DataHandle import DataHandle
    for a_comp in iterateComp() :
        for name,prop in a_comp.getProperties().items() :
            if isinstance(prop,(str,DataHandle)) and str(prop) == prob_val :
                return True
    return False

def hasSplitProb(key) :
    # @TODO find better solution than searching through a huge number of properties
    from RecExConfig.AutoConfiguration import IsInInputFile
    if IsInInputFile('Trk::ClusterSplitProbabilityContainer',key) :
        return True

    if searchProb(key) :
        return True
    return False

def combinedClusterSplitProbName() :
  # precisely mimics the configuration in InDetRec_jobOptions
  # chaings in InDetRec_jobOptions to the ClusterSplitProbContainer also have to be implemented here
  # To synchronise with InDetRec_jobOptions the logic can be extracted with
  # grep "CombinedInDetClusterSplitProbContainer\|ClusterSplitProbContainer\|[[:space:]]\(el\|\)if\([[:space:]]\|(\)\|[[:space:]]else[[:space:]]*:\|ConfiguredNewTrackingCuts" 
  #    InnerDetector/InDetExample/InDetRecExample/share/InDetRec_jobOptions.py
  # @TODO find a better way to provide the final name of ClusterSplitProbContainer used for the combined InDetTrackParticles
  from AthenaCommon.BeamFlags import jobproperties
  from InDetRecExample.InDetJobProperties import InDetFlags
  CombinedInDetClusterSplitProbContainer = ''
  ClusterSplitProbContainer=''
  if InDetFlags.Enabled():
    from InDetRecExample.ConfiguredNewTrackingCuts import ConfiguredNewTrackingCuts
    if ('InDetNewTrackingCuts' not in dir()):
      if InDetFlags.doBLS():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("BLS")
      elif InDetFlags.doDBMstandalone():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("DBM")
      elif InDetFlags.doVtxLumi():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxLumi")
      elif InDetFlags.doVtxBeamSpot():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("VtxBeamSpot")
      elif InDetFlags.doCosmics():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Cosmics")
      elif InDetFlags.doHeavyIon():
        InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("HeavyIon")
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
    if InDetFlags.doLargeD0() or InDetFlags.runLRTReco() or InDetFlags.doLowPtLargeD0():
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

@makePublicTool
def getInDetFullLinearizedTrackFactory(name='InDetFullLinearizedTrackFactory', **kwargs) :
    the_name                    = makeName( name, kwargs)
    if 'Extrapolator' not in kwargs :
        kwargs=setDefaults(kwargs,Extrapolator           = getInDetExtrapolator()) # @TODO AtlasExtrapolator ? 

    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    return Trk__FullLinearizedTrackFactory(the_name, **kwargs)

@makePublicTool
def getTrackToVertexIPEstimator(name='TrackToVertexIPEstimator', **kwargs) :
    the_name                    = makeName( name, kwargs)
    if 'Extrapolator' not in kwargs :
        kwargs=setDefaults(kwargs,Extrapolator           = getInDetExtrapolator()) # @TODO AtlasExtrapolator ? 
    if 'LinearizedTrackFactory' not in kwargs :
        kwargs=setDefaults(kwargs, LinearizedTrackFactory = getInDetFullLinearizedTrackFactory() )
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
    return Trk__TrackToVertexIPEstimator( the_name, **kwargs)

@makePublicTool
def getV0Tools(name='V0Tools', **kwargs) :
    the_name                    = makeName( name, kwargs)
    if 'Extrapolator' not in kwargs :
        kwargs=setDefaults(kwargs,Extrapolator = getInDetExtrapolator()) # @TODO AtlasExtrapolator ?
    from TrkVertexAnalysisUtils.TrkVertexAnalysisUtilsConf import Trk__V0Tools
    return Trk__V0Tools(the_name, **kwargs)

def getInDetxAODParticleCreatorTool(prd_to_track_map=None, suffix="", trt_pid_tool=True, pixel_dedx=True) :
    from AthenaCommon.AppMgr import ToolSvc
    from InDetRecExample.InDetJobProperties import InDetFlags
    if hasattr(ToolSvc,'InDetxAODParticleCreatorTool'+suffix) :
        return getattr(ToolSvc,'InDetxAODParticleCreatorTool'+suffix)

    perigee_expression=InDetFlags.perigeeExpression()
    # need to treat Vertex specifically because at the time of
    # the track particle creation the primary vertex does not yet exist.
    # The problem is solved by first creating track particles wrt. the beam line
    # and correcting the parameters after the vertex finding.
    if perigee_expression == 'Vertex' :
        perigee_expression = 'BeamLine'

    if prd_to_track_map is None :
        track_summary_tool = getInDetTrackSummaryToolSharedHits()
    else :
        prop_args          = setDefaults({}, nameSuffix = suffix)
        asso_tool          = getConstPRD_AssociationTool(**setDefaults(prop_args,
                                                                                      PRDtoTrackMap = prd_to_track_map))
        helper_tool        = getInDetSummaryHelperSharedHits(**setDefaults(prop_args,
                                                                                          AssoTool = asso_tool) )
        track_summary_tool = getInDetTrackSummaryToolSharedHits(**setDefaults(prop_args,
                                                                                             InDetSummaryHelperTool=helper_tool))

    TRT_ElectronPidTool    = getInDetTRT_ElectronPidTool(MinimumTrackPtForNNPid = 2000.) if trt_pid_tool else None
    PixelToTPIDTool        = getInDetPixelToTPIDTool()                                   if pixel_dedx   else None
    LayerToolInner         = getInDetTestPixelLayerToolInner()                           if pixel_dedx   else None

    from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
    InDetxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetxAODParticleCreatorTool"+suffix,
                                                                 TrackToVertex           = getInDetTrackToVertexTool(),
                                                                 TrackSummaryTool        = track_summary_tool,
                                                                 TestPixelLayerTool      = LayerToolInner,
                                                                 ComputeAdditionalInfo   = True,
                                                                 BadClusterID            = InDetFlags.pixelClusterBadClusterID(),
                                                                 KeepParameters          = True,
                                                                 KeepFirstParameters     = InDetFlags.KeepFirstParameters(),
                                                                 PerigeeExpression       = perigee_expression,
                                                                 TRT_ElectronPidTool     = TRT_ElectronPidTool,
                                                                 PixelToTPIDTool         = PixelToTPIDTool)

    ToolSvc += InDetxAODParticleCreatorTool
    return InDetxAODParticleCreatorTool

@makePublicTool
def getTrackObserverTool(name='TrackObserverTool', write_tracks = False, **kwargs) :
    the_name = makeName( name, kwargs)
    from InDetRecExample.InDetKeys import InDetKeys
    from TrkValTools.TrkValToolsConf import Trk__TrkObserverTool
    TrackObserverTool = Trk__TrkObserverTool(the_name, **kwargs)
    if write_tracks:
        TrackObserverTool.ObsTrackCollection = InDetKeys.ObservedTracks()
        TrackObserverTool.ObsTrackCollectionMap = InDetKeys.ObservedTracks()+"Map"
    return TrackObserverTool
