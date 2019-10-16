#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthHitMonToolCfg.py
@brief Configuration of Pixel Hit Monitoring Histograms for Run 3
'''

from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist
from PixelMonitoring.PixelAthMonitoringBase import define1DLayers
from PixelMonitoring.PixelAthMonitoringBase import define1DProfLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import layers, totcuts, xbinsem, xminsem, lumibinsx
from PixelMonitoring.PixelAthMonitoringBase import addOnTrackTxt, fullDressTitle
from RecExConfig.AutoConfiguration import GetRunNumber
runNumber = GetRunNumber()
runtext = ' (Run ' + str(runNumber) + ')'

def PixelAthHitMonToolCfg(helper, alg, **kwargs):
    '''
    Arguments:
         helper   -- AthMonitorCfgHelper(Old) instance
         alg      -- algorithm Configurable object returned from addAlgorithm
         kwargs  -- jo agruments
    '''
    path        = '/Pixel/Hits/'
    pathLowStat = '/Pixel/LumiBlock/'

