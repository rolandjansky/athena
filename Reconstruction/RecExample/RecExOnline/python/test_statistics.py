# -*- coding: utf-8 -*-
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
Test statistic functions.
"""
from collections import namedtuple
import numpy as np
import scipy.stats
import scipy.interpolate
import scipy.special
import statsmodels.api as sm
import warnings
import distributions as distr
from sympy import binomial, log

#If True, estimate distributions of the test statistic for the current null hypothesis and dump it into statistic_seq
DEBUG = False
#The estimated distributions of the test statistic for the current null hypothesis.
statistic_seq = []

Ks_2sampResult = namedtuple('Ks_2sampResult', ('statistic', 'pvalue','ndf'))
def ks_2samp(data1, data2, binned=False):
    """
    Computes the Kolmogorov-Smirnov statistic on 2 samples/frequencies.

    This is a two-sided test for the null hypothesis that 2 independent samples
    are drawn from the same continuous distribution.

    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Can be either observed frequencies in each category or array
        of sample observations.
    binned : bool, optional, default: False
        If True/False, assume the type of input data is observed frequencies/samples.

    Returns
    -------
    statistic : float
        KS statistic
    pvalue : float
        two-tailed p-value
    ndf : NaN
        「Degrees of freedom」: the degrees of freedom for the p-value. Always NaN in this case.

    Notes
    -----
    This code is modified from scipy.stats.ks_2samp and extended with supporting on
    frequency data. See [1]_ for further information.

    This tests whether 2 samples are drawn from the same distribution. Note
    that, like in the case of the one-sample K-S test, the distribution is
    assumed to be continuous.

    This is the two-sided test, one-sided tests are not implemented.
    The test uses the two-sided asymptotic Kolmogorov-Smirnov distribution.

    If the K-S statistic is small or the p-value is high, then we cannot
    reject the hypothesis that the distributions of the two samples
    are the same.

    References
    ----------
    .. [1] scipy.stats.mstats.ks_2samp — SciPy Reference Guide
           http://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.mstats.ks_2samp.html
    .. [2] Frank C. Porter. (2008). Testing Consistency of Two Histograms, arXiv:0804.0380 [physics.data-an].

    Examples
    --------
    >>> from scipy import stats
    >>> np.random.seed(12345678)  #fix random seed to get the same result
    >>> n1 = 200  # size of first sample
    >>> n2 = 300  # size of second sample

    For a different distribution, we can reject the null hypothesis since the
    pvalue is below 1%:

    >>> rvs1 = stats.norm.rvs(size=n1, loc=0., scale=1)
    >>> rvs2 = stats.norm.rvs(size=n2, loc=0.5, scale=1.5)
    >>> stats.ks_2samp(rvs1, rvs2)
    (0.20833333333333337, 4.6674975515806989e-005)

    For a slightly different distribution, we cannot reject the null hypothesis
    at a 10% or lower alpha since the p-value at 0.144 is higher than 10%

    >>> rvs3 = stats.norm.rvs(size=n2, loc=0.01, scale=1.0)
    >>> stats.ks_2samp(rvs1, rvs3)
    (0.10333333333333333, 0.14498781825751686)

    For an identical distribution, we cannot reject the null hypothesis since
    the p-value is high, 41%:

    >>> rvs4 = stats.norm.rvs(size=n2, loc=0.0, scale=1.0)
    >>> stats.ks_2samp(rvs1, rvs4)
    (0.07999999999999996, 0.41126949729859719)

    """
    if binned is True:
        cdf1 = np.cumsum(data1)
        cdf2 = np.cumsum(data2)
        n1 = cdf1[-1]
        n2 = cdf2[-1]
        cdf1 /= n1
        cdf2 /= n2
        ndf = data1.shape[0]
    else:
        n1 = data1.shape[0]
        n2 = data2.shape[0]
        ndf = float("nan")
        if binned is False:
            data1 = np.sort(data1)
            data2 = np.sort(data2)
            data_all = np.concatenate([data1, data2])
            cdf1 = np.searchsorted(data1, data_all, side='right') / (1.0*n1)
            cdf2 = np.searchsorted(data2, data_all, side='right') / (1.0*n2)
        if binned == 'kde':
            data1_kde = sm.nonparametric.KDEUnivariate(data1)
            data2_kde = sm.nonparametric.KDEUnivariate(data2)
            data1_kde.fit()
            data2_kde.fit()
            i, min_sup = min(enumerate([data1_kde.support, data1_kde.support]), key = lambda data: data[1].shape[0])
            if i == 0:
                cdf1 = data1_kde.cdf
                cdf2 = scipy.interpolate.interp1d(data2_kde.support, data2_kde.cdf)(min_sup)
            else:
                cdf1 = scipy.interpolate.interp1d(data1_kde.support, data1_kde.cdf)(min_sup)
                cdf2 = data2_kde.cdf
    d = np.max(np.absolute(cdf1 - cdf2))
    # Note: d absolute not signed distance
    en = np.sqrt(n1 * n2 / float(n1 + n2))
    try:
        prob = scipy.stats.distributions.kstwobign.sf((en + 0.12 + 0.11 / en) * d)
    except Exception:
        prob = 1.0
    if DEBUG:
        global statistic_seq
        statistic_seq = np.absolute(cdf1 - cdf2)
    return Ks_2sampResult(d, prob, ndf)


Chi2_2sampResult = namedtuple('Chi2_2sampResult', ('statistic', 'pvalue','ndf'))
def chi2_2samp(data1, data2, normed=True, binned=True):
    """
    The chi square test tests the null hypothesis that 2 given frequencies are drawn
    from the same distribution.
    
    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Observed frequencies in each category.
    normed : bool, optional, default: True
        If True/False, shape/value comparison test.
    binned : bool, optional, default: True
        If True/False, assume the type of input data is observed frequencies/samples.
    
    Returns
    -------
    statistic : float
        The chi-squared test statistic.
    pvalue : float
        Corresponding p-value.
    ndf : int
        「Degrees of freedom」: the degrees of freedom for the p-value. The p-value
        is computed using a chi-squared distribution with k - 1 degrees of freedom,
        where k is the number of observed frequencies without bins has zero counts 
        in both histograms.

    Note
    ----
    This code is modified from scipy.stats.chisquare and extended with supporting on
    2 sample cases and shape comparison test. 
    """
    if binned is True:
        filter = ~((data1 == 0.) & (data2 == 0.))
        data1 = data1[filter] 
        data2 = data2[filter] 
        ndf = data1.shape[0]
        if not normed:
            chi2 = np.power(data1 - data2, 2).dot(np.power(data1 + data2, -1))
        else:
            ndf -= 1
            N_data1 = data1.sum()
            N_data2 = data2.sum()
            data1 /= N_data1
            data2 /= N_data2
            chi2 = np.power(data1 - data2, 2).dot(np.power(data1/N_data1 + data2/N_data2, -1))
    if DEBUG:
        global statistic_seq
        if not normed:
            statistic_seq = np.power(data1 - data2, 2) * (np.power(data1 + data2, -1))
        else:
            statistic_seq = np.power(data1 - data2, 2) * np.power(data1/N_data1 + data2/N_data2, -1)
    return Chi2_2sampResult(chi2, scipy.stats.chi2.sf(chi2, ndf), ndf)

BDM_2sampResult = namedtuple('BDM_2sampResult', ('statistic', 'pvalue', 'ndf'))
def BDM_2samp(data1, data2, normed=True, binned=True):
    """
    The Bhattacharyya test tests the null hypothesis that 2 given frequencies are drawn
    from the same distribution.    

    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Observed frequencies in each category.
    normed : bool, optional
        If True/False, shape/value comparison test.
    binned : bool, optional
        If True/False, assume the type of input data is observed frequencies/samples.
    
    Returns
    -------
    statistic : float
        The Bhattacharyya distance measure.
    pvalue : float
        Corresponding p-value.
    ndf : int
        「Degrees of freedom」: the degrees of freedom for the p-value. The p-value
        is computed using a chi-squared distribution with k - 1 degrees of freedom,
        where k is the number of observed frequencies without bins has zero counts 
        in both histograms.
    """
    if binned:
        filter = ~((data1 == 0.) & (data2 == 0.))
        data1 = data1[filter] 
        data2 = data2[filter] 
        ndf = data1.shape[0]
        N_data1 = data1.sum()
        N_data2 = data2.sum()
        bdm = np.sqrt(data1 * data2).sum()
        approx_chi2 = 2*N_data1+2*N_data2 - 4*bdm
        if normed:
            ndf -= 1
            bdm /= np.sqrt(N_data1 * N_data2)
    if DEBUG:
        global statistic_seq
        statistic_seq = 2*data1+2*data2 - 4*np.sqrt(data1 * data2)
    return BDM_2sampResult(bdm, scipy.stats.chi2.sf(approx_chi2, ndf), ndf)

CVM_2sampResult = namedtuple('CVM_2sampResult', ('statistic', 'pvalue', 'ndf'))
def CVM_2samp(data1, data2, normed=True, binned=False):
    """
    Computes the Cram ́er-von-Mises statistic on 2 samples/frequencies.

    This is a two-sided test for the null hypothesis that 2 independent samples
    are drawn from the same continuous distribution.

    Parameters
    ----------
    data1 ,data2 : sequence of 1-D ndarrays
        Input data. Can be either observed frequencies in each category or array
        of sample observations.
    binned : bool, optional, default: False
        If True/False, assume the type of input data is observed frequencies/samples.
    
    Returns
    -------
    statistic : float
        CVM statistic
    pvalue : float
        two-tailed p-value
    ndf : NaN
        「Degrees of freedom」: the degrees of freedom for the p-value. Always NaN in this case.
    """
    N1 = data1.shape[0]
    N2 = data2.shape[0]
    N = N1 + N2
    if binned:
        E1 = data1.sum()
        E2 = data2.sum()
        cdf1 = np.cumsum(data1) / E1
        cdf2 = np.cumsum(data2) / E2
        weights = data1 + data2
    else:
        E1 = N1
        E2 = N2
        data_all = np.concatenate([data1, data2])
        cdf1 = np.searchsorted(data1, data_all, side='right') / (1.0*N1)
        cdf2 = np.searchsorted(data2, data_all, side='right') / (1.0*N2)
        weights = 1.
    cvm = (weights * (cdf1 - cdf2)**2).sum() * E1 * E2 / (E1 + E2)**2
    mean = 1./6 + 1./6/N
    var = (N + 1.) * (4.*N1*N2*N - 3*(N1**2+N2**2) - 2*N1*N2) / (4 * N1 * N2) /N**2/45
    if DEBUG:
        global statistic_seq
        statistic_seq = (weights * (cdf1 - cdf2)**2) * E1 * E2 / (E1 + E2)**2
    return CVM_2sampResult(cvm, 1-distr.CVM(cvm, mean, var), float("nan"))

def _anderson_ksamp_midrank_binned(data, Z, Zstar, k, n, N):
    #########################################
    if DEBUG:
        global statistic_seq
        statistic_seq = []
    #########################################
    A2akN = 0.
    lj = Z
    Bj = Z.cumsum() - lj/2
    for i in np.arange(0, k):
        d = data[i]
        Mij = d.cumsum() - d/2
        inner = lj / float(N) * (N*Mij - Bj*n[i])**2 / (Bj*(N - Bj) - N*lj/4.)
        A2akN += np.nan_to_num(inner).sum() / n[i]
        ########################################
        if DEBUG:
            statistic_seq.append(A2akN*(N-1.)/N)
        ########################################
    A2akN *= (N - 1.) / N
    return A2akN

def _anderson_ksamp_midrank(samples, Z, Zstar, k, n, N):
    """
    Compute A2akN equation 7 of Scholz and Stephens.

    Parameters
    ----------
    samples : sequence of 1-D array_like
        Array of sample arrays.
    Z : array_like
        Sorted array of all observations.
    Zstar : array_like
        Sorted array of unique observations.
    k : int
        Number of samples.
    n : array_like
        Number of observations in each sample.
    N : int
        Total number of observations.

    Returns
    -------
    A2aKN : float
        The A2aKN statistics of Scholz and Stephens 1987.
    """
    #########################################
    if DEBUG:
        global statistic_seq
        statistic_seq = []
    #########################################
    A2akN = 0.
    Z_ssorted_left = Z.searchsorted(Zstar, 'left')
    if N == Zstar.size:
        lj = 1.
    else:
        lj = Z.searchsorted(Zstar, 'right') - Z_ssorted_left
    Bj = Z_ssorted_left + lj / 2.
    for i in np.arange(0, k):
        s = np.sort(samples[i])
        s_ssorted_right = s.searchsorted(Zstar, side='right')
        Mij = s_ssorted_right.astype(float)
        fij = s_ssorted_right - s.searchsorted(Zstar, 'left')
        Mij -= fij / 2.
        inner = lj / float(N) * (N*Mij - Bj*n[i])**2 / (Bj*(N - Bj) - N*lj/4.)
        A2akN += inner.sum() / n[i]
        ########################################
        if DEBUG:
            statistic_seq.append(A2akN*(N-1.)/N)
        ########################################
    A2akN *= (N - 1.) / N
    return A2akN


def _anderson_ksamp_right(samples, Z, Zstar, k, n, N):
    """
    Compute A2akN equation 6 of Scholz & Stephens.

    Parameters
    ----------
    samples : sequence of 1-D array_like
        Array of sample arrays.
    Z : array_like
        Sorted array of all observations.
    Zstar : array_like
        Sorted array of unique observations.
    k : int
        Number of samples.
    n : array_like
        Number of observations in each sample.
    N : int
        Total number of observations.

    Returns
    -------
    A2KN : float
        The A2KN statistics of Scholz and Stephens 1987.
    """

    A2kN = 0.
    lj = Z.searchsorted(Zstar[:-1], 'right') - Z.searchsorted(Zstar[:-1],  'left')
    Bj = lj.cumsum()
    for i in range(0, k):
        s = np.sort(samples[i])
        Mij = s.searchsorted(Zstar[:-1], side='right')
        inner = lj / float(N) * (N * Mij - Bj * n[i])**2 / (Bj * (N - Bj))
        A2kN += inner.sum() / n[i]
    return A2kN


Anderson_ksampResult = namedtuple('Anderson_ksampResult', ('statistic', 'pvalue', 'ndf'))
def anderson_ksamp(data1, data2, binned=False, midrank=True):
    """The Anderson-Darling test for 2 samples.

    It tests the null hypothesis
    that the 2 samples are drawn from the same population without
    having to specify the distribution function of that population.
    The critical values depend on the number of samples.

    Parameters
    ----------
    samples : sequence of 1-D array_like
        Array of sample data in arrays.
    
    Returns
    -------
    statistic : float
        AD statistic
    pvalue : float
        two-tailed p-value
    ndf : NaN
        「Degrees of freedom」: the degrees of freedom for the p-value.
        Always NaN in this case.

    Raises
    ------
    ValueError
        If less than 2 samples are provided, a sample is empty, or no
        distinct observations are in the samples.

    See Also
    --------
    ks_2samp : 2 sample Kolmogorov-Smirnov test

    Notes
    -----
    This code is modified from scipy.stats.anderson and extended with
    supporting on frequency data. See [1]_ for further information.

    [2]_ Defines three versions of the k-sample Anderson-Darling test:
    one for continuous distributions and two for discrete
    distributions, in which ties between samples may occur. The
    default of this routine is to compute the version based on the
    midrank empirical distribution function. This test is applicable
    to continuous and discrete data. If midrank is set to False, the
    right side empirical distribution is used for a test for discrete
    data. According to [1]_, the two discrete test statistics differ
    only slightly if a few collisions due to round-off errors occur in
    the test not adjusted for ties between samples.

    References
    ----------
    .. [1] scipy.stats.anderson — SciPy Reference Guide
           http://docs.scipy.org/doc/scipy/reference/generated/scipy.stats.anderson.html
    .. [2] Scholz, F. W and Stephens, M. A. (1987), K-Sample
           Anderson-Darling Tests, Journal of the American Statistical
           Association, Vol. 82, pp. 918-924.

    Examples
    --------
    >>> from scipy import stats
    >>> np.random.seed(314159)

    The null hypothesis that the two random samples come from the same
    distribution can be rejected at the 5% level because the returned
    test value is greater than the critical value for 5% (1.961) but
    not at the 2.5% level. The interpolation gives an approximate
    significance level of 3.1%:

    >>> stats.anderson_ksamp([np.random.normal(size=50),
    ... np.random.normal(loc=0.5, size=30)])
    (2.4615796189876105,
      array([ 0.325,  1.226,  1.961,  2.718,  3.752]),
      0.03134990135800783)


    The null hypothesis cannot be rejected for three samples from an
    identical distribution. The approximate p-value (87%) has to be
    computed by extrapolation and may not be very accurate:

    >>> stats.anderson_ksamp([np.random.normal(size=50),
    ... np.random.normal(size=30), np.random.normal(size=20)])
    (-0.73091722665244196,
      array([ 0.44925884,  1.3052767 ,  1.9434184 ,  2.57696569,  3.41634856]),
      0.8789283903979661)

    """
    samples = [data1, data2]
    k = len(samples)
    if (k < 2):
        raise ValueError("anderson_ksamp needs at least two samples")

    samples = list(map(np.asarray, samples))
    if binned:
        Z = Zstar = sum(samples)
        n = np.array([sample.sum() for sample in samples])
        N = n.sum()
        A2kN = _anderson_ksamp_midrank_binned(samples, Z, Zstar, k, n, N)
    else:
        Z = np.sort(np.hstack(samples))
        N = Z.size
        Zstar = np.unique(Z)
        if Zstar.size < 2:
            raise ValueError("anderson_ksamp needs more than one distinct "
                             "observation")

        n = np.array([sample.size for sample in samples])
        if any(n == 0):
            raise ValueError("anderson_ksamp encountered sample without "
                             "observations")

        if midrank:
            A2kN = _anderson_ksamp_midrank(samples, Z, Zstar, k, n, N)
        else:
            A2kN = _anderson_ksamp_right(samples, Z, Zstar, k, n, N)

    H = (1. / n).sum()
    hs_cs = (1. / np.arange(N - 1, 1, -1)).cumsum()
    h = hs_cs[-1] + 1
    g = (hs_cs / np.arange(2, N)).sum()

    a = (4*g - 6) * (k - 1) + (10 - 6*g)*H
    b = (2*g - 4)*k**2 + 8*h*k + (2*g - 14*h - 4)*H - 8*h + 4*g - 6
    c = (6*h + 2*g - 2)*k**2 + (4*h - 4*g + 6)*k + (2*h - 6)*H + 4*h
    d = (2*h + 6)*k**2 - 4*h*k
    sigmasq = (a*N**3 + b*N**2 + c*N + d) / ((N - 1.) * (N - 2.) * (N - 3.))
    m = k - 1
    A2 = (A2kN - m) / np.sqrt(sigmasq)

    # The b_i values are the interpolation coefficients from Table 2
    # of Scholz and Stephens 1987
    b0 = np.array([0.675, 1.281, 1.645, 1.96, 2.326])
    b1 = np.array([-0.245, 0.25, 0.678, 1.149, 1.822])
    b2 = np.array([-0.105, -0.305, -0.362, -0.391, -0.396])
    critical = b0 + b1 / np.sqrt(m) + b2 / m
    pf = np.polyfit(critical, np.log(np.array([0.25, 0.1, 0.05, 0.025, 0.01])), 2)
    if A2 < critical.min() or A2 > critical.max():
        warnings.warn("approximate p-value will be computed by extrapolation")

    p = np.exp(np.polyval(pf, A2))
    return Anderson_ksampResult(A2, p, float('nan'))

def _log_fac(m,n):
    return np.log(np.arange(m,n+1)).sum()
@np.vectorize
def _log_binomial(n,k): 
    if k > (n-k):
        return (_log_fac(n-k+1,n)-_log_fac(2,k))
    else:
        return (_log_fac(k+1,n)-_log_fac(2,n-k))

def _loglikelihood(data1, data2, N_data1, N_data2, ratio, H = "H0"):
    t = data1 + data2
    logC = 0.
    if not ratio:
        logC = float(sum(log(binomial(t[i], data2[i])) for i in range(t.shape[0])))
    if H=="H1":
        a = data2/data1
    if H=="H0":
        a = N_data2 / N_data1
    ###################################################################
    if DEBUG:
        global statistic_seq
        statistic_seq = -2 * (logC - scipy.special.xlogy(t, 1+a) + scipy.special.xlogy(data2, a))
    ###################################################################
    return -2 * (logC - scipy.special.xlogy(t, 1+a).sum() + scipy.special.xlogy(data2, a).sum())

def _zloglikelihood(data1, data2, N_data1, N_data2, H = "H0"):
    z_not_in_both = (data1 == 0) ^ (data2 == 0)
    data1 = data1[z_not_in_both]
    data2 = data2[z_not_in_both]
    t = data1 + data2
    if H == "H0":
        p = N_data1 / (N_data1 + N_data2)
        #################################################################
        if DEBUG:
            global statistic_seq
            statistic_seq = np.concatenate([statistic_seq, -2 * scipy.special.xlogy(t[data2==0], p), scipy.special.xlogy(t[data1 == 0], 1-p)])
        #################################################################
        return -2 * (scipy.special.xlogy(t[data2==0], p).sum() + scipy.special.xlogy(t[data1 == 0], 1-p).sum())
    if H == "H1":
        return 0.

LikelihoodRatio_ksampResult = namedtuple('LikelihoodRatio_ksampResult', ('statistic', 'pvalue', 'ndf'))
def likelihoodratio_ksamp(data1, data2, normed=True, binned=True):
    """
    The likelihood-ratio test tests the null hypothesis that 2 given frequencies are
    drawn from the same distribution.
    
    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Observed frequencies in each category.
    normed : bool, optional, default: True
        If True/False, shape/value comparison test.
    binned : bool, optional, default: True
        If True/False, assume the type of input data is observed frequencies/samples.
    
    Returns
    -------
    statistic : float
        The llike-ratio test statistic.
    pvalue : float
        Corresponding p-value.
    ndf : int
        「Degrees of freedom」: the degrees of freedom for the p-value. The p-value
        is computed using a chi-squared distribution with k - 1 degrees of freedom,
        where k is the number of observed frequencies without bins has zero counts 
        in both histograms.
    """
    not_all_zeros = (data1 != 0) & (data2 != 0)
    ndf = sum((data1 != 0) | (data2 != 0))
    N_data1 = data1.sum()
    N_data2 = data2.sum()
    if normed:
        ndf -= 1
    llike_H0 = _loglikelihood(data1[not_all_zeros], data2[not_all_zeros], N_data1, N_data2, ratio=True, H="H0") + _zloglikelihood(data1[~not_all_zeros], data2[~not_all_zeros], N_data1, N_data2, H="H0")
    if DEBUG:
        global statistic_seq
        STATISTIC_SEQ = statistic_seq
    llike_H1 = _loglikelihood(data1[not_all_zeros], data2[not_all_zeros], N_data1, N_data2, ratio=True, H="H1")
    if DEBUG:
        STATISTIC_SEQ[:statistic_seq.shape[0]] -= statistic_seq
        statistic_seq = STATISTIC_SEQ
    llike_ratio = llike_H0 - llike_H1
    return LikelihoodRatio_ksampResult(llike_ratio, scipy.stats.chi2.sf(llike_ratio, ndf), ndf)

LikelihoodValue_ksampResult = namedtuple('LikelihoodValue_ksampResult', ('statistic', 'pvalue', 'ndf'))
def likelihoodvalue_ksamp(data1, data2, normed=True, binned=True):
    """
    The likelihood-value test tests the null hypothesis that 2 given frequencies are
    drawn from the same distribution.
    
    Parameters
    ----------
    data1, data2 : sequence of 1-D ndarrays
        Input data. Observed frequencies in each category.
    normed : bool, optional, default: True
        If True/False, shape/value comparison test.
    binned : bool, optional, default: True
        If True/False, assume the type of input data is observed frequencies/samples.
    
    Returns
    -------
    statistic : float
        The llike-value test statistic.
    pvalue : float
        Corresponding p-value.
    ndf : int
        「Degrees of freedom」: the degrees of freedom for the p-value. The p-value
        is computed using a chi-squared distribution with k - 1 degrees of freedom,
        where k is the number of observed frequencies without bins has zero counts 
        in both histograms.
    """
    no_zeros = (data1 != 0) | (data2 != 0)
    ndf = no_zeros.sum()
    if normed:
        ndf -= 1
    llike = _loglikelihood(data1[no_zeros], data2[no_zeros], data1.sum(), data2.sum(), ratio=False, H="H0") 
    return LikelihoodValue_ksampResult(llike, scipy.stats.chi2.sf(llike, ndf), ndf)
