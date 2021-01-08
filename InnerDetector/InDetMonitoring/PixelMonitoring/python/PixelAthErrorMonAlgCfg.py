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
from PixelMonitoring.PixelAthMonitoringBase import layers, fei3layers, fei4layers
from PixelMonitoring.PixelAthMonitoringBase import fullDressTitle
from PixelMonitoring.PixelAthMonitoringBase import errbbinsy, errbminsy, errbbsizy, errtbinsy
from PixelMonitoring.PixelAthMonitoringBase import ErrCatRODModLabels, ErrCatLabels, ErrCatRODModLabelsNorm, ErrCatLabelsNorm
from PixelMonitoring.PixelAthMonitoringBase import ErrStateLabelsFEI3, ErrStateLabelsFEI4

def PixelAthErrorMonAlgCfg(helper, alg, **kwargs):
    '''
    Arguments:
         helper  -- AthMonitorCfgHelper(Old) instance
         alg     -- algorithm Configurable object returned from addAlgorithm
         kwargs  -- jo agruments
    '''
    doLumiBlock = kwargs.get('doLumiBlock', False)
    path        = '/Pixel/Errors/'
    pathExpert  = '/Pixel/ErrorsExpert/'
    pathLowStat = '/Pixel/LumiBlock/'

    errorGroup = helper.addGroup(alg, 'Error')

    histoGroupName = 'ErrorsPerLumi'
    title          = 'Average Total Errors'
    yaxistext      = ';# errors/event'
    define1DProfLumiLayers(helper, alg, histoGroupName, title, path, yaxistext, type='TProfile')

    histoGroupName = 'ErrorStatePerLumi'
    title          = 'Average Errors by Error States'
    ylabels        = [[i[1] for i in ErrStateLabelsFEI3]]*len(layers)
    defineMapVsLumiLayers(helper, alg, histoGroupName, title, path, ';lumi block', ';error state', ybins=errbbinsy, ymins=errbminsy, binsizes=errbbsizy, ylabels=ylabels, type='TProfile2D', onlylayers=fei3layers)
    ylabels        = [[i[1] for i in ErrStateLabelsFEI4]]*len(layers)
    defineMapVsLumiLayers(helper, alg, histoGroupName, title, path, ';lumi block', ';error state', ybins=errbbinsy, ymins=errbminsy, binsizes=errbbsizy, ylabels=ylabels, type='TProfile2D', onlylayers=fei4layers)

    histoGroupName = 'ErrorCatRODModPerLumi'
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

    histoGroupName = "FEMCCErrorwords"
    title          = "Average FE/MCC Error Words"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D', onlylayers=fei3layers)

    histoGroupName = "TruncatedROBErrors"
    title          = "Truncated ROB Errors"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    histoGroupName = "MaskedROBErrors"
    title          = "Masked ROB Errors"
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TH2F')

    if doLumiBlock:
        histoGroupName = "ErrorsLB"
        title          = "Errors"
        define2DProfHist(helper, alg, histoGroupName, title, pathLowStat, type='TH2F', doWeight=True, lifecycle='lowStat')

        histoGroupName = "ErrorsModSyncLB"
        title          = "Errors_ModSync"
        define2DProfHist(helper, alg, histoGroupName, title, pathLowStat, type='TH2F', lifecycle='lowStat')

        histoGroupName = "ErrorsRODSyncLB"
        title          = "Errors_RODSync"
        define2DProfHist(helper, alg, histoGroupName, title, pathLowStat, type='TH2F', lifecycle='lowStat')

    for state in ErrStateLabelsFEI3:
        histoGroupName = state[0]+"Map"
        title          = state[1]+" Errors per event per LB"
        define2DProfHist(helper, alg, histoGroupName, title, pathExpert, type='TH2F', doWeight=True)
        histoGroupName = state[0]+"PerLumi"
        title          = 'Average '+state[1]+" Errors per event per LB"
        yaxistext      = ';# errors/module/event'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathExpert, yaxistext, type='TProfile', onlylayers=fei3layers)

    for state in ErrStateLabelsFEI4:
        histoGroupName = state[0]+"Map"
        title          = state[1]+" Errors per event per LB"
        define2DProfHist(helper, alg, histoGroupName, title, pathExpert, type='TH2F', doWeight=True)
        histoGroupName = state[0]+"PerLumi"
        title          = 'Average '+state[1]+" Errors per event per LB"
        yaxistext      = ';# errors/module/event'
        define1DProfLumiLayers(helper, alg, histoGroupName, title, pathExpert, yaxistext, type='TProfile', onlylayers=fei4layers)

    varName = 'ServiceRecord_val'
    title = fullDressTitle('IBL ServiceRecord Unweighted', False, ';SR',';# errors')
    varName += ';ServiceRecordUnweighted_IBL'
    errorGroup.defineHistogram(varName, 
                               type='TH1F', path=pathExpert, title=title,
                               xbins=32, xmin=-0.5, xmax=31.5)
    varName = 'ServiceRecord_val'
    title = fullDressTitle('IBL ServiceRecord Weighted with Payload', False, ';SR',';# errors')
    varName += ';ServiceRecordWeighted_IBL'
    errorGroup.defineHistogram(varName, weight='ServiceRecord_wgt',
                               type='TH1F', path=pathExpert, title=title,
                               xbins=32, xmin=-0.5, xmax=31.5)
 
