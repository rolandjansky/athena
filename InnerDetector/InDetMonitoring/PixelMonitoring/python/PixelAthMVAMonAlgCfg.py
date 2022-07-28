#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

'''
@file PixelAthMVAMonAlgCfg.py
@brief Configuration of Pixel MVA Monitoring Histograms for Run 3
'''

from PixelMonitoring.PixelAthMonitoringBase import define2DProfHist, ModulesHead
from PixelMonitoring.PixelAthMonitoringBase import LabelX, LabelY, baselayers
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags ### test of 100LB histograms

def PixelAthMVAMonAlgCfg(helper, alg, **kwargs):
    '''
    Arguments:
         helper   -- AthMonitorCfgHelper(Old) instance
         alg      -- algorithm Configurable object returned from addAlgorithm
         kwargs   -- jo agruments
    '''

    dumpTree = kwargs.get('dumpTree', False)
    path     = '/Pixel/MVA/'

    forceOnline = not athenaCommonFlags.isOnline

    if dumpTree:
        mvaGroup = helper.addGroup(alg, 'MVA')
        mvaGroup.defineTree('pixmvamontool_lb,status_vec,\
holes_vec,outliers_vec,meas_vec,\
clsontrkf_vec,clsontrk_vec,clsofftrk_vec,clsall_vec,\
clsontrksize_vec,clsontrkrowsize_vec,clsontrkcolsize_vec,\
clsofftrksize_vec,clsofftrkrowsize_vec,clsofftrkcolsize_vec,\
trkalpha_vec,trkchi2byndf_vec,trknpixdead_vec,trknblayerhits_vec,\
clsontrktot_vec,clsofftrktot_vec,mva_vec;\
vartree', path=path, treedef='pixmvamontool_lb/i:status_vec/vector<float>\
:h_vec/vector<float>:o_vec/vector<float>:m_vec/vector<float>\
:clsontrkf_vec/vector<float>:clsontrk_vec/vector<float>:clsofftrk_vec/vector<float>:clsall_vec/vector<float>\
:clsontrksize_vec/vector<float>:clsontrkrowsize_vec/vector<float>:clsontrkcolsize_vec/vector<float>\
:clsofftrksize_vec/vector<float>:clsofftrkrowsize_vec/vector<float>:clsofftrkcolsize_vec/vector<float>\
:trkalpha_vec/vector<float>:trkchi2byndf_vec/vector<float>\
:trknpixdead_vec/vector<float>:trknblayerhits_vec/vector<float>\
:clsontrktot_vec/vector<float>:clsofftrktot_vec/vector<float>:mva_vec/vector<float>')

    histoGroupName = 'BDTWeights'
    title = 'BDT score'
    define2DProfHist(helper, alg, histoGroupName, title, path, type='TProfile2D')

    histoGroupName = 'BDTWeightsReset'
    title = 'BDT score reset every 10 LB'
    define2DProfHist(helper, alg, 'BDTWeights', title, path, type='TProfile2D', opt='kLBNHistoryDepth=10', histname=histoGroupName)

    if forceOnline: athenaCommonFlags.isOnline = True
    xbins = 10 # only last 10LB will be shown and used
    fullvarstring  = 'pixmvamontool_lb, BDTWeights_val' #re-use monitoring variables
    modArray = []
    for i, layer in enumerate(baselayers):
        hnames = []
        for ix, x in enumerate(LabelX[i]):
            for y in LabelY[i]:
                hname  = ModulesHead[i] 
                if 'Disk' in x:
                    hname += str(ix+1)
                    hname+= '_' + y
                    if i==0:
                        hname += 'A'
                    else:
                        hname += 'C'
                elif layer=='IBL':
                    #A1_0 -> S0_M1A
                    newx = 'S'+ x[3] + x[2] + 'M' + x[1] + x[0]
                    #S14 -> B14
                    newy = 'B'+ y[1:]
                    hname += '_' + newy + '_' + newx
                else:
                    hname += '_' + y + '_' + x
                hnames.append(hname)
        modArray.append(helper.addArray([len(hnames)], alg, layer, path+'Modules/'))
        for postfix, tool in modArray[i].Tools.items():
            hname_idx = int( postfix.split('_').pop() )
            hname = hnames[hname_idx]
            tool.defineHistogram(fullvarstring + ';' + hname, type='TProfile', path=layer, 
                                        title='BDT score for last 100LB ' + hname + ';lumi block;<BDT score>',
                                        xbins=xbins, xmin=-0.5, xmax=-0.5+xbins, opt='kLive={0}'.format(xbins))
    if forceOnline: athenaCommonFlags.isOnline = False
'''
This function configures 1D (Profile) vs lumi histograms for Pixel(IBL) modules(FEs).
'''
