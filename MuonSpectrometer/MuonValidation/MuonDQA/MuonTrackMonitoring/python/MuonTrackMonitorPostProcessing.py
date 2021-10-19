"""
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 2021 Peter Kraemer - Uni Mainz
"""


"""Implement functions for postprocessing used by histgrinder."""


def project_mean_ROOT(inputs):
    """Fit peak for every row in 2D histogram."""
    mean = inputs[0][1][0].ProjectionY().Clone()
    mean.Clear()
    sigma = inputs[0][1][0].ProjectionY().Clone()
    sigma.Clear()
    name = inputs[0][1][0].GetName()
    n_bins_y = inputs[0][1][0].GetNbinsY()
    for i in range(n_bins_y):
        tmp = inputs[0][1][0].ProjectionX(name, i,i).Clone()
        if tmp.GetEntries() == 0:
            print("zero entries in Projection")
            continue
        mean.SetBinContent(i, tmp.GetMean(1))
        mean.SetBinError(i, tmp.GetMeanError(1))
        sigma.SetBinContent(i, tmp.GetRMS(1))
        sigma.SetBinError(i, tmp.GetRMSError(1))
    return [mean, sigma]
