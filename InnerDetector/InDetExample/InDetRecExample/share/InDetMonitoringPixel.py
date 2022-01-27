#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file InDetMonitoringPixel.py
@brief Top configuration of Pixel Monitoring in Run 3 style but in Run 2 environment
'''
forceOnline = False # for testing of online monitoring and 100LB histograms

doHitMonAlg       = True
doClusterMonAlg   = True
doErrorMonAlg     = True

from PixelMonitoring.PixelMonitoringConf import PixelAthHitMonAlg
from PixelMonitoring.PixelAthHitMonAlgCfg import PixelAthHitMonAlgCfg

from PixelMonitoring.PixelMonitoringConf import PixelAthClusterMonAlg
from PixelMonitoring.PixelAthClusterMonAlgCfg import PixelAthClusterMonAlgCfg

from PixelMonitoring.PixelMonitoringConf import PixelAthErrorMonAlg
from PixelMonitoring.PixelAthErrorMonAlgCfg import PixelAthErrorMonAlgCfg

from InDetRecExample.InDetKeys import InDetKeys
from InDetRecExample import TrackingCommon

if forceOnline : 
  isOnline = True
else:
  isOnline = athenaCommonFlags.isOnline()
kwargsHitMonAlg = { 'doOnline'        : isOnline,      #Histograms for online (GlobalMonitoring) running
                     'doLumiBlock'     : not isOnline,     #Turn on/off histograms stored every 1(20) lumi block(s)
                     'doFEPlots'       : True,                                                #Turn on/off per FE-I3 histograms
                     'RDOName'         : InDetKeys.PixelRDOs()
}

kwargsClusMonAlg = { 'doOnline'        : isOnline,     #Histograms for online (GlobalMonitoring) running
                      'doLumiBlock'     : not isOnline,    #Turn on/off histograms stored every 1(20) lumi block(s)
                      'doLowOccupancy'  : InDetFlags.doCosmics(),      #Setting up 1D histogram ranges and binnings, if False, high occupancy i.e. collisions settings will be used
                      'doHeavyIonMon'   : InDetFlags.doHeavyIon(),     #Setting up 1D histogram ranges and binnings for heavy ions
                      'doFEPlots'       : True,                                               #Turn on/off per FE-I3 histograms
                      'ClusterName'     : InDetKeys.PixelClusters(),
                      'TrackName'       : InDetKeys.Tracks()
}

kwargsErrMonAlg = { 'doOnline'        : isOnline,      #Histograms for online (GlobalMonitoring) running
                     'doLumiBlock'     : not isOnline      #Turn on/off histograms stored every 1(20) lumi block(s)
}
                                                                           
from AthenaMonitoring.DQMonFlags import DQMonFlags                                                                                                                                      
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "NewPixelMonitoring")

if doHitMonAlg:
  pixelAthMonAlgHitMonAlg = helper.addAlgorithm(PixelAthHitMonAlg, 'PixelAthHitMonAlg')
  for k, v in kwargsHitMonAlg.items():
    setattr(pixelAthMonAlgHitMonAlg, k, v)
  PixelAthHitMonAlgCfg(helper, pixelAthMonAlgHitMonAlg, **kwargsHitMonAlg)

if doClusterMonAlg:
  pixelAthClusterMonAlg = helper.addAlgorithm(PixelAthClusterMonAlg, 'PixelAthClusterMonAlg')
  for k, v in kwargsClusMonAlg.items():
    setattr(pixelAthClusterMonAlg, k, v)
  pixelAthClusterMonAlg.HoleSearchTool   = TrackingCommon.getInDetHoleSearchTool()
  pixelAthClusterMonAlg.TrackSelectionTool.UseTrkTrackTools = True
  pixelAthClusterMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
  pixelAthClusterMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
  pixelAthClusterMonAlg.TrackSelectionTool.maxD0            = 2
  pixelAthClusterMonAlg.TrackSelectionTool.maxZ0            = 150
  pixelAthClusterMonAlg.TrackSelectionTool.TrackSummaryTool = TrackingCommon.getInDetTrackSummaryTool()
  pixelAthClusterMonAlg.TrackSelectionTool.Extrapolator     = TrackingCommon.getInDetExtrapolator()
  
  #print getattr(pixelAthClusterMonAlg, 'onTrack') 
  PixelAthClusterMonAlgCfg(helper, pixelAthClusterMonAlg, **kwargsClusMonAlg)

if doErrorMonAlg:
  pixelAthMonAlgErrorMonAlg = helper.addAlgorithm(PixelAthErrorMonAlg, 'PixelAthErrorMonAlg')
  for k, v in kwargsErrMonAlg.items():
    setattr(pixelAthMonAlgErrorMonAlg, k, v)
  PixelAthErrorMonAlgCfg(helper, pixelAthMonAlgErrorMonAlg, **kwargsErrMonAlg)

topSequence += helper.result()



