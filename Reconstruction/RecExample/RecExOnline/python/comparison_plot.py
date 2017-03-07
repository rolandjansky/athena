# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Draw comparison plots and statistical tables for each 2 histograms inside 2 data sets.
"""

import rootpy.ROOT as ROOT
from rootpy.plotting.hist import Hist
from rootpy.plotting.style import get_style, set_style
import rootpy.plotting.root2matplotlib as rplt
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.ticker
import itertools
import numpy as np
import pandas as pd
import re
from collections import defaultdict
import HistCompare.test_statistics as TS
import copy
from UnbinnedHist import UnbinnedHist
from hash_tool import HashTool
matplotlib.rcParams["text.usetex"]=True
matplotlib.rcParams['text.latex.preamble']=['\usepackage{booktabs}']
def tree(): return defaultdict(tree)
#set_style('ATLAS',mpl=True)
def set_yaxis(ax, tick=None):
    if tick == "%":
        def to_percent(y, position):
            s = format(100*y, '.1f')
            if matplotlib.rcParams['text.usetex'] is True:
                return s + r'$\%$'
            else:
                return s + '%'
        formatter = matplotlib.ticker.FuncFormatter(to_percent)
        ax.yaxis.set_major_formatter(formatter)
def identity(*x):
    if len(x)==1:
        return x[0]
    return x
def formatted_plt_dict(plt_kw, length, **kwarg):
    for i in range(length):
        yield dict((key, val[i]) if np.iterable(val) else (key, val) for key, val in plt_kw.iteritems())

# keyword arguments for matplotlib.pyplot.text
TEXT_KW = {
    "x": 0.94,
    "y": 0.25,
    "ha": "right",
    "va": "center",
    'zorder': 2}

# keyword arguments for auto_statistic_table
STATISTIC_TABLE_KW = {
    "iterfunc": itertools.combinations,
    "iterkey": {"r": 2},
    "row_title_func": identity,
    "col_title_func": identity, 
    "table_fformat": ".2e",
    "text_kw": TEXT_KW}

# keyword arguments for matplotlib.pyplot.setp
AXES_KW = {
    "ylim":(0,None)}

# keyword arguments for matplotlib.pyplot.plot
PLT_KW = {
    "linewidth": 2,
    "color": None,
    'zorder': 1
    }

def statistic_table_kw_func(ax):
    statistic_table_kw = copy.deepcopy(STATISTIC_TABLE_KW)
    statistic_table_kw["text_kw"].update({"axes": ax, "transform": ax.transAxes})
    return statistic_table_kw

def auto_draw(hist_dict, cumulative = False, normalized = False, table = True, axes = None, frameon = True, cmap = plt.cm.brg, add_statistic_table_kw = {}, add_axes_kw = {}, add_plt_kw = {}):
    """
    Draw comparison plots and statistical tables for each 2 histograms inside 2 data sets.
    
    Parameters
    ----------
    hist_dict : pandas dataframe
        Input data. *hist_dict* Should have 2 *rootpy.plotting.Hist*/*UnbinnedHist* instances
        in each row.
    cumulative : bool, optional
        If True, then a histogram is computed where each bin gives the counts in that bin plus all bins for smaller
        values. The last bin gives the total number of datapoints. If normed is also True then the histogram is
        normalized such that the last bin equals 1. If cumulative evaluates to less than 0 (e.g., -1), the direction
        of accumulation is reversed. In this case, if normed is also True, then the histogram is normalized such that
        the first bin equals 1.
    normalized : bool, optional
        If False, the result will contain the number of samples in each bin. If True, the result is the value of the 
        probability density function at the bin, normalized such that the integral over the range is 1.
    table : bool, optional
        If True, places a statistical table on each axes.
    axes : list of matplotlib axis, optional, default : None
        if provided, plot on these axes. Note *axes* should have the same number as *hist_dict*. i.e.::
        >>> len(axes) = len(hist_dict)
    frameon : bool, optional, default : True
        If False, suppress drawing the figure frame
    cmap : matplotlib.colors.Colormap instance or list of str, optional, default : plt.cm.brg
        list of any matplotlib colors.
    add_statistic_table_kw : dict, optional
        Additional Keyword Arguments for ``statistic_table``
    add_axes_kw : dict, optional
        Additional Keyword Arguments for ``matplotlib.pyplot.setp``
    add_plt_kw : dict, optional
        dditional Keyword Arguments for ``matplotlib.pyplot.plot``
    
    Returns
    -------
    [ax1, ax2, ...] : list
    list of matplotlib axes
    """
    if axes == None:
        axes = [plt.axes() for _ in hist_dict if plt.figure(frameon=frameon)]
    for ax, kind in zip(axes, hist_dict): 
        statistic_table_kw = statistic_table_kw_func(ax)
        statistic_table_kw.update(add_statistic_table_kw)
        axes_kw = AXES_KW
        axes_kw.update(add_axes_kw)
        plt_kw = PLT_KW
        plt_kw.update(add_plt_kw)
        if cmap:
            plt_kw["color"] = cmap(np.linspace(0, 1, len(hist_dict)))   
        plt_dicts = formatted_plt_dict(plt_kw, len(hist_dict))

        for (i, h), d in zip(hist_dict[kind].iteritems(), plt_dicts):
            entries = h.entries
            h_ = h.clone("New1")
            if normalized:
                h_.Scale(1./h_.Integral())      
            if cumulative:
                arr = np.cumsum([bin.value for bin in h_ ])
                h_.set_content(arr)
            for key, val in d.iteritems():
                setattr(h_,key,val)
            rplt.hist(h_, label = "%s (%i)" % (statistic_table_kw["col_title_func"](i),entries), axes = ax)

        ########################################################################
        if np.all([type(v)==UnbinnedHist for v in hist_dict[kind].values]) and cumulative:
            data_all = np.concatenate(map(UnbinnedHist.get_raw_data, hist_dict[kind].values))
            if normalized:
                cdfs = [sorted(np.searchsorted(data.get_raw_data(), data_all, side='right')*1.0 / data.get_raw_data().shape[0]) for data in hist_dict[kind].values]
            else:
                cdfs = [sorted(np.searchsorted(data.get_raw_data(), data_all, side='right')) for data in hist_dict[kind].values]
            for i, cdf, d in zip(hist_dict[kind].keys(), cdfs, plt_dicts):
                plt.plot(sorted(data_all), cdf, label = "%s (%i)" % (statistic_table_kw["col_title_func"](i),entries), linestyle="--", axes = ax, **d)
        ########################################################################

        ax.legend()
        if table:
            df = auto_statistic_table(hist_dict[kind], ax=ax, ret_table=False, ret_text=False, **statistic_table_kw)
            if HASH.objs:
                textkw = statistic_table_kw["text_kw"].copy()
                text = ax.text(textkw.pop("x"), textkw.pop("y"), df[0].to_latex(escape=False, sparsify=False, index_names=False).replace("\n",""), **textkw)
        ax.set_xlabel(kind, fontsize=25)
        
        plt.setp(ax, **axes_kw) 
        if normalized:
            ax.set_ylim(0,1)
            set_yaxis(ax,tick='%')
        if not cumulative:
            ax.set_ylabel("Events", fontsize=25)
        else:
            ax.set_ylabel("Cumulative Events", fontsize=25)
    return axes

HASH = HashTool('hist_dict', ret_table=True)
@HASH
def auto_statistic_table(hist_dict, ax=None, ret_table=True, ret_text=True, **statistic_table_kw):
    """Compute statistic tables for the 2 histograms in each row of *hist_dict*.
    
    Parameters
    ----------
    hist_dict : pandas dataframe
        Input data. *hist_dict* Should have 2 *rootpy.plotting.Hist*/*UnbinnedHist* instances
        in each row.
    ax : matplotlib axis, optional, default : None
        if provided, plot on this axis
    statistic_table_kw : dict, optional
        Other keyword arguments
    ret_table : bool, optional, default : True
        If True, returns the formatted table in *list* form.
    ret_text : bool, optional, default : True
        If True, returns the formatted table in *latex* form.
    
    Returns
    -------
    [text[, table]] : list
        depending on *ret_table* and *ret_text*, returns a list with required inside.
    """
    header = ["Type of Test", "Bin", "T","P","NDF","Description"]
    df_ = pd.DataFrame()
    textkw = statistic_table_kw["text_kw"].copy()
    for (nA,A),(nB,B) in statistic_table_kw["iterfunc"](hist_dict.iteritems(), **statistic_table_kw["iterkey"]):
        A_Arr_Binned = np.array([bin.value for bin in A.bins()])
        B_Arr_Binned = np.array([bin.value for bin in B.bins()])
        bin_str = A_Arr_Binned.shape[0]
        Table=[#["$\chi^2$ Absolute Comparison", bin_str] + list(TS.chi2_2samp(A_Arr_Binned, B_Arr_Binned, normed=False, binned=True)) + ["Scipy + Modificatoin"],
        ["$\chi^2$ Shape Comparison",bin_str] + list(TS.chi2_2samp(A_Arr_Binned, B_Arr_Binned, normed=True, binned=True)) + ["Scipy + Modificatoin"],
        ["",bin_str, A.Chi2Test(B,"WW CHI2"), A.Chi2Test(B,"WW"),'',"ROOT"],
        ["Likelihood Ratio Shape Comparison",bin_str]+list(TS.likelihoodratio_ksamp(A_Arr_Binned, B_Arr_Binned))+["Scipy + Modification"],
        #["Likelihood Value Shape Comparison",bin_str]+list(TS.likelihoodvalue_ksamp(A_Arr_Binned, B_Arr_Binned))+["Scipy + Modification"],
        #["BDM Shape Comparison",bin_str] + list(TS.BDM_2samp(A_Arr_Binned, B_Arr_Binned))+["Scipy + Modification"],
        ["K-S Shape Comparison",bin_str] + list(TS.ks_2samp(A_Arr_Binned,B_Arr_Binned,binned=True))+ ["SciPy + Modification"],
        ["",bin_str,A.KolmogorovTest(B,"M"), A.KolmogorovTest(B), '',"ROOT"],
        ["A-D Shape Comparison",bin_str] + list(TS.anderson_ksamp(A_Arr_Binned, B_Arr_Binned, binned=True)) + ["Scipy + Modification"],  
        ["CVM Shape Comparison",bin_str] + list(TS.CVM_2samp(A_Arr_Binned, B_Arr_Binned, binned=True)) + ["Matlab"]]

        if type(A)==UnbinnedHist and type(B)==UnbinnedHist:
            Table.insert(8,  ["","Unbinned"] + list(TS.ks_2samp(A.get_raw_data(), B.get_raw_data(), binned=False)) + ["SciPy + Modification"])
            Table.insert(10, ["","Unbinned"] + list(TS.anderson_ksamp(A.get_raw_data(), B.get_raw_data(), binned=False)) + ["Scipy"])
            Table.insert(12, ["","Unbinned"] + list(TS.CVM_2samp(A.get_raw_data(), B.get_raw_data(), binned=False)) + ["Matlab"])

        df=pd.DataFrame(Table, columns=header)
        df["T/P"] = map(lambda tp: "/".join(["%"+statistic_table_kw["table_fformat"]]*2)%(tp[0],tp[1]), zip(df["T"],df.P))
        df_ = df[header]
    text = ax.text(textkw.pop("x"), textkw.pop("y"), df_.to_latex(escape=False, sparsify=False, index_names=False).replace("\n",""), **textkw)
    result = []
    if ret_text:
        result.append(text)
    if ret_table:
        result.append(df_)
    return result
