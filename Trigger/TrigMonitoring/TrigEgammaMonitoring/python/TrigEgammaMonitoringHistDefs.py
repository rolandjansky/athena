#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

class HistDefinition(object):
    def __init__(self, name):
        self.name = name
        self.kwargs = {}

    def __setitem__(self, key, value):
        self.kwargs[key] = value


def _define1D(name, title, nxbins, bins_par2, bins_par3=None, path='/', **kwargs):
    """
    Emulate multiple ROOT C++ constructors to minimize retyping of histograms definitions

    TH1F (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup) -- signature 1
    TH1F (const char *name, const char *title, Int_t nbinsx, const Double_t *xbins)       -- signature 2
    """

    res = HistDefinition(name)
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


def TH1F(*args, **kwargs):
    res = _define1D(*args, **kwargs)
    res['type'] = 'TH1F'
    return res

def TH1I(*args, **kwargs):
    res = _define1D(*args, **kwargs)
    res['type'] = 'TH1I'
    return res

def TProfile(*args, **kwargs):
    res = _define1D(*args, **kwargs)
    res.name = '{0}#1,{0}#2;{0}'.format(res.name) # Create two monitored variables with automatic names (; alias=hist name in ROOT), the same names created in C++ at filling time
    res['type'] = 'TProfile'
    return res

def _define2D(name, title, nxbins, bins_par2, bins_par3, bins_par4, bins_par5=None, bins_par6=None, path='/', **kwargs):
    """
    Emulate multiple ROOT C++ constructors to minimize retyping of histograms definitions

    TH2F (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup) -- signature 1
    TH2F (const char *name, const char *title, Int_t nbinsx, const Double_t *xbins,       Int_t nbinsy, Double_t ylow, Double_t yup) -- signature 2
    TH2F (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, const Double_t *ybins)       -- signature 3
    TH2F (const char *name, const char *title, Int_t nbinsx, const Double_t *xbins,       Int_t nbinsy, const Double_t *ybins)       -- signature 4
    """
    res = HistDefinition(name)
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

def TH2F(name, *args, **kwargs):
    res = _define2D(name, *args, **kwargs)
    res.name = '{0}#1,{0}#2;{0}'.format(res.name) # Create two monitored variables with automatic names (; alias=hist name in ROOT), the same names created in C++ at filling time
    res['type'] = 'TH2F'
    return res

def TProfile2D(name, *args, **kwargs):
    res = _define2D(name, *args, **kwargs)
    res.name = '{0}#1,{0}#2,{0}#3;{0}'.format(res.name) # Create tree monitored variables with automatic names (; alias=hist name in ROOT), the same names created in C++ at filling time
    res['type'] = 'TProfile2D'
    return res
