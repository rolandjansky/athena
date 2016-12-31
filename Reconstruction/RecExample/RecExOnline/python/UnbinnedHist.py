# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import rootpy.ROOT as ROOT
import ROOT as R
from rootpy.plotting import Hist
import numpy as np
class UnbinnedHist(ROOT.TH1F,R.TH1F):
    def __new__(cls, tree, expression, selection='', options='', hist=None, create_hist=False, **kwargs):
        obj = tree.Draw(expression, selection, options, hist, create_hist, **kwargs)
        v1 = tree.get_v1()
        v1.SetSize(obj.entries)
        obj._raw_data = np.fromiter(v1, np.float)
        return obj
    def get_raw_data(self):
        return self._raw_data
    def __add__(self,y):
        x = self.Clone("New")
        x.fill_array(y.get_raw_data())
        x._raw_data = np.append(self._raw_data, y._raw_data)
        return x
    def __radd__(self,y):
        return y.__add__(self)
    def refill(self, bins=100, range=None, inplace=False):
        if np.iterable(bins):
            self_ = Hist(bins)
        else:
            if not range:
                range = (self.lowerbound(), self.upperbound())
            self_ = Hist(bins, *range)
        raw_data = self._raw_data
        self_.fill_array(raw_data)
        self_._raw_data = raw_data
        self_.__class__ = UnbinnedHist
        return self_
def unbinned_hist(tree, expression, selection='', options='', hist=None, create_hist=False, **kwargs):
    obj = UnbinnedHist(tree, expression, selection, options, hist, create_hist, **kwargs)
    obj.__class__ = UnbinnedHist
    return obj
def group_eqbin(hist_list, bins=100, range=None, inplace=True):
    if not range:
        lowerbound = min(hist.lowerbound() for hist in hist_list)
        upperbound = max(hist.upperbound() for hist in hist_list)
        range = (lowerbound, upperbound)
        hist_list = [hist.refill(bins, range, inplace) for hist in hist_list]
    if not inplace:
        return hist_list
        
