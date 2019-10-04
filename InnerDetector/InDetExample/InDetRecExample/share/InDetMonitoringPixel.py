#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file InDetMonitoringPixel.py
@brief Top configuration of Pixel Monitoring in Run 3 style but in Run 2 environment
'''

doHitMonTool       = False
doClusterMonTool   = True
doErrorMonTool     = True

from PixelMonitoring.PixelMonitoringConf import PixelAthHitMonTool
from PixelMonitoring.PixelAthHitMonToolCfg import PixelAthHitMonToolCfg

from PixelMonitoring.PixelMonitoringConf import PixelAthClusterMonTool
from PixelMonitoring.PixelAthClusterMonToolCfg import PixelAthClusterMonToolCfg

from PixelMonitoring.PixelMonitoringConf import PixelAthErrorMonTool
from PixelMonitoring.PixelAthErrorMonToolCfg import PixelAthErrorMonToolCfg

from InDetRecExample.InDetKeys import InDetKeys                                                                                     

kwargsHitMonTool = { 'doOnline'        : True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                     'doModules'       : True if athenaCommonFlags.isOnline() else False, #Turn on/off the sets of 1744 module histograms (for dqmf)
                     'doLumiBlock'     : False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                     'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                     'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                     'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                     'RDOName'         : InDetKeys.PixelRDOs()
}

kwargsClusMonTool = { 'doOnline'        : True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                      'doModules'       : True if athenaCommonFlags.isOnline() else False, #Turn on/off the sets of 1744 module histograms (for dqmf)
                      'doLumiBlock'     : False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                      'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam
                      'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                      'doHeavyIonMon'   : InDetFlags.doHeavyIon(),   # Histogram modification for heavy ion monitoring
                      #                  'doStatus'        : True,       #Turn on/off histograms with Module Status information
                      'ClusterName'     : InDetKeys.PixelClusters(),
                      'TrackName'       : InDetKeys.Tracks()
}

kwargsErrMonTool = { 'doOnline'        : True if athenaCommonFlags.isOnline() else False,      #Histograms for online (athenaPT) running
                     'doModules'       : True if athenaCommonFlags.isOnline() else False, #Turn on/off the sets of 1744 module histograms (for dqmf)
                     'doLumiBlock'     : False if athenaCommonFlags.isOnline() else True,       #Turn on/off histograms stored for each lumi block
                     'doLowOccupancy'  : False,      #Turn on/off histograms with binning for cosmics/single beam                    
                     'doHighOccupancy' : True,       #Turn on/off histograms with binning for collisions
                     'doHeavyIonMon'   : InDetFlags.doHeavyIon()
}


                                                                           
from AthenaMonitoring.DQMonFlags import DQMonFlags                                                                                                                                      
from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "NewPixelMonitoring")


if doHitMonTool:
  pixelAthMonAlgHitMonTool = helper.addAlgorithm(PixelAthHitMonTool, 'PixelAthHitMonTool', 'PixelAthHitMonTool')
  for k, v in kwargsHitMonTool.items():
    setattr(pixelAthMonAlgHitMonTool, k, v)
  PixelAthHitMonToolCfg(helper, pixelAthMonAlgHitMonTool, **kwargsHitMonTool)


if doClusterMonTool:
  pixelAthClusterMonTool = helper.addAlgorithm(PixelAthClusterMonTool, 'PixelAthClusterMonTool', 'PixelAthClusterMonTool')
  for k, v in kwargsClusMonTool.items():
    setattr(pixelAthClusterMonTool, k, v)
  pixelAthClusterMonTool.TrackSelectionTool.UseTrkTrackTools = True
  pixelAthClusterMonTool.TrackSelectionTool.CutLevel         = "TightPrimary"
  pixelAthClusterMonTool.TrackSelectionTool.maxNPixelHoles   = 1
  pixelAthClusterMonTool.TrackSelectionTool.maxD0            = 2
  pixelAthClusterMonTool.TrackSelectionTool.maxZ0            = 150
  pixelAthClusterMonTool.TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
  pixelAthClusterMonTool.TrackSelectionTool.Extrapolator     = InDetExtrapolator
  
  #print getattr(pixelAthClusterMonTool, 'onTrack') 
  PixelAthClusterMonToolCfg(helper, pixelAthClusterMonTool, **kwargsClusMonTool)

if doErrorMonTool:
  pixelAthMonAlgErrorMonTool = helper.addAlgorithm(PixelAthErrorMonTool, 'PixelAthErrorMonTool', 'PixelAthErrorMonTool')
  for k, v in kwargsErrMonTool.items():
    setattr(pixelAthMonAlgErrorMonTool, k, v)
  PixelAthErrorMonToolCfg(helper, pixelAthMonAlgErrorMonTool, **kwargsErrMonTool)


topSequence += helper.result()



