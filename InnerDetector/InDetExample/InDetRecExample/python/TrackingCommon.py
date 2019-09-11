# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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

def setDefaults(kwargs, **def_kwargs) :
    def_kwargs.update(kwargs)
    return def_kwargs

def makeName( name, kwargs) :
    namePrefix=kwargs.pop('namePrefix','')
    nameSuffix=kwargs.pop('nameSuffix','')
    return namePrefix + name + nameSuffix

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
        if len(args) == 1 :
            if name != None :
                raise Exception('Name given as positional and keyword argument')
            name=args[0]
        if name is None :
            name=getDefaultName(tool_creator)

        the_name =  kwargs.get('namePrefix','') + name + kwargs.get('nameSuffix','')
        if the_name not in ToolSvc :
            if len(args) > 1 :
                raise Exception('Too many positional arguments')
            tool = tool_creator(name, **kwargs)
            if the_name != tool.name() :
                raise Exception('Tool has not the exepected name %s but %s' % (the_name, tool.the_name()))
            ToolSvc += tool
            return tool
        else :
            return getattr(ToolSvc, the_name)

    createPublicTool.__name__   = tool_creator.__name__
    createPublicTool.__module__ = tool_creator.__module__
    return createPublicTool

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
        conddb.addFolder("PIXEL_OFL","/PIXEL/PixelClustering/PixelClusNNCalib",className='CondAttrListCollection')

    kwargs=setDefaults(kwargs,
                       NetworkNames = nn_names,
                       WriteKey     ='PixelClusterNN' if not track_nn else 'PixelClusterNNWithTrack')

    if 'NetworkToHistoTool' not in kwargs :
        kwargs = setDefaults( kwargs,
                              NetworkToHistoTool   = getNeuralNetworkToHistoTool() )

    from SiClusterizationTool.SiClusterizationToolConf import InDet__TTrainedNetworkCondAlg
    return InDet__TTrainedNetworkCondAlg(kwargs.pop("name", 'PixelClusterNnCondAlg'), **kwargs)

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

    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as geoFlags
    do_runI = geoFlags.Run() not in ["RUN2", "RUN3"]
    createAndAddCondAlg( getPixelClusterNnCondAlg,         'PixelClusterNnCondAlg',          GetInputsInfo = do_runI)
    createAndAddCondAlg( getPixelClusterNnWithTrackCondAlg,'PixelClusterNnWithTrackCondAlg', GetInputsInfo = do_runI)

    from InDetRecExample.InDetJobProperties import InDetFlags
    kwargs = setDefaults( kwargs,
                          doRunI                             = do_runI,
                          useToT                             = False if do_runI else InDetFlags.doNNToTCalibration(),
                          useRecenteringNNWithoutTracks      = True  if do_runI else False,  # default,
                          useRecenteringNNWithTracks         = False if do_runI else False,  # default,
                          correctLorShiftBarrelWithoutTracks = 0,
                          correctLorShiftBarrelWithTracks    = 0.030 if do_runI else 0.000,  # default,
                          NnCollectionReadKey                = 'PixelClusterNN',
                          NnCollectionWithTrackReadKey       = 'PixelClusterNNWithTrack')
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
                         RunningTIDE_Ambi         = InDetFlags.doTIDE_Ambi())

    return InDet__PixelClusterOnTrackTool(the_name , **kwargs)

def getInDetPixelClusterOnTrackToolNNSplitting(name='InDetPixelClusterOnTrackToolNNSplitting', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if InDetFlags.doPixelClusterSplitting() and InDetFlags.pixelClusterSplittingType() == 'NeuralNet':
        if 'NnClusterizationFactory' not in kwargs :
            kwargs = setDefaults(kwargs, NnClusterizationFactory  = getNnClusterizationFactory())

        if InDetFlags.doTIDE_RescalePixelCovariances() :
            kwargs = setDefaults(kwargs, applydRcorrection = True)
    return getInDetPixelClusterOnTrackToolBase(name=name, **kwargs)

def getInDetPixelClusterOnTrackTool(name='InDetPixelClusterOnTrackTool', **kwargs) :
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getPixelLorentzAngleTool())

    return getInDetPixelClusterOnTrackToolNNSplitting(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolPattern(name='InDetPixelClusterOnTrackToolPattern', **kwargs) :
    return getInDetPixelClusterOnTrackToolNNSplitting(name=name, **kwargs)

def getInDetPixelClusterOnTrackToolDigital(name='InDetPixelClusterOnTrackToolDigital', **kwargs) :
    from InDetRecExample.InDetJobProperties import InDetFlags
    if 'LorentzAngleTool' not in kwargs :
        kwargs = setDefaults(kwargs, LorentzAngleTool = getPixelLorentzAngleTool())

    if InDetFlags.doDigitalROTCreation():
        kwargs = setDefaults(kwargs,
                             applyNNcorrection = False,
                             NNIBLcorrection   = False,
                             ErrorStrategy     = 2,
                             PositionStrategy  = 1)
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

# @TODO rename to InDetTRT_DriftCircleOnTrackTool ?
def getInDetTRT_DriftCircleOnTrackTool(name='TRT_DriftCircleOnTrackTool', **kwargs) :
    createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')
    kwargs = setDefaults(kwargs,
                         EventInfoKey       = getEventInfoKey(),
                         TRTErrorScalingKey = '/Indet/TrkErrorScalingTRT')
    return getInDetBroadTRT_DriftCircleOnTrackTool(name = name, **kwargs)

default_ScaleHitUncertainty = 2.5

@makePublicTool
def getInDetTRT_DriftCircleOnTrackUniversalTool(name='InDetTRT_RefitRotCreator',**kwargs) :
    the_name = makeName( name, kwargs)
    if 'RIOonTrackToolDrift' not in kwargs :
        kwargs = setDefaults(kwargs, RIOonTrackToolDrift = getInDetTRT_DriftCircleOnTrackTool())
    if 'RIOonTrackToolTube' not in kwargs :
        kwargs = setDefaults(kwargs, RIOonTrackToolTube  = getInDetBroadTRT_DriftCircleOnTrackTool())

    from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackUniversalTool
    return InDet__TRT_DriftCircleOnTrackUniversalTool(name = the_name, **setDefaults(kwargs,
                                                                                     ScaleHitUncertainty = default_ScaleHitUncertainty))

@makePublicTool
def getInDetRotCreator(name='InDetRotCreator', **kwargs) :
    the_name = makeName( name, kwargs)
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    from InDetRecExample.InDetJobProperties import InDetFlags
    use_broad_cluster_pix = InDetFlags.useBroadPixClusterErrors() and (not InDetFlags.doDBMstandalone())
    use_broad_cluster_sct = InDetFlags.useBroadSCTClusterErrors() and (not InDetFlags.doDBMstandalone())

    if 'ToolPixelCluster' not in kwargs :
        if use_broad_cluster_pix :
            kwargs = setDefaults( kwargs,
                                  ToolPixelCluster = getInDetBroadPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                          SplitClusterMapExtension = split_cluster_map_extension))
        else :
            kwargs = setDefaults( kwargs,
                                  ToolPixelCluster = getInDetPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                     SplitClusterMapExtension = split_cluster_map_extension))

    if 'ToolSCT_Cluster' not in kwargs :
        if use_broad_cluster_sct :
            kwargs = setDefaults( kwargs, ToolSCT_Cluster = getInDetBroadSCT_ClusterOnTrackTool())
        else :
            kwargs = setDefaults( kwargs, ToolSCT_Cluster = getInDetSCT_ClusterOnTrackTool())

    kwargs = setDefaults( kwargs, Mode             = 'indet')

    from TrkRIO_OnTrackCreator.TrkRIO_OnTrackCreatorConf import Trk__RIO_OnTrackCreator
    return Trk__RIO_OnTrackCreator(name=the_name, **kwargs)

def getInDetRotCreatorPattern(name='InDetRotCreatorPattern', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolPattern(nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension))
    return getInDetRotCreator(name=name, **kwargs)


def getInDetRotCreatorDBM(name='InDetRotCreatorDBM', **kwargs) :
    split_cluster_map_extension = kwargs.pop('SplitClusterMapExtension','')
    if 'ToolPixelCluster' not in kwargs :
        from InDetRecExample.InDetJobProperties import InDetFlags
        from AthenaCommon.DetFlags              import DetFlags
        if InDetFlags.loadRotCreator() and DetFlags.haveRIO.pixel_on():
            kwargs = setDefaults(kwargs,
                                 ToolPixelCluster = getInDetPixelClusterOnTrackToolDBM(nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension))
        else :
            kwargs = setDefaults(kwargs,
                                 ToolPixelCluster = getInDetPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                    SplitClusterMapExtension = split_cluster_map_extension))
    return getInDetRotCreator(name=name, **kwargs)

def getInDetRotCreatorDigital(name='InDetRotCreatorDigital', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster = getInDetPixelClusterOnTrackToolDigital(nameSuffix               = split_cluster_map_extension,
                                                                                       SplitClusterMapExtension = split_cluster_map_extension))
    return getInDetRotCreator(name=name, **kwargs)

# @TODO rename to InDetBroadRotCreator
def getInDetBroadRotCreator(name='InDetBroadInDetRotCreator', **kwargs) :
    if 'ToolPixelCluster' not in kwargs :
        split_cluster_map_extension = kwargs.get('SplitClusterMapExtension','')
        kwargs = setDefaults(kwargs,
                             ToolPixelCluster    = getInDetBroadPixelClusterOnTrackTool(nameSuffix               = split_cluster_map_extension,
                                                                                        SplitClusterMapExtension = split_cluster_map_extension))
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
def getInDetGsfMaterialUpdator(name='InDetGsfMaterialUpdator', **kwargs) :
    the_name = makeName( name, kwargs)
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfMaterialMixtureConvolution
    return Trk__GsfMaterialMixtureConvolution (name = the_name, **kwargs)

@makePublicTool
def getInDetGsfComponentReduction(name='InDetGsfComponentReduction', **kwargs) :
    the_name = makeName( name, kwargs)
    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__QuickCloseComponentsMultiStateMerger
    return Trk__QuickCloseComponentsMultiStateMerger (name = the_name, **setDefaults(kwargs, MaximumNumberOfComponents = 12))

@makePublicTool
def getInDetGsfExtrapolator(name='InDetGsfExtrapolator', **kwargs) :
    the_name = makeName(name,kwargs)
    if 'Propagators' not in kwargs :
        from AthenaCommon.AppMgr import ToolSvc
        kwargs=setDefaults(kwargs, Propagators = [ ToolSvc.InDetPropagator ] )

    if 'Navigator' not in kwargs :
        from AthenaCommon.AppMgr import ToolSvc
        kwargs=setDefaults(kwargs, Navigator   =  ToolSvc.InDetNavigator)

    if 'GsfMaterialConvolution' not in kwargs :
        kwargs=setDefaults(kwargs, GsfMaterialConvolution        = getInDetGsfMaterialUpdator())

    if 'ComponentMerger' not in kwargs :
        kwargs=setDefaults(kwargs, ComponentMerger               = getInDetGsfComponentReduction())

    from TrkGaussianSumFilter.TrkGaussianSumFilterConf import Trk__GsfExtrapolator
    return Trk__GsfExtrapolator(name = the_name, **setDefaults(kwargs,
                                                               SearchLevelClosestParameters  = 10,
                                                               StickyConfiguration           = True,
                                                               SurfaceBasedMaterialEffects   = False ))
