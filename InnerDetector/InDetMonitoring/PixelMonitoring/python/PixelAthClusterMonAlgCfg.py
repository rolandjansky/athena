#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthClusterMonAlgCfg.py
@brief Configuration of Pixel Monitoring Clusters, Tracks and Status Histograms for Run 3
'''

from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist, define2DProfPerFEHist, definePP0Histos
from PixelMonitoring.PixelAthMonitoringBase import define1DLayers, defineMapVsLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import define1DProfLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import layers, totcuts, xbinsem, xminsem, lumibinsx, ztotbinsy, ztotminsy
from PixelMonitoring.PixelAthMonitoringBase import addOnTrackTxt, addOnTrackToPath, fullDressTitle
from PixelMonitoring.PixelAthMonitoringBase import runtext, ReadingDataErrLabels


def PixelAthClusterMonAlgCfg(helper, alg, **kwargs):

    doOnline  = kwargs.get('doOnline',  False)
    doLumiBlock = kwargs.get('doLumiBlock', False)
    doFEPlots  = kwargs.get('doFEPlots',  False)

### begin status histograms

    path        = '/Pixel/Status/'
    pathLowStat = '/Pixel/LumiBlock/'

    histoGroupName = 'MapOfModulesStatus' 
    title = 'Modules Status (0=Active+Good, 1=Active+Bad, 2=Inactive)'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    title = 'Modules Status Reset (0=Active+Good, 1=Active+Bad, 2=Inactive)'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D', zmin=0, zmax=2, opt='kLBNHistoryDepth=2', histname='MapOfModulesStatusMon')

    if doFEPlots:
        histoGroupName = 'MapOfFEsStatus' 
        title = 'FEs Status (0=Active+Good, 1=Active+Bad, 2=Inactive)'
        define2DProfPerFEHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    if doLumiBlock:
        if not doFEPlots:
            histoGroupName = 'MapOfModulesStatus'
            title = 'Modules Status (0=Active+Good, 1=Active+Bad, 2=Inactive)'
            define2DProfHist(helper, alg, histoGroupName, title, pathLowStat, type='TProfile2D', lifecycle='lumiblock', histname='MapOfModulesStatusLB')
        else:
            histoGroupName = 'MapOfFEsStatus'
            title = 'FEs Status (0=Active+Good, 1=Active+Bad, 2=Inactive)'
            define2DProfPerFEHist(helper, alg, histoGroupName, title, pathLowStat, type='TProfile2D', lifecycle='lumiblock', histname='MapOfFEsStatusLB')

    histoGroupName = 'BadModulesPerLumi'
    title          = 'Number of bad modules (bad+active) per event per LB'
    yaxistext      = ';# modules/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    histoGroupName = 'DisabledModulesPerLumi'
    title          = 'Number of disabled modules per event per LB'
    yaxistext      = ';# modules/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

### end status histograms
### begin track histograms
    path        = '/Pixel/TrackOnTrack/'
    pathLowStat = '/Pixel/LumiBlockOnTrack/'

    histoGroupName = 'TSOSMeasurement' 
    title = 'TSOS of type Measurement'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = 'TSOSHole' 
    title = 'TSOS of type Hole'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = 'TSOSOutlier' 
    title = 'TSOS of type Outlier'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = 'HitEffAll'
    title          = 'hit efficiency'
    yaxistext      = ';hit efficiency'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    histoGroupName = 'HolesRatio' 
    title = 'Holes per track'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    histoGroupName = 'MissHitsRatio' 
    title = 'Hole+Outlier per track'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    trackGroup = helper.addGroup(alg, 'Track')
    varName = 'res_phi'
    title = fullDressTitle('Pixel Residual LocX(phi)', False, ';Residual LocX(phi)',';# measurements on track')
    varName += ';TrackResPhi'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-0.1, xmax=0.1)
    varName = 'res_eta'
    title = fullDressTitle('Pixel Residual LocY(eta)', False, ';Residual LocY(eta)',';# measurements on track')
    varName += ';TrackResEta'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-0.3, xmax=0.3)
    varName = 'pull_phi'
    title = fullDressTitle('Pixel Pull LocX(phi)', False, ';Pull LocX(phi)',';# measurements on track')
    varName += ';TrackPullsPhi'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-1.2, xmax=1.2)
    varName = 'pull_eta'
    title = fullDressTitle('Pixel Pull LocY(eta)', False, ';Pull LocY(eta)',';# measurements on track')
    varName += ';TrackPullsEta'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-2.0, xmax=2.0)
    varName = 'fit_chi2byndf'
    title = fullDressTitle('chi2/ndf of track', False, ';#chi^{2}/DoF',';# of tracks')
    varName += ';TrackChi2byNDF'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=50, xmin=-0., xmax=10.0)

    varName = 'pixclusmontool_lb,ntrks_per_event'
    title = fullDressTitle('Number of tracks per event per LB', False, ';lumi block', ';tracks/event')

    varName += ';TracksPerEvtPerLumi'
    trackGroup.defineHistogram(varName,
                                type='TProfile', path=path, title=title,
                                xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)

    varName = 'pixclusmontool_lb,npixhits_per_track'
    title   = fullDressTitle('Number of pixhits per track per LB', False, ';lumi block', ';number of hits')
    varName += ';NPixHitsPerTrackPerLumi'
    trackGroup.defineHistogram(varName,
                                type='TH2F', path=path, title=title, weight='npixhits_per_track_wgt',
                                xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                ybins=10, ymin=-0.5, ymax=9.5)

    if doOnline:
        varName = 'pixclusmontool_lb,npixhits_per_track'
        title   = fullDressTitle('Number of pixhits per track per LB for last 100LB', False, ';lumi block', ';number of hits')
        varName += ';NPixHitsPerTrackPerLumiLast100LB'
        trackGroup.defineHistogram(varName,
                                type='TH2F', path=path, title=title, weight='npixhits_per_track_wgt',
                                xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                ybins=10, ymin=-0.5, ymax=9.5, opt='kLBNHistoryDepth=100')

        histoGroupName = 'HolesRatio5min' 
        title = 'Holes per track reset every 5 min'
        define2DProfHist(helper, alg, 'HolesRatio', title, path, type='TProfile2D', zmin=0, zmax=1.1, opt='kLBNHistoryDepth=5', histname=histoGroupName)

        histoGroupName = 'MissHitsRatio5min' 
        title = 'Hole+Outlier per track reset every 5 min'
        define2DProfHist(helper, alg, 'MissHitsRatio', title, path, type='TProfile2D', zmin=0, zmax=1.1, opt='kLBNHistoryDepth=5', histname=histoGroupName)

    varName = 'trkdataread_err;ReadingTrackDataErr'
    title = 'Number of Track data reading errors;error type;# events'
    trackGroup.defineHistogram(varName,
                               type='TH1I', path=path, title=title,
                               xbins=len(ReadingDataErrLabels), xmin=-0.5, xmax=-0.5+len(ReadingDataErrLabels), xlabels=ReadingDataErrLabels)

### end track histograms
### begin cluster histograms
    path        = '/Pixel/Clusters'
    pathLowStat = '/Pixel/LumiBlock'
    pathTiming  = '/Pixel/Timing'

### begin cluster timing
###
    clusterGroup = { True: helper.addGroup(alg, "Cluster_OnTrack" ),
                     False: helper.addGroup(alg, "Cluster") }
    for ontrack in True,False:
        pathGroup = addOnTrackToPath(pathTiming, ontrack)
        varName = 'Cluster_LVL1A_lvl1a'
        title = fullDressTitle('Cluster Level 1 Accept', ontrack, ';LVL1A',';# clusters')
        varName += ';'+ addOnTrackTxt('ClusterLVL1A', ontrack)
        clusterGroup[ontrack].defineHistogram(varName, 
                                              type='TH1F', path=pathGroup, title=title,
                                              xbins=14, xmin=-1.5, xmax=12.5)

        histoGroupName = addOnTrackTxt('ClusterLVL1AMod', ontrack)
        title = addOnTrackTxt('Average cluster Level 1 Accept', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TProfile2D')

        histoGroupName = addOnTrackTxt('ClusterLVL1ASizeCut', ontrack)
        title = addOnTrackTxt('Average Size>1 Cluster Level 1 Accept', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TProfile2D')

        varName = 'Cluster_LVL1A_lvl1a' # re-using same variable
        histoGroupName = addOnTrackTxt('ClusterLVL1AToTCut', ontrack)
        xaxistext      = ';LVL1A'
        yaxistext      = ';# clusters/event'
        for idx, layer in enumerate(layers):
            groupname   = histoGroupName  + '_{0}'.format(layer)
            title = addOnTrackTxt('Cluster Level 1 Accept with ToT > {0}'.format(totcuts[idx]), ontrack, True)
            fulltitle   = title + ', {0}'.format(layer) + runtext + xaxistext + yaxistext
            layerGroup = helper.addGroup(alg, groupname)
            fullvarstring = varName + ';' + groupname
            layerGroup.defineHistogram(fullvarstring, 
                                        type='TH1F', path=pathGroup, title=fulltitle,
                                        xbins=14, xmin=-1.5, xmax=12.5)
###    
### end cluster timing
### begin cluster sizes
###
        if ontrack:
            pathGroup = addOnTrackToPath(path, ontrack)
            var1Name = addOnTrackTxt('ClusterSize', ontrack) + '_em'
            var2Name = addOnTrackTxt('ClusterSize', ontrack) + '_clsize'
            histoGroupName = addOnTrackTxt('ClusterGroupsizeVsEta', ontrack)
            xaxistext      = ';Module eta index'
            yaxistext      = ';# pixels/cluster'
            title = addOnTrackTxt('Number of pixels per cluster vs eta', ontrack, True)
            for idx, layer in enumerate(layers):
                groupname   = histoGroupName  + '_{0}'.format(layer)
                fulltitle   = title + ', {0}'.format(layer) + runtext + xaxistext + yaxistext
                layerGroup = helper.addGroup(alg, groupname)
                fullvarstring = var1Name + ',' + var2Name + ';' + groupname
                layerGroup.defineHistogram(fullvarstring, 
                                            type='TProfile', path=pathGroup, title=fulltitle,
                                            xbins=xbinsem[idx], xmin=xminsem[idx], xmax=xminsem[idx]+xbinsem[idx])

            histoGroupName = addOnTrackTxt('ClusterSizeMap', ontrack)
            title = addOnTrackTxt('Average cluster size map', ontrack, True)
            define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TProfile2D')

### 
### end cluster sizes
### begin cluster rates
###
        pathGroup = addOnTrackToPath(path, ontrack)

        varName = 'pixclusmontool_lb,ncls_per_event'
        title = fullDressTitle('Average number of pixel clusters per event per LB', ontrack, ';lumi block', ';# clusters/event')
        varName += ';'+ addOnTrackTxt('ClustersPerLumi', ontrack)
        clusterGroup[ontrack].defineHistogram(varName,
                                              type='TProfile', path=pathGroup, title=title,
                                              xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)

        histoGroupName = addOnTrackTxt('ClustersPerLumi', ontrack)
        title          = addOnTrackTxt('Average number of pixel clusters per event per LB', ontrack, True)
        yaxistext      = ';# clusters/event'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

        if ontrack:
            varName = 'pixclusmontool_lb,cls_frac_ontrack'
            title = fullDressTitle('Fraction pixel clusters on track per event per LB', False, ';lumi block', ';fraction clusters/event')
            varName += ';ClustersOnOffTrackPerLumi'
            clusterGroup[ontrack].defineHistogram(varName,
                                                  type='TProfile', path=pathGroup, title=title,
                                                  xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)


            histoGroupName = 'NumClustersPerTrackPerLumi'
            title          = addOnTrackTxt('Number of pixel clusters per track per event', ontrack, True)
            yaxistext      = ';avg #clusters/track/event'
            define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

        if doOnline:
            histoGroupName = addOnTrackTxt('ClusterMapMon', ontrack)
            title = addOnTrackTxt('Cluster map for monitoring', ontrack, True)
            define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TH2D', zmin=0, zmax=1e4, opt='kLBNHistoryDepth=2') #FIXME zmax value w/ high stat

### 
### end cluster rates
### begin cluster occupancy
###
        pathGroup = addOnTrackToPath(path, ontrack)

        histoGroupName = addOnTrackTxt('ClusterOccupancy', ontrack)
        title = addOnTrackTxt('Cluster occupancy', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TH2D')

        histoGroupName = addOnTrackTxt('ClusOccSizeCut', ontrack)
        title = addOnTrackTxt('Size>1 Cluster occupancy', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TH2D')
        if ontrack:
            histoGroupName = addOnTrackTxt('ClusterOccupancyPP0', ontrack)
            title = addOnTrackTxt('Average per module(FE) cluster occupancy reset every 5 min', ontrack, True)
            definePP0Histos(helper, alg, histoGroupName, title, pathGroup, opt='kLBNHistoryDepth=5')

        if doFEPlots:
            histoGroupName = addOnTrackTxt('ClusterFEOccupancy', ontrack) 
            title = addOnTrackTxt('Cluster occupancy per FE', ontrack, True)
            define2DProfPerFEHist(helper, alg, histoGroupName, title, pathGroup, type='TH2F')

        if doLumiBlock:
            pathGroup = addOnTrackToPath(pathLowStat, ontrack)
            if not doFEPlots:
                histoGroupName = addOnTrackTxt('ClusterOccupancyLB', ontrack)
                title = addOnTrackTxt('Cluster occupancy', ontrack, True)
                define2DProfHist(helper, alg, addOnTrackTxt('ClusterOccupancy', ontrack), title, pathGroup, type='TH2D', lifecycle='lumiblock', histname=histoGroupName)
            else:
                histoGroupName = addOnTrackTxt('ClusterFEOccupancyLB', ontrack)
                title = addOnTrackTxt('Cluster occupancy per FE', ontrack, True)
                define2DProfPerFEHist(helper, alg, addOnTrackTxt('ClusterFEOccupancy', ontrack), title, pathGroup, type='TH2F', lifecycle='lumiblock', histname=histoGroupName)


### 
### end cluster occupancy
### begin cluster ToT and charge
###
        if ontrack:
            pathGroup = addOnTrackToPath(path, ontrack)
            histoGroupName = addOnTrackTxt('ClusterToTxCosAlpha', ontrack)
            title = addOnTrackTxt('Cluster ToTxCosAlpha', ontrack, True)
            define1DLayers(helper, alg, histoGroupName, title, pathGroup, ';ToT [BC]', ';# clusters', xbins=[300], xmins=[-0.5])
            if not doOnline:
                histoGroupName = addOnTrackTxt('ClusterQxCosAlpha', ontrack)
                title = addOnTrackTxt('Cluster Q normalized', ontrack, True)
                define1DLayers(helper, alg, histoGroupName, title, pathGroup, ';Charge [e]', ';# clusters', xbins=[70], xmins=[-0.5], binsizes=[3000.])
            else:
                title = addOnTrackTxt('Zoomed Cluster ToTxCosAlpha per LB', ontrack, True)
                defineMapVsLumiLayers(helper, alg, histoGroupName, title, pathGroup, ';lumi block', ';ToT [BC]', ybins=ztotbinsy, ymins=ztotminsy, histname='ZoomedClusterToTxCosAlphaPerLumi')

### 
### end cluster ToT and charge

        if not ontrack:
            varName = 'clsdataread_err;ReadingClusterDataErr'
            title = 'Number of Cluster data reading errors;error type;# events'
            trackGroup.defineHistogram(varName,
                                       type='TH1I', path=pathGroup, title=title,
                                       xbins=len(ReadingDataErrLabels), xmin=-0.5, xmax=-0.5+len(ReadingDataErrLabels), xlabels=ReadingDataErrLabels)

### end cluster histograms
