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
                                                                        TrackParticleCreator = InDetTrigMTxAODParticleCreatorTool,
                                                                        #Add online track monitoring
                                                                        DoMonitoring = True,
                                                                        TrackMonTool  = monTrackCnv
                                                                      )

  return InDetTrigMTxAODTrackParticleCnvAlg


