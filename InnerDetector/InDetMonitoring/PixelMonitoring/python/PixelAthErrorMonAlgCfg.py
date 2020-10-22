#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthErrorMonAlgCfg.py
@brief Configuration of Pixel Error Monitoring Histograms for Run 3
'''
from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist
from PixelMonitoring.PixelAthMonitoringBase import defineMapVsLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import define1DProfLumiLayers
from PixelMonitoring.PixelAthMonitoringBase import layers
from PixelMonitoring.PixelAthMonitoringBase import fullDressTitle
from PixelMonitoring.PixelAthMonitoringBase import errbbinsy, errbminsy, errbbsizy, errtbinsy
from PixelMonitoring.PixelAthMonitoringBase import ErrBitLabels, ErrCatRODModLabels, ErrCatLabels, ErrCatRODModLabelsNorm, ErrCatLabelsNorm
from PixelMonitoring.PixelAthMonitoringBase import ErrStateLabelsFEI3, ErrStateLabelsFEI4

def PixelAthErrorMonAlgCfg(helper, alg, **kwargs):
    '''
    Arguments:
         helper  -- AthMonitorCfgHelper(Old) instance
         alg     -- algorithm Configurable object returned from addAlgorithm
         kwargs  -- jo agruments
    '''
    path        = '/Pixel/Errors/'
    pathExpert  = '/Pixel/ErrorsExpert/'

    errorGroup = helper.addGroup(alg, 'Error')

    histoGroupName = 'errors_per_lumi'
    title          = 'Average Total Errors'
    yaxistext      = ';# errors/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    histoGroupName = 'ErrorBit_per_lumi'
    title          = 'Average Errors by Error Bits'
    defineMapVsLumiLayers(helper, alg, histoGroupName, title, path, ';lumi block', ';error bit', ybins=errbbinsy, ymins=errbminsy, binsizes=errbbsizy, ylabels=ErrBitLabels, type='TProfile2D')

    histoGroupName = 'ErrorCatRODMod_per_lumi'
    title          = 'Average Errors by Error Types'
    ylabels        = [[i[1] for i in ErrCatRODModLabels]]*len(layers)
    defineMapVsLumiLayers(helper, alg, histoGroupName, title, path, ';lumi block', ';error type', ybins=errtbinsy, ymins=errbminsy, binsizes=errbbsizy, ylabels=ylabels, type='TProfile2D')

    for i, cat in enumerate(ErrCatRODModLabels):
        if i==4: #other histograms are covered by ErrCatLabels below
            break
        histoGroupName = cat[0]
        title          = "Total "+cat[1]
        define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    for cat in ErrCatLabels:
        histoGroupName = cat[0]
        title          = "Total "+cat[1]
        define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    yaxistext      = ';# errors/module/event'
    for i, cat in enumerate(ErrCatRODModLabelsNorm):
        histoGroupName = cat
        title          = "Average "+ErrCatRODModLabels[i][1]+" per Module"
        define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    for i, cat in enumerate(ErrCatLabelsNorm):
        histoGroupName = cat
        title          = "Average "+ErrCatLabels[i][1]+" per Module"
        define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    histoGroupName = "femcc_errorwords"
    title          = "Average FE/MCC Error Words"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    histoGroupName = "Errors_LB"
    title          = "Errors"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F', lifecycle='lowStat')

    histoGroupName = "Errors_ModSync_LB"
    title          = "Errors_ModSync"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F', lifecycle='lowStat')

    histoGroupName = "Errors_RODSync_LB"
    title          = "Errors_RODSync"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F', lifecycle='lowStat')

    for state in ErrStateLabelsFEI3:
        histoGroupName = state[0]+"_Map"
        title          = state[1]+" per event per LB"
        define2DProfHist(helper, alg, histoGroupName, title, pathExpert, type='TH2F')
        histoGroupName = state[0]+"_per_lumi"
        title          = 'Average '+state[1]
        yaxistext      = ';# errors/event'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathExpert, yaxistext, type='TProfile')

    for state in ErrStateLabelsFEI4:
        histoGroupName = state[0]+"_Map"
        title          = state[1]+" per event per LB"
        define2DProfHist(helper, alg, histoGroupName, title, pathExpert, type='TH2F')
        histoGroupName = state[0]+"_per_lumi"
        title          = 'Average '+state[1]
        yaxistext      = ';# errors/event'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathExpert, yaxistext, type='TProfile')

    varName = 'ServiceRecord_val'
    title = fullDressTitle('IBL ServiceRecord Unweighted', False, ';SR',';# errors/event')
    varName += ';ServiceRecord_Unweighted_IBL'
    errorGroup.defineHistogram(varName, 
                               type='TH1F', path=pathExpert, title=title,
                               xbins=40, xmin=-0.5, xmax=39.5)
    varName = 'ServiceRecord_val'
    title = fullDressTitle('IBL ServiceRecord Weighted with Payload', False, ';SR',';# errors/event')
    varName += ';ServiceRecord_Weighted_IBL'
    errorGroup.defineHistogram(varName, weight='ServiceRecord_wgt',
                               type='TH1F', path=pathExpert, title=title,
                               xbins=40, xmin=-0.5, xmax=39.5)
