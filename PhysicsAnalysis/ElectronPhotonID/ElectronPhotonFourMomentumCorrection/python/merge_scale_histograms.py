#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


from array import array
import logging
import ROOT
doc = """
This is an utility to merge histograms. The common case is when you have
old scale factors for the whole calorimeter and new scale factor only for
a subpart.
"""

ROOT.PyConfig.IgnoreCommandLineOptions = True
logging.basicConfig(level=logging.INFO)


def merge_histograms(old, new, merge_error=True):
    print("old binning: " + ", ".join(("%.3f" % old.GetBinLowEdge(ibin))
                                      for ibin in range(1, old.GetNbinsX()
                                                        + 2)))
    print("new binning: " + ", ".join(("%.3f" % new.GetBinLowEdge(ibin))
                                      for ibin in range(1, new.GetNbinsX()
                                                        + 2)))

    new_binning = []
    new_values = []
    new_errors = []
    UNDERFLOW = 0
    OVERFLOW = new.GetNbinsX() + 1

    for iold in range(1, old.GetNbinsX()):
        low = old.GetBinLowEdge(iold)
        r = low + old.GetBinWidth(iold)

        il_new = new.FindFixBin(low)
        ir_new = new.FindFixBin(r)
        remainer = None

        if il_new == UNDERFLOW and ir_new == UNDERFLOW:
            print("1. adding %.3f - %.3f from old" % (low, r))
            new_binning.append((low, r))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))

        elif il_new == UNDERFLOW and ir_new > UNDERFLOW:
            if abs(new.GetBinLowEdge(1) - low) < 1E-100:
                continue
            new_binning.append((low, new.GetBinLowEdge(1)))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))
            if ir_new == OVERFLOW:
                remainer = iold  # noqa: F841
            print("breaking")
            break
    last_old = iold

    for inew in range(1, new.GetNbinsX() + 1):
        low = new.GetBinLowEdge(inew)
        r = low + new.GetBinWidth(inew)
        print("2. adding %.3f - %.3f from new" % (low, r))
        new_binning.append((low, r))
        new_values.append(new.GetBinContent(inew))
        new_errors.append(new.GetBinError(inew))

    for iold in range(last_old, old.GetNbinsX() + 1):
        low = old.GetBinLowEdge(iold)
        r = low + old.GetBinWidth(iold)

        il_new = new.FindFixBin(low)
        ir_new = new.FindFixBin(r)

        if il_new == OVERFLOW and ir_new == OVERFLOW:
            print("4. adding %.3f - %.3f from old" % (low, r))
            new_binning.append((low, r))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))
        elif il_new < OVERFLOW and ir_new == OVERFLOW:
            if abs(new.GetBinLowEdge(new.GetNbinsX() + 1) - r) < 1E-100:
                continue
            new_binning.append((new.GetBinLowEdge(new.GetNbinsX() + 1), r))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))

    print(new_binning)
    new_edges = array('f', [x[0] for x in new_binning] + [new_binning[-1][1]])
    histo_type = type(new)
    result = histo_type(new.GetName(), new.GetTitle(),
                        len(new_edges) - 1, new_edges)
    for i, (v, e) in enumerate(zip(new_values, new_errors), 1):
        result.SetBinContent(i, v)
        if merge_error:
            result.SetBinError(i, e)

    print("merged binning: " + ", ".join(("%.3f" % result.GetBinLowEdge(ibin))
                                         for ibin in range(1, result.GetNbinsX()
                                         + 1)))

    return result


if __name__ == "__main__":
    try:
        ROOT.gROOT.ProcessLine(".x ~/atlasstyle/AtlasStyle.C")
        ROOT.SetAtlasStyle()
    except AttributeError:
        pass

    import argparse

    parser = argparse.ArgumentParser(description=doc,
                                     formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('histo_old')
    parser.add_argument('histo_new')
    parser.add_argument('--title', help='title of the new histogram')
    parser.add_argument('--name', help='name of the new histogram')
    parser.add_argument(
        '--recreate', help='create a new file', action='store_true')
    parser.add_argument('--output-filename', default='output.root')

    args = parser.parse_args()

    file_old = ROOT.TFile.Open(args.histo_old.split(":")[0])
    file_new = ROOT.TFile.Open(args.histo_new.split(":")[0])

    histo_old = file_old.Get(args.histo_old.split(":")[1])
    histo_new = file_new.Get(args.histo_new.split(":")[1])

    if not histo_old:
        raise IOError("cannot find histogram %s" % args.histo_old)
    if not histo_new:
        raise IOError("cannot find histogram %s" % args.histo_new)

    logging.info("going to merge %s with %s",
                 histo_old.GetName(), histo_new.GetName())
    histo_merged = merge_histograms(histo_old, histo_new)

    canvas = ROOT.TCanvas()
    histo_merged.SetFillColor(ROOT.kBlue)
    histo_old.SetMarkerColor(ROOT.kRed)
    histo_new.SetMarkerColor(ROOT.kGreen)
    for histo in histo_old, histo_new:
        histo.SetMarkerStyle(20)
        histo.SetMarkerSize(1)
    histo_merged.Draw("histo")
    histo_old.Draw("Psame")
    histo_new.Draw("Psame")

    legend = ROOT.TLegend(0.6, 0.7, 0.9, 0.9)
    legend.AddEntry(histo_old, "old")
    legend.AddEntry(histo_new, "new")
    legend.SetBorderSize(0)
    legend.Draw()

    fout = ROOT.TFile.Open(args.output_filename,
                           "recreate" if args.recreate else "update")
    if args.title is not None:
        histo_merged.SetTitle(args.title)
    name = args.name or histo_old.GetName()
    histo_merged.SetName(name)
    histo_merged.Write()

    input("press a key")
