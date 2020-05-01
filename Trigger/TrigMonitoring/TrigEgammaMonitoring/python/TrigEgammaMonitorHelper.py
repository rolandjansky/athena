#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigEgammaMonitoringConfigRun3.py
@author D. Maximov
@date 2019-07-08
@brief Run 3 configuration builder. Histograms definitions taken from TrigEgammaPlotTool
'''



class  THWrapper(object):
    def __init__(self, name):
        self.name = name
        self.kwargs = {}

    def __setitem__(self, key, value):
        self.kwargs[key] = value




def TH1F(name, title, nxbins, bins_par2, bins_par3=None, path='', **kwargs):

    res = THWrapper(name)
    res['type'] = 'TH1F'
    res['title'] = title

    if isinstance(bins_par2, (list, tuple) ): # signature 2
        res['xbins'] = bins_par2
    else: # signature 1
        res['xbins'] = nxbins
        res['xmin'] = bins_par2
        res['xmax'] = bins_par3

    res['path'] = path
    res.kwargs.update(kwargs)
    return res





def TH2F(name, title, nxbins, bins_par2, bins_par3, bins_par4, bins_par5=None, bins_par6=None, path='/', **kwargs):

    res = THWrapper(name) 
    res['type'] = 'TH2F'
    res['title'] = title

    if isinstance(bins_par2, (list, tuple) ): # signature 2 or 4
        res['xbins'] = bins_par2
        if isinstance(bins_par4, (list, tuple) ): # signature 4
            res['ybins'] = bins_par4
        else: # signature 2
            res['ybins'] = bins_par3
            res['ymin'] = bins_par4
            res['ymax'] = bins_par5

    else: # signature 1 or 3
        res['xbins'] = nxbins
        res['xmin'] = bins_par2
        res['xmax'] = bins_par3
        if isinstance(bins_par5, (list, tuple) ): # signature 3
            res['ybins'] = bins_par5
        else: # signature 1
            res['ybins'] = bins_par4
            res['ymin'] = bins_par5
            res['ymax'] = bins_par6

    res['path'] = path
    res.kwargs.update(kwargs)
    return res







def TProfile(*args, **kwargs):
    res = TH1F(*args, **kwargs)
    res.name = res.name
    res['type'] = 'TProfile'
    return res





