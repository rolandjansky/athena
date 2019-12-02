#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthClusterMonAlgCfg.py
@brief Configuration of Pixel Monitoring Clusters, Tracks and Status Histograms for Run 3
'''

from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist, definePP0Histos
from PixelMonitoring.PixelAthMonitoringBase import define1DLayers
from PixelMonitoring.PixelAthMonitoringBase import define1DProfLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import layers, totcuts, xbinsem, xminsem, lumibinsx
from PixelMonitoring.PixelAthMonitoringBase import addOnTrackTxt, addOnTrackToPath, fullDressTitle
from RecExConfig.AutoConfiguration import GetRunNumber
runNumber = GetRunNumber()
runtext = ' (Run ' + str(runNumber) + ')'


def PixelAthClusterMonAlgCfg(helper, alg, **kwargs):

    doOnline  = kwargs.get('doOnline',  False)
    doLumiBlock = kwargs.get('doLumiBlock', False)

### begin status histograms

    path        = '/Pixel/Status/'
    pathLowStat = '/Pixel/LumiBlock/'

    histoGroupName = 'Map_Of_Modules_Status' 
    title = 'Modules Status (0=Active+Good 1=Active+Bad 2=Inactive)'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    title = 'Modules Status Reset (0=Active+Good 1=Active+Bad 2=Inactive)'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D', zmin=0, zmax=2, opt='kLBNHistoryDepth=2', histname='Map_Of_Modules_Status_Mon')

    if doLumiBlock:
        title = 'Modules Status (0=Active+Good 1=Active+Bad 2=Inactive)'
        define2DProfHist(helper, alg, histoGroupName, title, pathLowStat, type='TProfile2D', lifecycle='lowStat', histname='Map_Of_Modules_Status_LB')

    histoGroupName = 'BadModules_per_lumi'
    title          = 'Number of bad modules (bad+active) per event per LB'
    yaxistext      = ';# modules/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    histoGroupName = 'DisabledModules_per_lumi'
    title          = 'Number of disabled modules per event per LB'
    yaxistext      = ';# modules/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

### end status histograms
### begin track histograms
    path        = '/Pixel/TrackOnTrack/'
    pathLowStat = '/Pixel/LumiBlockOnTrack/'

    histoGroupName = 'TSOS_Measurement' 
    title = 'TSOS of type Measurement'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = 'TSOS_Hole' 
    title = 'TSOS of type Hole'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = 'TSOS_Outlier' 
    title = 'TSOS of type Outlier'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = 'HitEff_all'
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
    varName += ';Track_res_phi'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-0.1, xmax=0.1)
    varName = 'res_eta'
    title = fullDressTitle('Pixel Residual LocY(eta)', False, ';Residual LocY(eta)',';# measurements on track')
    varName += ';Track_res_eta'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-0.3, xmax=0.3)
    varName = 'pull_phi'
    title = fullDressTitle('Pixel Pull LocX(phi)', False, ';Pull LocX(phi)',';# measurements on track')
    varName += ';Track_pulls_phi'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-1.2, xmax=1.2)
    varName = 'pull_eta'
    title = fullDressTitle('Pixel Pull LocY(eta)', False, ';Pull LocY(eta)',';# measurements on track')
    varName += ';Track_pulls_eta'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=100, xmin=-2.0, xmax=2.0)
    varName = 'fit_chi2byndf'
    title = fullDressTitle('chi2/ndf of track', False, ';#chi^{2}/DoF',';# of tracks')
    varName += ';Track_chi2byndf'
    trackGroup.defineHistogram(varName,
                                type='TH1F', path=path, title=title,
                                xbins=50, xmin=-0., xmax=10.0)

    varName = 'pixclusmontool_lb,ntrks_per_event'
    title = fullDressTitle('Number of tracks per event per LB', False, ';lumi block', ';tracks/event')
    varName += ';tracksPerEvt_per_lumi'
    trackGroup.defineHistogram(varName,
                                type='TProfile', path=path, title=title,
                                xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)

    varName = 'pixclusmontool_lb,npixhits_per_track'
    title   = fullDressTitle('Number of pixhits per track per LB', False, ';lumi block', ';number of hits')
    varName += ';NPixhits_per_track_lumi'
    trackGroup.defineHistogram(varName,
                                type='TH2F', path=path, title=title, weight='npixhits_per_track_wgt',
                                xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx,
                                ybins=10, ymin=-0.5, ymax=9.5)

    if doOnline:
        histoGroupName = 'HolesRatio_5min' 
        title = 'Holes per track reset every 5 min'
        define2DProfHist(helper, alg, 'HolesRatio', title, path, type='TProfile2D', zmin=0, zmax=1.1, opt='kLBNHistoryDepth=5', histname=histoGroupName)

        histoGroupName = 'MissHitsRatio_5min' 
        title = 'Hole+Outlier per track reset every 5 min'
        define2DProfHist(helper, alg, 'MissHitsRatio', title, path, type='TProfile2D', zmin=0, zmax=1.1, opt='kLBNHistoryDepth=5', histname=histoGroupName)

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
        title = fullDressTitle('Cluster Level 1 Accept', ontrack, ';LVL1A',';# clusters/event')
        varName += ';'+ addOnTrackTxt('Cluster_LVL1A', ontrack)
        clusterGroup[ontrack].defineHistogram(varName, 
                                              type='TH1F', path=pathGroup, title=title,
                                              xbins=14, xmin=-1.5, xmax=12.5)

        histoGroupName = addOnTrackTxt('Cluster_LVL1A_Mod', ontrack)
        title = addOnTrackTxt('Average cluster Level 1 Accept', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TProfile2D')

        histoGroupName = addOnTrackTxt('Cluster_LVL1A_SizeCut', ontrack)
        title = addOnTrackTxt('Average Size>1 Cluster Level 1 Accept', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TProfile2D')

        varName = 'Cluster_LVL1A_lvl1a' # re-using same variable
        histoGroupName = addOnTrackTxt('Cluster_LVL1A_ToTCut', ontrack)
        xaxistext      = ';LVL1A'
        yaxistext      = ';# clusters/event'
        for idx, layer in enumerate(layers):
            groupname   = histoGroupName  + '_{0}'.format(layer)
            title = addOnTrackTxt('Cluster Level 1 Accept with ToT > {0}'.format(totcuts[idx]), ontrack, True)
            fulltitle   = title + ' {0}'.format(layer) + runtext + xaxistext + yaxistext
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
            var1Name = addOnTrackTxt('Cluster_Size', ontrack) + '_em'
            var2Name = addOnTrackTxt('Cluster_Size', ontrack) + '_clsize'
            histoGroupName = addOnTrackTxt('Cluster_groupsize_vs_eta', ontrack)
            xaxistext      = ';Module eta index'
            yaxistext      = ';# pixels/cluster'
            title = addOnTrackTxt('Number of pixels per cluster vs eta', ontrack, True)
            for idx, layer in enumerate(layers):
                groupname   = histoGroupName  + '_{0}'.format(layer)
                fulltitle   = title + ' {0}'.format(layer) + runtext + xaxistext + yaxistext
                layerGroup = helper.addGroup(alg, groupname)
                fullvarstring = var1Name + ',' + var2Name + ';' + groupname
                layerGroup.defineHistogram(fullvarstring, 
                                            type='TProfile', path=pathGroup, title=fulltitle,
                                            xbins=xbinsem[idx], xmin=xminsem[idx], xmax=xminsem[idx]+xbinsem[idx])

            histoGroupName = addOnTrackTxt('Cluster_Size_Map', ontrack)
            title = addOnTrackTxt('Average cluster size map', ontrack, True)
            define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TProfile2D')

### 
### end cluster sizes
### begin cluster rates
###
        pathGroup = addOnTrackToPath(path, ontrack)

        varName = 'pixclusmontool_lb,ncls_per_event'
        title = fullDressTitle('Average number of pixel clusters per event per LB', ontrack, ';lumi block', ';clusters/event')
        varName += ';'+ addOnTrackTxt('Clusters_per_lumi', ontrack)
        clusterGroup[ontrack].defineHistogram(varName,
                                              type='TProfile', path=pathGroup, title=title,
                                              xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)

        histoGroupName = addOnTrackTxt('Clusters_per_lumi', ontrack)
        title          = addOnTrackTxt('Average number of pixel clusters per event per LB', ontrack, True)
        yaxistext      = ';# clusters/event'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

        if ontrack:
            varName = 'pixclusmontool_lb,cls_frac_ontrack'
            title = fullDressTitle('Fraction pixel clusters on track per event per LB', False, ';lumi block', ';fraction clusters/event')
            varName += ';ClustersOnOffTrack_per_lumi'
            clusterGroup[ontrack].defineHistogram(varName,
                                                  type='TProfile', path=pathGroup, title=title,
                                                  xbins=lumibinsx, xmin=-0.5, xmax=-0.5+lumibinsx)


            histoGroupName = 'num_clusters_per_track_per_lumi'
            title          = addOnTrackTxt('Number of pixel clusters per track per event', ontrack, True)
            yaxistext      = ';avg #clusters/track/event'
            define1DProfLumiLayers(helper, alg, histoGroupName, title, pathGroup, yaxistext, type='TProfile')

        if doOnline:
            histoGroupName = addOnTrackTxt('ClusterMap_Mon', ontrack)
            title = addOnTrackTxt('Cluster map for monitoring', ontrack, True)
            define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TH2D', zmin=0, zmax=1e4, opt='kLBNHistoryDepth=2') #FIXME zmax value w/ high stat

### 
### end cluster rates
### begin cluster occupancy
###
        pathGroup = addOnTrackToPath(path, ontrack)

        histoGroupName = addOnTrackTxt('Cluster_Occupancy', ontrack)
        title = addOnTrackTxt('Cluster occupancy', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TH2D')

        histoGroupName = addOnTrackTxt('Clus_Occ_SizeCut', ontrack)
        title = addOnTrackTxt('Size>1 Cluster occupancy', ontrack, True)
        define2DProfHist(helper, alg, histoGroupName, title, pathGroup, type='TH2D')
        if ontrack:
            histoGroupName = addOnTrackTxt('Cluster_Occupancy_PP0', ontrack)
            title = addOnTrackTxt('Average per module(FE) cluster occupancy reset every 5 min', ontrack, True)
            definePP0Histos(helper, alg, histoGroupName, title, pathGroup, opt='kLBNHistoryDepth=5')


        if doLumiBlock:
            pathGroup = addOnTrackToPath(pathLowStat, ontrack)
            histoGroupName = addOnTrackTxt('Cluster_Occupancy_LB', ontrack)
            title = addOnTrackTxt('Cluster occupancy lowStat', ontrack, True)
            define2DProfHist(helper, alg, addOnTrackTxt('Cluster_Occupancy', ontrack), title, pathGroup, type='TH2D', lifecycle='lowStat', histname=histoGroupName)


### 
### end cluster occupancy
### begin cluster ToT and charge
###
        if ontrack:
            pathGroup = addOnTrackToPath(path, ontrack)
            histoGroupName = addOnTrackTxt('Cluster_ToTxCosAlpha', ontrack)
            title = addOnTrackTxt('Cluster ToTxCosAlpha', ontrack, True)
            define1DLayers(helper, alg, histoGroupName, title, pathGroup, ';ToT [BC]', ';# clusters', xbins=[300], xmins=[-0.5])
            if (not doOnline):
                histoGroupName = addOnTrackTxt('Cluster_QxCosAlpha', ontrack)
                title = addOnTrackTxt('Cluster Q normalized', ontrack, True)
                define1DLayers(helper, alg, histoGroupName, title, pathGroup, ';Charge [e]', ';# clusters', xbins=[70], xmins=[-0.5], binsizes=[3000.])


### 
### end cluster ToT and charge



### end cluster histograms
