# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id$
#
# @file PhotonD3PDMaker/python/PhotonD3PDObject.py
# @author Mike Hance <hance@hep.upenn.edu>
# @date July, 2010
# @brief Configure diphoton D3PD object.
#


from egammaD3PDAnalysis.PhotonUserDataConfig                import PhotonUserDataConfig
from D3PDMakerCoreComps.D3PDObject                          import make_SGDataVector_D3PDObject
from D3PDMakerCoreComps.release_version                     import release_version
from D3PDMakerCoreComps.IndexMultiAssociation               import IndexMultiAssociation
from D3PDMakerCoreComps.D3PDObject                          import D3PDObject
from D3PDMakerConfig.D3PDMakerFlags                         import D3PDMakerFlags
from D3PDMakerCoreComps.resolveSGKey                        import resolveSGKey
from TriggerD3PDMaker.defineTriggerBits                     import defineTriggerBits
from AthenaCommon.AlgSequence                               import AlgSequence
from RecExConfig.RecFlags                                   import rec
import egammaD3PDMaker
import EventCommonD3PDMaker
import D3PDMakerCoreComps
import PhotonD3PDMaker

DiPhotonD3PDObject = \
                   make_SGDataVector_D3PDObject ('PAU::DiPhotonContainer',
                                                 'PAUDiphotonCollection',
                                                 'diphoton_',
                                                 'DiPhotonD3PDObject')

def _diphotonAlgHook (c, prefix, sgkey, typeName,
                      seq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName()),
                      *args, **kw):

    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    PAUflags.PhotonContainer = D3PDMakerFlags.PhotonSGKey()
    
    PAUflags.AlgSequence = seq.getName()
    from PhotonAnalysisUtils.DiphotonAnalysisGetter import DiphotonAnalysisGetter
    dip = DiphotonAnalysisGetter()
    
    return


DiPhotonD3PDObject.defineHook (_diphotonAlgHook)




from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import ServiceMgr


from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
EMTrackIsolationTrackSummaryTool = AtlasTrackSummaryTool()
ToolSvc += EMTrackIsolationTrackSummaryTool

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
EMTrackIsolationExtrapolator = AtlasExtrapolator()
ToolSvc+=EMTrackIsolationExtrapolator

EMIsolBuildTestBLayerTool = None
if DetFlags.pixel_on():

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from IOVDbSvc.CondDB import conddb
    from InDetRecExample.InDetJobProperties import InDetFlags
    from AthenaCommon.GlobalFlags import globalflags
    
    #
    # --- Load PixelConditionsServices
    #
    # Load pixel conditions summary service
    from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
    InDetPixelConditionsSummarySvc = PixelConditionsSummarySvc()
      
    # Load pixel special pixel map services
    if athenaCommonFlags.isOnline() :
        InDetPixelConditionsSummarySvc.UseSpecialPixelMap = False
    else:
        InDetPixelConditionsSummarySvc.UseSpecialPixelMap = True
        if not conddb.folderRequested('/PIXEL/PixMapShort'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapShort")
        if not conddb.folderRequested('/PIXEL/PixMapLong'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapLong")
        if not conddb.folderRequested('/PIXEL/NoiseMapShort'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/NoiseMapShort")
        if not conddb.folderRequested('/PIXEL/NoiseMapLong'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/NoiseMapLong")
        if not conddb.folderRequested('/PIXEL/PixMapOverlay'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapOverlay")
        from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc
        InDetSpecialPixelMapSvc = SpecialPixelMapSvc(DBFolders  = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" , "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ] ,
                                                     SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong", "NoiseMapShort", "NoiseMapLong" ] ,
                                                     OverlayFolder       = "/PIXEL/PixMapOverlay",
                                                     OverlayKey          = "PixMapOverlay")
        ServiceMgr += InDetSpecialPixelMapSvc
        if InDetFlags.doPrintConfigurables():
            print InDetSpecialPixelMapSvc
        
        # Load pixel DCS information
    if InDetFlags.usePixelDCS():
        from PixelConditionsServices.PixelConditionsServicesConf import PixelDCSSvc
        if athenaCommonFlags.isOnline():
            if not conddb.folderRequested('/PIXEL/HLT/DCS/TEMPERATURE'):
                conddb.addFolder("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE")
            if not conddb.folderRequested('/PIXEL/HLT/DCS/HV'):
                conddb.addFolder("PIXEL_ONL","/PIXEL/HLT/DCS/HV")
                    
            InDetPixelDCSSvc =  PixelDCSSvc(RegisterCallback     = TRUE,
                                            TemperatureFolder    = "/PIXEL/HLT/DCS/TEMPERATURE",
                                            HVFolder             = "/PIXEL/HLT/DCS/HV",
                                            TemperatureFieldName = "temperature",
                                            HVFieldName          = "HV")
        else:
            if not conddb.folderRequested('/PIXEL/DCS/TEMPERATURE'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/TEMPERATURE")
            if not conddb.folderRequested('/PIXEL/DCS/HV'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/HV")
            if not conddb.folderRequested('/PIXEL/DCS/FSMSTATUS'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATUS")
            if not conddb.folderRequested('/PIXEL/DCS/FSMSTATE'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATE")
                
            InDetPixelDCSSvc =  PixelDCSSvc(RegisterCallback     = TRUE,
                                            TemperatureFolder    = "/PIXEL/DCS/TEMPERATURE",
                                            HVFolder             = "/PIXEL/DCS/HV",
                                            FSMStatusFolder      = "/PIXEL/DCS/FSMSTATUS",
                                            FSMStateFolder       = "/PIXEL/DCS/FSMSTATE",
                                            TemperatureFieldName = "temperature",
                                            HVFieldName          = "HV",
                                            FSMStatusFieldName   = "FSM_status",
                                            FSMStateFieldName    = "FSM_state" )
            ServiceMgr += InDetPixelDCSSvc
            if InDetFlags.doPrintConfigurables():
                print InDetPixelDCSSvc
    
        InDetPixelConditionsSummarySvc.UseDCS         = True
        InDetPixelConditionsSummarySvc.IsActiveStates = [ 'READY' ]
        InDetPixelConditionsSummarySvc.IsActiveStatus = [ 'OK', 'WARNING' ]
    
    
    # Load Pixel BS errors service
    if ( globalflags.DataSource == 'geant4' ) :
        # Due to a "feature" in the BS encoder for simulation,
        # the information of the BS error service
        # is not reliable on MC.
        InDetPixelConditionsSummarySvc.UseByteStream = False 
    else :
        from PixelConditionsServices.PixelConditionsServicesConf import PixelByteStreamErrorsSvc
        InDetPixelByteStreamErrorsSvc = PixelByteStreamErrorsSvc()
        if ( globalflags.InputFormat != 'bytestream' ):
            InDetPixelByteStreamErrorsSvc.ReadingESD = True
        InDetPixelConditionsSummarySvc.UseByteStream = True
    
        ServiceMgr += InDetPixelByteStreamErrorsSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetPixelByteStreamErrorsSvc
                
    # Register and printout configuration of  PixelConditionsSummarySvc
    ServiceMgr += InDetPixelConditionsSummarySvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetPixelConditionsSummarySvc


    from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
    EMIsolBuildTestBLayerTool = InDet__InDetTestBLayerTool(name= "EMIsolBuildTestBLayerTool",
                                                           PixelSummarySvc = InDetPixelConditionsSummarySvc,
                                                           Extrapolator    = EMTrackIsolationExtrapolator)
    ToolSvc += EMIsolBuildTestBLayerTool


from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
EMTrackIsolationPhotonTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(name = "EMTrackIsolationPhotonTrackSelectorTool",
                                                                                IPd0Max              = 1.5*mm,
                                                                                IPz0Max              = 3*mm,
                                                                                pTMin                = 1000,
                                                                                nHitBLayer           = 1,
                                                                                nHitPix              = 0,
                                                                                nHitSct              = 0,
                                                                                nHitSi               = 9,
                                                                                nHitTrt              = 0,
                                                                                nHolesPixel          = 0,
                                                                                nSharedBLayer        = 999,
                                                                                nSharedPix           = 999,
                                                                                nSharedSct           = 999,
                                                                                nSharedSi            = 999,
                                                                                useTrackQualityInfo  = False,
                                                                                d0MaxPreselection    = 9999.,
                                                                                TrackSummaryTool     = EMTrackIsolationTrackSummaryTool,
                                                                                Extrapolator         = EMTrackIsolationExtrapolator,
                                                                                InDetTestBLayerTool  = EMIsolBuildTestBLayerTool,
                                                                                OutputLevel          = WARNING)
ToolSvc += EMTrackIsolationPhotonTrackSelectorTool


from TrackIsolationTools.TrackIsolationToolsConf import TrackIsolationTool
EMTrackIsolationPhotonTrackIsolationTool = TrackIsolationTool(name = "EMTrackIsolationPhotonTrackIsolationTool",
                                                              VertexContainer = "PAUDiphotonVertexCollection",
                                                              DoTrackIsolation         = True,
                                                              DoSumConversionTracks    = False,
                                                              InnerCone                = 0.1,
                                                              TrackParticleCollection  = "TrackParticleCandidate",
                                                              ConversionContainer      = "AllPhotonsVxCandidates",
                                                              CaloCellContainer        = "AODCellContainer",
                                                              TrackSelector            = EMTrackIsolationPhotonTrackSelectorTool,
                                                              InnerConeTrackSelector   = None)
ToolSvc += EMTrackIsolationPhotonTrackIsolationTool
print EMTrackIsolationPhotonTrackIsolationTool


DiPhotonD3PDObject.defineBlock (0, 'PAUVariables',
                                PhotonD3PDMaker.PAUDiPhotonFillerTool,
                                PhotonTrackIsolationTool = EMTrackIsolationPhotonTrackIsolationTool
                                )

DiPhotonPhotonIndexAssoc = IndexMultiAssociation(DiPhotonD3PDObject,
                                                 PhotonD3PDMaker.PAUDiPhotonAssociationTool,
                                                 target = 'ph_',
                                                 prefix = 'ph_',
                                                 level = 1,
                                                 blockname = "PhotonIndex",
                                                 allowMissing = False)

