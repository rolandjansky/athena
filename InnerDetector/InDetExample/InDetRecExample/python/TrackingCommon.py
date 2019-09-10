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

def getPixelRIO_OnTrackErrorScalingDbOverrideCondAlg( **kwargs) :
    '''
    usage:
       createAndAddCondAlg( getPixelRIO_OnTrackErrorScalingDbOverrideCondAlg, 'PixelRIO_OnTrackErrorScalingDbOverrideCondAlg' )
    '''
    the_name=kwargs.pop("name",None)
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
    return RIO_OnTrackErrorScalingDbOverrideCondAlg( 'PixelRIO_OnTrackErrorScalingDbOverrideCondAlg',
                                                     **setDefaults( kwargs,
                                                                    ErrorScalingTypeName  = error_scaling_type,
                                                                    WriteKey              = "/Indet/TrkErrorScalingPixel",
                                                                    ErorScalingParameters = params,
                                                                    OutputLevel = 1) ) # VERBOSE

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
    from AthenaCommon.GlobalFlags  import globalflags
    from AthenaCommon.DetFlags import DetFlags

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


def getTRT_DriftCircleOnTrackTool() :
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc,'TRT_DriftCircleOnTrackTool') :
        createAndAddCondAlg(getRIO_OnTrackErrorScalingCondAlg,'RIO_OnTrackErrorScalingCondAlg')
        from TRT_DriftCircleOnTrackTool.TRT_DriftCircleOnTrackToolConf import InDet__TRT_DriftCircleOnTrackTool
        tool = InDet__TRT_DriftCircleOnTrackTool( EventInfoKey      = getEventInfoKey(),
                                                  TRTErrorScalingKey = '/Indet/TrkErrorScalingTRT')
        log.debug('default name = %s', tool.getName())
        ToolSvc += tool
        return tool
    else :
        return ToolSvc.TRT_DriftCircleOnTrackTool

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
