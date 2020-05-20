#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
#     Contains algs/tools used by Inner Detector Trigger
  

#TODO 
#need to implement this in both FTF and PT
#Better retrieval of summary tool (rather than through a parameter?)? Use the correct one!
def getTrackParticleCnv( prefix, suffix, outPTTracks, outPTTrackParticles ):
  """ Use the  track collection and convert it to container of xAOD Track Particles  """
  #suffix = '_%s'suff if suff else '' #Suffix should be based on the signature and type of tracking, e.g for cosmics and PT  -> Cosmics_PT ( for FTF or EFID, Cosmics_FTF, Cosmics_EFID )
  from AthenaCommon.AppMgr import ToolSvc
  keepParameter = False 
  if 'Electron' in suffix:
      keepParameter = True
      

  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool, InDetTrigExtrapolator
  from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
  InDetTrigMTxAODParticleCreatorTool = Trk__TrackParticleCreatorTool(name =  "%sxAODParticleCreatorTool%s" %(prefix, suffix),
                                                                     Extrapolator = InDetTrigExtrapolator,
                                                                     KeepParameters = keepParameter,
                                                                     #TrackSummaryTool = InDetTrigTrackSummaryToolSharedHits) 
                                                                     TrackSummaryTool = InDetTrigTrackSummaryTool )
  
  ToolSvc += InDetTrigMTxAODParticleCreatorTool
  #log.info(InDetTrigMTxAODParticleCreatorTool)
  
  
  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackCollectionCnvTool
  InDetTrigMTxAODTrackCollectionCnvTool= xAODMaker__TrackCollectionCnvTool(name = "%sxAODTrackCollectionCnvTool%s" %(prefix, suffix),
                                                                           TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTxAODTrackCollectionCnvTool
  #log.info(InDetTrigMTxAODTrackCollectionCnvTool)
  
  #This one shouldn't be necessary
  #TODO: obsolete, try to turn off
  from xAODTrackingCnv.xAODTrackingCnvConf import  xAODMaker__RecTrackParticleContainerCnvTool
  InDetTrigMTRecTrackParticleContainerCnvTool=  xAODMaker__RecTrackParticleContainerCnvTool(name = "%sRecTrackContainerCnvTool%s" %(prefix, suffix),
                                                                                            TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool)
  
  ToolSvc += InDetTrigMTRecTrackParticleContainerCnvTool



  #Adding new monitoring tool
  #First load the generic monitoring tool with set of histograms for Particle Cnv
  from TrigInDetMonitoringTools.TrigInDetTrackingMonitoring import  TrigInDetTrackCnvMonitoring
  #TODO need to add extra property for prefix and suffix of the track collections to be monitored
  #Or maybe no need the name itself should be obvious from the particle cnv alg
  monTool = TrigInDetTrackCnvMonitoring( name = '%sParticleCreatorCnv%s'%( prefix, suffix ) )

  #Now pass this tool to the Track Monitoring tool
  from TrigInDetMonitoringTools.TrigInDetTrackMonitoringToolConf import TrigInDetTrackMonitoringTool
  monTrackCnv = TrigInDetTrackMonitoringTool( name = '%sParticleCreatorCnv%s'%( prefix, suffix ),
                                             MonitoringTool = monTool
                                            )

  
   
  ToolSvc += monTrackCnv

  from xAODTrackingCnv.xAODTrackingCnvConf import xAODMaker__TrackParticleCnvAlg
  InDetTrigMTxAODTrackParticleCnvAlg = xAODMaker__TrackParticleCnvAlg(  name = "%sxAODParticleCreatorAlg%s" %( prefix, suffix),
                                                                      # Properties below are used for:  TrackCollection -> xAOD::TrackParticle
                                                                        ConvertTracks = True,  #Turn on  retrieve of TrackCollection, false by default
                                                                        TrackContainerName                        = outPTTracks,
                                                                        xAODTrackParticlesFromTracksContainerName = outPTTrackParticles, 
                                                                        TrackCollectionCnvTool = InDetTrigMTxAODTrackCollectionCnvTool,
                                                                       ## Properties below are used for: Rec:TrackParticle, aod -> xAOD::TrackParticle (Turn off)
                                                                        ConvertTrackParticles = False,  # Retrieve of Rec:TrackParticle, don't need this atm
                                                                        xAODContainerName = '',  
                                                                        RecTrackParticleContainerCnvTool = InDetTrigMTRecTrackParticleContainerCnvTool,
                                                                        #PrintIDSummaryInfo = True, #Just to test and have some output
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool,
                                                                        #Add online track monitoring
                                                                        DoMonitoring = True,
                                                                        TrackMonTool  = monTrackCnv
                                                                      )

  return InDetTrigMTxAODTrackParticleCnvAlg





#-----------------------------------------------------------------------------
#                        Choose track summary helper tool
#
#
#
#  from AthenaCommon.GlobalFlags import globalflags
#  
#  from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRTStrawStatusSummaryTool
#  
#  from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
#  from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
#  InDetTrigTrackSummaryHelperTool = InDet__InDetTrackSummaryHelperTool(name          = "InDetTrigSummaryHelper",
#                                                                       HoleSearch    = InDetTrigHoleSearchTool,
#                                                                       AssoTool      = InDetTrigPrdAssociationTool,
#                                                                       TestBLayerTool = None,
#                                                                       PixelToTPIDTool= InDetTrigPixelToTPIDTool,
#                                                                       DoSharedHits  = False,
#                                                                       TRTStrawSummarySvc=InDetTrigTRTStrawStatusSummaryTool,
#                                                                       usePixel      = DetFlags.haveRIO.pixel_on(),
#                                                                       useSCT        = DetFlags.haveRIO.SCT_on(),
#                                                                       useTRT        = DetFlags.haveRIO.TRT_on())
#    
#  ToolSvc += InDetTrigTrackSummaryHelperTool
#  if (InDetTrigFlags.doPrintConfigurables()):
#    print (     InDetTrigTrackSummaryHelperTool)
#
#   from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
#    from InDetTrigRecExample.InDetTrigConditionsAccess import TRT_ConditionsSetup
#    InDetTrigTrackSummaryHelperToolSharedHits = InDet__InDetTrackSummaryHelperTool(name         = "InDetTrigSummaryHelperSharedHits",
#                                                                                   AssoTool     = InDetTrigPrdAssociationTool,
#                                                                                   DoSharedHits = InDetTrigFlags.doSharedHits(),
#                                                                                   HoleSearch   = InDetTrigHoleSearchTool,
#                                                                                   TestBLayerTool=InDetTrigTestBLayerTool,
#                                                                                   PixelToTPIDTool=InDetTrigPixelToTPIDTool,
#                                                                                   TRTStrawSummarySvc = InDetTrigTRTStrawStatusSummaryTool)
#
#


def getInDetTrigTrackSummaryToolBase(slice, name='InDetTrigTrackSummaryTool', **kwargs) :
  the_name=makeName(name,kwargs)

  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  return Trk__TrackSummaryTool(the_name,
                                       **setDefaults(kwargs,
                                                    InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
                                                    doSharedHits           = False,
                                                    doHolesInDet           = True,
                                                    #this may be temporary #61512 (and used within egamma later)
                                                    #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool, 
                                                    TRT_ElectronPidTool    = None, 
                                                    )

def getInDetTrigTrackSummaryTool(slice, name='InDetTrigTrackSummaryTool', **kwargs) :
  the_name=makeName(name,kwargs)
  return getInDetTrigTrackSummaryToolBase(name = the_name,
                                       **setDefaults(kwargs,
                                                    InDetSummaryHelperTool = InDetTrigTrackSummaryHelperTool,
                                                    doSharedHits           = False,
                                                    doHolesInDet           = True,
                                                    #this may be temporary #61512 (and used within egamma later)
                                                    #TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool, 
                                                    TRT_ElectronPidTool    = None, 
                                                    )

  #-----------------------------------------------------------------------------
  #                        Choose track summary tool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTrackSummaryTool
  from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
  from InDetTrigRecExample.InDetTrigConfigRecLoadTools import  InDetTrigTrackSummaryHelperToolSharedHits,InDetTrigTRT_ElectronPidTool

  InDetTrigTrackSummaryToolSharedHitsWithTRTPid  = Trk__TrackSummaryTool(name = "%sTrackSummaryToolSharedHitsWithTRT%s"%(algNamePrefix, signature),
                          InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                          doSharedHits           = InDetTrigFlags.doSharedHits(),
                          doHolesInDet           = True,
                          TRT_ElectronPidTool    = InDetTrigTRT_ElectronPidTool)


  if whichSignature == "electron" or "tau" in whichSignature :
      Parameter_config = True 
      SummaryTool_config = InDetTrigTrackSummaryToolSharedHitsWithTRTPid
  else:
      SummaryTool_config = InDetTrigTrackSummaryTool
      Parameter_config = False


   #Shared hits
    from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
    InDetTrigTrackSummaryToolSharedHits = Trk__TrackSummaryTool(name = "InDetTrigTrackSummaryToolSharedHits",
                                                                InDetSummaryHelperTool = InDetTrigTrackSummaryHelperToolSharedHits,
                                                                doSharedHits           = InDetTrigFlags.doSharedHits(),
                                                                doHolesInDet           = True,
                                                                TRT_ElectronPidTool    = None)

    ToolSvc += InDetTrigTrackSummaryToolSharedHits
    if (InDetTrigFlags.doPrintConfigurables()):
      print (     InDetTrigTrackSummaryToolSharedHits)


##----------------------
## Scoring tools
# @NOTE: add some description what it does
##----------------------

def getInDetTrigAmbiScoringToolBase( slice,  name='InDetAmbiScoringTool', **kwargs) :
    from InDetRecExample.TrackingCommon import makeName
    #NewTrackingCuts = kwargs.pop("NewTrackingCuts")
    #TODO: add InDetTrigMT to offline common tracking naming functions/make our own
    the_name=makeName(name,kwargs)
    
    from InDetTrigRecExample.InDetTrigSliceSettings import InDetTrigSliceSettings
    ptintcut = InDetTrigSliceSettings[('pTmin',slice)]
    if slice=='minBias':
      ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',slice)]
    elif slice=='minBias400':
      ptintcut = 0.95*InDetTrigSliceSettings[('pTmin',slice)]

    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigExtrapolator
    from InDetTrigRecExample.InDetTrigConfigRecLoadTools import InDetTrigTRTDriftCircleCut

    #This seems to be run2 cuts setting, should we not have different for each signature/slice? 
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    InDetTrigCutValues = EFIDTrackingCuts


    from InDetTrackScoringTools.InDetTrackScoringToolsConf import InDet__InDetAmbiScoringTool
    return InDet__InDetAmbiScoringTool(the_name,
                                       **setDefaults(kwargs,
                                                     Extrapolator        = InDetTrigExtrapolator, #TODO: getInDetExtrapolator(),
                                                     SummaryTool         = #TODO:need to retriee correct summary getInDetTrackSummaryTool(),
                                                     DriftCircleCutTool  = InDetTrigTRTDriftCircleCut, #TODO: getInDetTRTDriftCircleCutForPatternReco(),
                                                     useAmbigFcn         = True,   # this is NewTracking
                                                     useTRT_AmbigFcn     = False,
                                                     #to have a steeper turn-n curve
                                                     minPt               = ptintcut,
                                                     maxRPhiImp          = InDetTrigCutValues.maxPrimaryImpact(),
                                                     maxZImp             = InDetTrigCutValues.maxZImpact(),
                                                     maxEta              = InDetTrigCutValues.maxEta(),
                                                     minSiClusters       = InDetTrigCutValues.minClusters(),
                                                     maxSiHoles          = InDetTrigCutValues.maxHoles(),
                                                     maxPixelHoles       = InDetTrigCutValues.maxPixelHoles(),
                                                     maxSCTHoles         = InDetTrigCutValues.maxSCTHoles(),
                                                     maxDoubleHoles      = InDetTrigCutValues.maxDoubleHoles(),
                                                     usePixel            = InDetTrigCutValues.usePixel(),
                                                     useSCT              = InDetTrigCutValues.useSCT(),
                                                     doEmCaloSeed        = False
                                                     )
   
def getInDetTrigAmbiScoringTool(slice, name='InDetAmbiScoringTool', **kwargs) :
    #Retrieve base tool
    ambScoringTool =  getInDetAmbiScoringToolBase(name
                                       **setDefaults( kwargs,
                                                      minTRTonTrk  = 0,    # no TRT here
                                                    )
    #Is this still needed?
    if slice=='beamgas':
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCutsBeamGas
        ambScoringTool.minPt          = EFIDTrackingCutsBeamGas.minPT()
        ambScoringTool.maxRPhiImp     = EFIDTrackingCutsBeamGas.maxPrimaryImpact()
        ambScoringTool.maxZImp        = EFIDTrackingCutsBeamGas.maxZImpact()
        ambScoringTool.minSiClusters  = EFIDTrackingCutsBeamGas.minClusters()
        ambScoringTool.maxSiHoles     = EFIDTrackingCutsBeamGas.maxHoles()
        ambScoringTool.useTRT_AmbigFcn= False
        ambScoringTool.useSigmaChi2   = True

    return ambScoringTool





def getInDetTrigExtenScoringTool(slice, name='InDetAmbiScoringTool', **kwargs) :
    #This seems to be run2 cuts setting, should we not have different for each signature/slice? 
    from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import EFIDTrackingCuts
    InDetTrigCutValues = EFIDTrackingCuts

    #Retrieve base tool
    return getInDetAmbiScoringToolBase(name
                                       **setDefaults( kwargs,
                                                      minTRTonTrk  = InDetTrigCutValues.minTRTonTrk(),
                                                      #useSigmaChi2   = False # tuning from Thijs
                                                     )
