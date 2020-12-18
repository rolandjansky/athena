# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
this small script dumps the ratio raw-Ex / raw-E where x=0,1,2,3 for converted,
uncoverted and electrons from single particle (you need to check the paths).
For converted photon is dumps also the radius of conversion.

This file and its input are not needed for the tool.

The output of this tool is needed to generate single particle from scratch and
then to compute the decomposition of the covariance matrix.
"""

import ROOT
from glob import glob
from array import array


def main(path, particle):
    files = glob(path)
    chain = ROOT.TChain("egamma")
    for f in files:
        chain.Add(f)
    print("entries:", chain.GetEntries())

    selection = {"unconv": "(ph_Rconv >= 800 || ph_Rconv <= 0)",
                 "conv": "(ph_Rconv < 800 && ph_Rconv > 0)",
                 "electron": "1"}[particle]

    prefix = {"unconv": "ph", "conv": "ph", "electron": "el"}[particle]

    layers = "rawcl_Es0", "rawcl_Es1", "rawcl_Es2", "rawcl_Es3"
    layers = [prefix + "_" + v for v in layers]
    title = ["ratio_Es0_true_E",
             "ratio_Es1_true_E",
             "ratio_Es2_true_E",
             "ratio_Es3_true_E"]
    aetaCalo = "abs(" + prefix + "_" + "cl_etaCalo" + ")"
    truth_E = prefix + "_truth_E"
    ratio_layers = ["%s / %s" % (layer, truth_E) for layer in layers]
    truth_eta = prefix + "_truth_eta"
    truth_pt = "(%s / cosh(%s))" % (truth_E, truth_eta)
    matched = ("abs(({0}_rawcl_Es0 + "
               "{0}_rawcl_Es1 + {0}_rawcl_Es2 + "
               "{0}_rawcl_Es3)/{0}_truth_E - 1)<1").format(prefix)
    vars_to_plot = ratio_layers
    selection += " && " + "(" + matched + ")"

    pt_binning = [0, 5E3, 10E3, 15E3, 20E3,
                  30E3, 35E3, 40E3, 45E3, 50E3, 55E3, 60E3, 65E3,
                  70E3, 75E3, 80E3, 85E3, 90E3,
                  100E3, 110E3, 120E3, 130E3,
                  140E3, 160E3, 180E3, 200E3, 220E3,
                  250E3, 300E3, 350E3, 400E3, 450E3, 500E3, 550E3, 600E3,
                  700E3, 800E3, 900E3, 1000E3,
                  1200E3, 1400E3, 1600E3, 1800E3, 2000E3,
                  2500E3]
    aeta_binning = [0, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                    0.7, 0.75, 0.8, 0.85, 0.9, 1.0,
                    1.1, 1.2, 1.3, 1.425, 1.550, 1.6,
                    1.65, 1.7, 1.75, 1.8, 1.9, 2.0, 2.1,
                    2.2, 2.3, 2.35, 2.4, 2.5]
    pt_binning = array('d', pt_binning)
    aeta_binning = array('d', aeta_binning)
    if particle == "conv":
        vars_to_plot.append("ph_Rconv")
        title.append("ph_Rconv")
        vars_to_plot.append("ph_zconv")
        title.append("ph_zconv")

    print("plotting counting")
    histo_name = "histo_count_%s" % particle
    histo_count = ROOT.TH2F(histo_name, "count %s" % particle, len(
        pt_binning) - 1, pt_binning, len(aeta_binning) - 1, aeta_binning)
    chain.Draw(aetaCalo + ":" + truth_pt + ">>" + histo_name, selection)

    result = []
    for t, v in zip(title, vars_to_plot):
        print("plotting", v)
        histo_name = "histo_%s_%s" % (particle, t)
        if "zconv" in v:
            sel = selection + " && abs(ph_zconv) < 5000"
        else:
            sel = selection
        histo = ROOT.TProfile2D(histo_name, t, len(
            pt_binning) - 1, pt_binning, len(aeta_binning) - 1, aeta_binning)
        arg = v + ":" + aetaCalo + ":" + truth_pt + ">>" + histo_name
        chain.Draw(arg, sel, "profcolz")
        histo.GetXaxis().SetTitle("true pT [MeV]")
        histo.GetYaxis().SetTitle("etaCalo")
        result.append(histo)
    result.append(histo_count)
    return result


if __name__ == "__main__":
    output_file = ROOT.TFile.Open("average_layers.root", "recreate")
    result = main(
        "/storage_tmp/atlas/MVA2015/inputs_MC15/"
        "photon_grid_v5/data-output/*/*.root",
        "unconv")
    result += main("/storage_tmp/atlas/MVA2015/inputs_MC15/"
                   "photon_grid_v5/data-output/*/*.root",
                   "conv")
    result += main("/storage_tmp/atlas/MVA2015/inputs_MC15/"
                   "electron_local_v5.1/data-output/mc15_electron.root",
                   "electron")
    for r in result:
        r.Write()
