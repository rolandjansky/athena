#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthErrorMonToolCfg.py
@brief Configuration of Pixel Error Monitoring Histograms for Run 3
'''
from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist
from PixelMonitoring.PixelAthMonitoringBase import define1DLayers
from PixelMonitoring.PixelAthMonitoringBase import define1DProfLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import layers, totcuts, xbinsem, xminsem, lumibinsx
from PixelMonitoring.PixelAthMonitoringBase import addOnTrackTxt, fullDressTitle
from RecExConfig.AutoConfiguration import GetRunNumber
runNumber = GetRunNumber()
runtext = ' (Run ' + str(runNumber) + ')'

def PixelAthErrorMonToolCfg(helper, alg, **kwargs):
    '''
    Arguments:
         helper  -- AthMonitorCfgHelper(Old) instance
         alg     -- algorithm Configurable object returned from addAlgorithm
         kwargs  -- jo agruments
    '''
    doOnline  = kwargs.get('doOnline',  False)

    path        = '/Pixel/Errors/'
    pathLowStat = '/Pixel/LumiBlock/'

    histoGroupName = 'errors_per_lumi'
    title          = 'Average Total Errors'
    yaxistext      = ';# errors/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')
