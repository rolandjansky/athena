#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthHitMonAlgCfg.py
@brief Configuration of Pixel Hit Monitoring Histograms for Run 3
'''

from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist, definePP0Histos, getLayerGroup
from PixelMonitoring.PixelAthMonitoringBase import define1DProfLumiLayers, define2DProfPerFEHist
from PixelMonitoring.PixelAthMonitoringBase import define1DLayers
from PixelMonitoring.PixelAthMonitoringBase import layers, lumibinsx, bcidbinsx
from PixelMonitoring.PixelAthMonitoringBase import addOnTrackTxt, addOnTrackToPath, fullDressTitle
from PixelMonitoring.PixelAthMonitoringBase import runtext, ReadingDataErrLabels

def PixelAthHitMonAlgCfg(helper, alg, **kwargs):
    '''
    Arguments:
         helper   -- AthMonitorCfgHelper(Old) instance
         alg      -- algorithm Configurable object returned from addAlgorithm
         kwargs   -- jo agruments
    '''
    doOnline  = kwargs.get('doOnline',  False)
    doLumiBlock = kwargs.get('doLumiBlock', False)
    doFEPlots  = kwargs.get('doFEPlots',  False)

    ontrack = False

    path        = '/Pixel/Hits/'
    pathLowStat = '/Pixel/LumiBlock/'
    pathGroup   = addOnTrackToPath(path, ontrack)

    hitGroup = helper.addGroup(alg, 'Hit')

    varName = 'hitdataread_err;ReadingHitDataErr'
    title = 'Number of Hit data reading errors;error type;# events'
    hitGroup.defineHistogram(varName,
                             type='TH1I', path=pathGroup, title=title,
                             xbins=len(ReadingDataErrLabels), xmin=-0.5, xmax=-0.5+len(ReadingDataErrLabels), xlabels=ReadingDataErrLabels)

    varName = 'pixhitsmontool_lb,nhits_per_event'
    title = fullDressTitle('Average number of pixel hits per event per LB', ontrack, ';lumi block', ';# hits/event')
    varName += ';'+ addOnTrackTxt('HitsPerLumi', ontrack)
    hitGroup.defineHistogram(varName,
                             type='TProfile', path=pathGroup, title=title,
                             xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)
    
    histoGroupName = addOnTrackTxt('HitsPerLumi', ontrack)
    title          = addOnTrackTxt('Average number of pixel hits per event per LB', ontrack, True)
    yaxistext      = ';# hits/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

    histoGroupName = addOnTrackTxt('AvgOccActivePerLumi', ontrack)
    title          = addOnTrackTxt('Average pixel occupancy for active modules per event per LB', ontrack, True)
    yaxistext      = ';# hits/pixel/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

    histoGroupName = addOnTrackTxt('AvgOccGoodPerLumi', ontrack)
    title          = addOnTrackTxt('Average pixel occupancy for good modules per event per LB', ontrack, True)
    yaxistext      = ';# hits/pixel/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

    if doOnline:
        histoGroupName = addOnTrackTxt('AvgOccRatioToIBLPerLumi', ontrack)
        title          = addOnTrackTxt('Relative to IBL pixel occupancy per event per LB', ontrack, True)
        yaxistext      = ';occ. ratio to IBL'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

    histoGroupName = 'HitToT'
    title = 'Hit ToT'
    define1DLayers(helper, alg, histoGroupName, title, pathGroup, ';ToT [BC]', ';# hits', xbins=[300]*5+[20]*3, xmins=[-0.5]*8, binsizes=[1.0]*8)

    histoGroupName = 'HitMap' 
    title = 'hit map'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    if doFEPlots:
        histoGroupName = 'HitFEMap' 
        title = 'hit map per FE'
        define2DProfPerFEHist(helper, alg, histoGroupName, title, path, type='TH2F')

    if doLumiBlock:
        if not doFEPlots:
            define2DProfHist(helper, alg, histoGroupName, title, pathLowStat, type='TH2F', lifecycle='lumiblock', histname='HitMapLB')
        else:
            define2DProfPerFEHist(helper, alg, histoGroupName, title, pathLowStat, type='TH2F', lifecycle='lumiblock', histname='HitFEMapLB')



    histoname = 'AvgOccPerBCID'
    for layer in layers:
        title      = 'Average pixel occupancy per BCID, {0}'.format(layer) + runtext + ';BCID;# hits/pixel/event'
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstr = 'pixhitsmontool_bcid'
        fullvarstr+= ',{0}_{1}'.format(histoname, 'val')
        fullvarstr+= ';' + histoname + '_{0}'.format(layer)
        layerGroup.defineHistogram(fullvarstr, 
                                   type='TProfile', path=pathGroup, title=title,
                                   xbins=bcidbinsx, xmin=-0.5, xmax=-0.5+bcidbinsx)

    histoGroupName = 'HitOccupancyPP0'
    title = 'Average per module(FE) hit occupancy reset every 5 min'
    definePP0Histos(helper, alg, histoGroupName, title, path=pathGroup, opt='kLBNHistoryDepth=5')

    histoGroupName ='OccupancyPerPixelEvent'
    title = '#hits / pixel / event'
    define2DProfHist(helper, alg, histoGroupName, title, path=pathGroup, type='TProfile2D', zmin=0, zmax=1.0, opt='kLBNHistoryDepth=2')

###
### begin hit timing
###
    pathTiming  = '/Pixel/Timing'
    varName = 'Hit_LVL1A_pixel'
    title = fullDressTitle('Hit Level 1 Accept', False, ';Level 1 Accept',';# hits')
    varName += ';HitLVL1A'
    hitGroup.defineHistogram(varName, 
                             type='TH1F', path=pathTiming, title=title,
                             xbins=14, xmin=-1.5, xmax=12.5)
    varName = 'Hit_LVL1A_layer'
    for layer in layers:
        histoname  = 'HitLVL1A_{0}'.format(layer)
        title      = 'Hit Level 1 Accept, {0}'.format(layer) + runtext + ';Level 1 Accept;# hits'
        layerGroup = getLayerGroup(helper, alg, layer)
        fullvarstr = varName + ';' + histoname
        layerGroup.defineHistogram(fullvarstr,
                                   type='TH1F', path=pathTiming, title=title,
                                   xbins=14, xmin=-1.5, xmax=12.5)
        if doLumiBlock:
            histonameLB  = 'HitLVL1ALB_{0}'.format(layer)
            titleLB      = 'Hit Level 1 Accept lowstat, {0}'.format(layer) + runtext + ';Level 1 Accept;# hits'
            fullvarstrLB = varName + ';' + histonameLB
            layerGroup.defineHistogram(fullvarstrLB,
                                       type='TH1F', path=pathLowStat, title=titleLB,
                                       xbins=14, xmin=-1.5, xmax=12.5, duration='lowStat')
            
###
### end hit timing
###
