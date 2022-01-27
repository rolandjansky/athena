# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

"""
Tools for histogram sampling, in particular inverse transform sampling which is
missing from ROOT's TH2 classes.
"""

__author__ = "Andy Buckley <andy.buckley@cern.ch>"

import random, copy, ROOT


def load_hist(*args):
    """
    Load a histogram from a filename/TFile and histo name. If a single arg is
    provided, it has to be a histo object and will be cloned before return.
    """
    h = None
    if len(args) == 1 and issubclass(type(args[0]), ROOT.TH1):
        h = args[0].Clone()
    elif len(args) == 2:
        if isinstance(args[0], str) and isinstance(args[1], str) :
            f = ROOT.TFile.Open(args[0])
            h = copy.deepcopy(f.Get(args[1]).Clone())
            #f.Close()
        elif type(args[0]) is ROOT.TFile and type(args[1]) is str:
            h = args[0].Get(args[1]).Clone()
    if h is None:
        raise Exception("Error in histogram loading from " + args)
    return h


def get_sampling_vars(h):
    """
    Get the following from a histogram h, since the ROOT API sucks:
    * list of global bin IDs (not even contiguous for 2D, gee thanks ROOT)
    * dict mapping global bin IDs to a tuple of axis bin IDs
    * list of nbins+1 cumulative bin values, in the same order as globalbins
    """
    globalbin_to_axisbin = {} # for reverse axis bin lookup to get edges
    globalbins = [] # because they aren't easily predicted, nor contiguous
    cheights = [0] # cumulative "histogram" from which to uniformly sample
    if issubclass(type(h), ROOT.TH1):
        for ix in range(1, h.GetNbinsX()+1):
            iglobal = h.GetBin(ix)
            globalbins.append(iglobal)
            globalbin_to_axisbin[iglobal] = (ix,)
            cheights.append(cheights[-1] + h.GetBinContent(iglobal))
    elif issubclass(type(h), ROOT.TH2):
        for ix in range(1, h.GetNbinsX()+1):
            for iy in range(1, h.GetNbinsY()+1):
                iglobal = h.GetBin(ix, iy)
                globalbins.append(iglobal)
                globalbin_to_axisbin[iglobal] = (ix, iy)
                cheights.append(cheights[-1] + h.GetBinContent(iglobal))
    return globalbins, globalbin_to_axisbin, cheights


def get_random_bin(globalbins, cheights):
    """
    Choose a random bin from the cumulative distribution list of nbins+1 entries.

    TODO: Search more efficiently (lin and log guesses, then lin search or
    binary split depending on vector size).
    """
    assert len(cheights) == len(globalbins)+1
    randomheight = random.uniform(0, cheights[-1])
    for i, iglobal in enumerate(globalbins):
        if randomheight >= cheights[i] and randomheight < cheights[i+1]:
            return iglobal
    raise Exception("Sample fell outside range of cumulative distribution?!?!")


def get_random_x(h, globalbins, cheights, globalbin_to_axisbin):
    """
    Choose a random bin via get_random_bin, then pick a uniform random x
    point in that bin (without any attempt at estimating the in-bin distribution).
    """
    irand = get_random_bin(globalbins, cheights)
    axisids = globalbin_to_axisbin.get(irand)
    assert axisids is not None
    xrand = random.uniform(h.GetXaxis().GetBinLowEdge(axisids[0]), h.GetXaxis().GetBinUpEdge(axisids[0]))
    return xrand


def get_random_xy(h2, globalbins, cheights, globalbin_to_axisbin):
    """
    Choose a random bin via get_random_bin, then pick a uniform random x,y
    point in that bin (without any attempt at estimating the in-bin distribution).
    """
    irand = get_random_bin(globalbins, cheights)
    axisids = globalbin_to_axisbin.get(irand)
    assert axisids is not None
    xrand = random.uniform(h2.GetXaxis().GetBinLowEdge(axisids[0]), h2.GetXaxis().GetBinUpEdge(axisids[0]))
    yrand = random.uniform(h2.GetYaxis().GetBinLowEdge(axisids[1]), h2.GetYaxis().GetBinUpEdge(axisids[1]))
    return xrand, yrand


class TH1(object):
    "Minimal wrapper for ROOT TH1, for sampling consistency and easy loading"

    def __init__(self, *args):
        self.th1 = load_hist(*args)
        self.globalbins, self.globalbin_to_axisbin, self.cheights = None, None, None

    def GetRandom(self):
        "A GetRandom that works for TH1s and uses Python random numbers"
        if self.globalbins is None or self.globalbin_to_axisbin is None or self.cheights is None:
            self.globalbins, self.globalbin_to_axisbin, self.cheights = get_sampling_vars(self.th1)
        return get_random_x(self.th1, self.globalbins, self.cheights, self.globalbin_to_axisbin)

    def __getattr__(self, attr):
        "Forward all attributes to the contained TH1"
        return getattr(self.th1, attr)


class TH2(object):
    "Minimal wrapper for ROOT TH2, for easy loading and to allow 2D sampling"

    def __init__(self, *args):
        self.th2 = load_hist(*args)
        self.globalbins, self.globalbin_to_axisbin, self.cheights = None, None, None

    def GetRandom(self):
        "A GetRandom that works for TH2s"
        if self.globalbins is None or self.globalbin_to_axisbin is None or self.cheights is None:
            self.globalbins, self.globalbin_to_axisbin, self.cheights = get_sampling_vars(self.th2)
        return get_random_xy(self.th2, self.globalbins, self.cheights, self.globalbin_to_axisbin)

    def __getattr__(self, attr):
        "Forward other attributes to the contained TH2"
        return getattr(self.th2, attr)
