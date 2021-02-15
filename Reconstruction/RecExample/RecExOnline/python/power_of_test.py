# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from scipy.stats import rv_discrete
import test_statistics as TS
import numpy as np

# If True, import IPyParallel package to do the parallel computation.
# And if IPyParallel is not installed, automatically turns it False.
PARALLEL = True
if PARALLEL:
    try:
        from ipyparallel import Client
    except ImportError:
        PARALLEL = False
# IPyParallel configurations.
# For example, to activate SSH mode:
# ipp_profile = {
#     'sshserver':'<username>@<host>',
#     'profile':'<IPyParallel profile directory>'
#     }
# Leave it empty if you don't want to make changes.
ipp_profile = {'sshserver':'kingman@140.114.94.171',
               'profile':'ssh@nthu'}

def _hist_fill(data, bins, entries = 0, freeze = None):
    if type(bins)==int:
        if bins==0:
            bins = np.arange(data.size + 1, dtype = float)
        else:
            bins = np.linspace(data.min(), data.max(), bins)
    binc = (bins[1:] + bins[:-1]) / 2 
    if freeze:
        return lambda : data
    else:
        rv = rv_discrete(values = (binc, data/data.sum()))
        return lambda : np.histogram(rv.rvs(size=data.sum() if entries==0 else entries), bins)[0].astype(float)

def rvs_pairs(data1, data2, bins = 0, kind = 'binbybin', bin_mean = 1, size = 1, entries = 0, freeze = False):
    """
    Compute the histogram pairs from the random variates of the given 2 samples/frequencies for size_times. 
    
    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Observed samples or frequencies.
    bins : [scalar|1-D array], optional, default : 0
        If bins is an int, it defines the number of equal-width bins in the given
        range. If bins is zero, the given data will be assumed to be frequency data,
        and it defines the bin index. If bins is an 1-D array, it defines the bin
        edges, including the rightmost edge, allowing for non-uniform bin widths.
    kind : [str|(str,str)], optional, default : "binbybin"
        Bin-by-bin test
    bin_mean : [scalar|(scalar,scalar)], optional, default : 1
        The desired mean count in each bin of the output histograms. For example, 
        if a histogram has 10000 entries and 100 bins, the bin_mean is 100. It is
        ignored when entries is given.
    size : int, optional, default : 1
        The number of repetitions for computing the histograms.
    entries : [scalar|(scalar,scalar)], optional, default : 0
        The desired entries of the output histograms. If entries is 0, the entries
        size of the input data will be used instead.
    freeze : [bool,(bool,bool)], optional. default : False
        if True, the output histogram will be the frozen distribution of the input
        data
    
    Yields
    ------
    hist1, hist2 : (1-D array, 1-D array)
        Histogram pairs
    """ 
    data1 = data1.astype(float)
    data2 = data2.astype(float)
    entries = np.resize(entries, 2)
    freeze = np.resize(freeze, 2)
    hist1 = _hist_fill(data1, bins, entries[0], freeze[0])
    hist2 = _hist_fill(data2, bins, entries[1], freeze[1])
    for _ in range(size):
        yield hist1(), hist2()

def power_of_test(data1, data2, rvs_func = 'rvs_pairs', tests = ['chi2_2samp'], rvs_key = {}, test_key = {}, parallel=None, sync=True):
    """Compute the corresponding p-values for each histrogram pairs from the random variates of the given 2 samples/frequencies for size_times.
    
    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Observed samples or frequencies.
    rvs_func : [callable|str], optional, default : "rvs_pairs"
        The random variates function. The rvs_func can be either a callable or one of the following strings::

            String                     Description
            "rvs_pairs"                Compute the histogram pairs from the random
                                       variates of the given 2 samples/frequencies
                                       for size_times. 

    tests : ([callable|str],...), optional, default : ["chi2_2samp"]
        A list of *test* statistical functions. The *test* can be either a callable or one of the following strings::

            String                     Description
            "chi2_2samp"               Read TS.chi2_2samp for further information.
            "BDM_2samp"                Read TS.BDM_2samp for further information.
            "likelihoodratio_ksamp"    Read TS.likelihoodratio_ksamp for further information.
            "likelihoodvalue_ksamp"    Read TS.likelihoodvalue_ksamp for further information.
            "ks_2samp"                 Read TS.ks_2samp for further information.
            "anderson_ksamp"           Read TS.anderson_ksamp for further information.
            "CVM_2samp"                Read TS.CVM_2samp for further information.

    rvs_key : dict, optional, default : {}
        Keyword arguments for the rvs function, rvs_func.
    test_key : dict, optional
        Keyword arguments for the test statistical function, test.
    parallel : bool, optional, default : None
        If True, import IPyParallel package to do the parallel computation. If parallel is None,
        the global variable PARALLEL will be used instead.
    sync : bool, optional, default : True
        When sync is False, an IPyParallel AsyncResult Object will be returned instead. Onyl affect
        when parallel is True.
    
    Returns
    -------
    [p1, p2, ...] : 1-D array
        The corresponding p-values for each histogram pairs.
    """
    if parallel is None: parallel = PARALLEL
    if parallel:
        try:
            global client
            client=Client(**ipp_profile)
            size = rvs_key['size']
            N = len(client)
            jobs = []
            for i in range(N):
                rvs_key['size'] = (size//N + 1) if (i < size % N) else size//N
                jobs.append(client[client.ids[i]].apply_async(power_of_test, data1, data2, rvs_func, tests, rvs_key, test_key, False))
            ars = client._asyncresult_from_jobs(jobs)
            if sync:
                ars.wait_interactive()
                ret = {}
                for key, val in ars.get():
                    ret.setdefault(key, []).extend(val)
            else:
                return ars
        finally:
            client.close()
        return ret
    if type(rvs_func) == str:
        rvs_func = globals()[rvs_func]
    if type(tests) not in (list, tuple):
        tests = [tests]
    tests = [(t, getattr(TS, t)) if type(t)==str else (str(t), t) for t in tests]
    ret = {}
    for rvs1, rvs2 in rvs_func(data1,data2,**rvs_key):
        for tname, test in tests:
            ret.setdefault(tname, []).append(test(rvs1, rvs2, binned=True, **test_key).pvalue)
    return ret
