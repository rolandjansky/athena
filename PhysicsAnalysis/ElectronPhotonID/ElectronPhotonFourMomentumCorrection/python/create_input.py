# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# this file do not work out of the box

import shutil
import ROOT
from array import array
import numpy as np

try:
    ROOT.gROOT.ProcessLine(".x ~/atlasstyle/AtlasStyle.C")
    ROOT.SetAtlasStyle()
except AttributeError:
    pass

old_filename = "/home/turra/simon/latest_simon.root"
old_file = ROOT.TFile.Open(old_filename)
old_scale_histogram = old_file.Get("Scales/es2012c/alphaZee_errStat")
old_ct_histogram = old_file.Get("Resolution/es2012c/ctZee_errStat")
old_scale_sys = old_file.Get("Scales/es2012c/alphaZee_errSyst")
old_ct_sys = old_file.Get("Resolution/es2012c/ctZee_errSyst")

assert old_scale_histogram
assert old_ct_histogram
assert old_scale_sys
assert old_ct_sys

file_christophe = ROOT.TFile("~/ElectronEnergyScaleFactor.root")
scales_christophe = file_christophe.Get("alpha68")
ct_christophe = file_christophe.Get("sigma24")
# sum of 8 / 13 TeV diff + 64 / 32 bins diff
scales_sys_christophe = file_christophe.Get("systAlpha")
ct_sys_christophe = file_christophe.Get("systSigma")  # 8 / 13 TeV diff

assert scales_christophe
assert ct_christophe
assert scales_sys_christophe
assert ct_sys_christophe


def qsum_histograms(histo1, histo2):
    new_histo = histo1.Clone()
    new_histo.Reset()
    for ibin in range(histo1.GetNbinsX() + 2):
        value1 = histo1.GetBinContent(ibin)
        central_value = histo1.GetBinCenter(ibin)
        ibin2 = histo2.FindBin(central_value)
        value2 = histo2.GetBinContent(ibin2)
        new_value = np.sqrt(value1 * value1 + value2 * value2)
        new_histo.SetBinContent(ibin, new_value)
    return new_histo


def copy_bin(histo, eta_range):
    """ avoid bin with value == 0 """
    empty_bins_in_transition = set()
    for eta in eta_range:
        bin = histo.FindBin(eta)
        if (histo.GetBinContent(bin) == 0):
            empty_bins_in_transition.add(bin)
    assert len(empty_bins_in_transition) == 1
    empty_bin = list(empty_bins_in_transition)[0]
    if eta_range[-1] > 0:
        assert(histo_scale.GetBinContent(empty_bin - 1) != 0)
        histo.SetBinContent(empty_bin, histo.GetBinContent(empty_bin - 1))
        histo.SetBinError(empty_bin, histo.GetBinError(empty_bin - 1))
    else:
        assert(histo_scale.GetBinContent(empty_bin + 1) != 0)
        histo.SetBinContent(empty_bin, histo.GetBinContent(empty_bin + 1))
        histo.SetBinError(empty_bin, histo.GetBinError(empty_bin + 1))


def copy_dir(source, destination):
    # equivalent of cp -r source/* destination
    savdir = ROOT.gDirectory
    destination.cd()
    # loop on all entries of this directory
    for key in source.GetListOfKeys():
        classname = key.GetClassName()
        cl = ROOT.gROOT.GetClass(classname)
        if (not cl):
            continue
        if (cl.InheritsFrom(ROOT.TDirectory.Class())):
            d = destination.mkdir(key.GetName())
            copy_dir(source.GetDirectory(key.GetName()), d)
            destination.cd()
        elif (cl.InheritsFrom(ROOT.TTree.Class())):
            T = source.Get(key.GetName())
            destination.cd()
            newT = T.CloneTree(-1, "fast")
            newT.Write()
        elif (cl.InheritsFrom(ROOT.TList.Class())):
            source.cd()
            obj = key.ReadObj()
            destination.cd()
            obj.Write(obj.GetName(), ROOT.TObject.kSingleKey)
            del obj
        else:
            source.cd()
            obj = key.ReadObj()
            destination.cd()
            obj.Write()
            del obj

    destination.SaveSelf(True)
    savdir.cd()


def create_structured_file(path):
    output_file = ROOT.TFile(path, "recreate")
    output_file.mkdir("Scales")
    output_file.mkdir("Resolution")
    output_file.mkdir("PSRecalibration")
    output_file.mkdir("S1Recalibration")
    return output_file


def create_new_directories(output_file):
    output_file.Get("Scales").mkdir("es2015PRE")
    output_file.Get("Resolution").mkdir("es2015PRE")
    output_file.Get("PSRecalibration").mkdir("es2015PRE")
    output_file.Get("S1Recalibration").mkdir("es2015PRE")


def merge_histograms(old, new, merge_error=True):
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
            new_binning.append((low, r))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))

        elif il_new == UNDERFLOW and ir_new > UNDERFLOW and ir_new < OVERFLOW:
            if abs(new.GetBinLowEdge(1) - low) < 1E-100:
                continue
            new_binning.append((low, new.GetBinLowEdge(1)))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))
            if ir_new == OVERFLOW:
                remainer = iold
            break
    last_old = iold

    for inew in range(1, new.GetNbinsX() + 1):
        low = new.GetBinLowEdge(inew)
        r = low + new.GetBinWidth(inew)
        new_binning.append((low, r))
        new_values.append(new.GetBinContent(inew))
        new_errors.append(new.GetBinError(inew))

    if remainer is not None:
        new_binning.append((new.GetBinLowEdge(new.GetNbinsX()),
                            old.GetBinLowEdge(remainer)
                            + old.GetBinWidth(remainer)))
        new_values.append(old.GetBinContent(remainer))
        new_errors.append(old.GetBinError(remainer))

    for iold in range(last_old, old.GetNbinsX() + 1):
        low = old.GetBinLowEdge(iold)
        r = low + old.GetBinWidth(iold)

        il_new = new.FindFixBin(low)
        ir_new = new.FindFixBin(r)

        if il_new == OVERFLOW and ir_new == OVERFLOW:
            new_binning.append((low, r))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))
        elif il_new < OVERFLOW and ir_new == OVERFLOW:
            if abs(new.GetBinLowEdge(new.GetNbinsX() + 1) - r) < 1E-100:
                continue
            new_binning.append((new.GetBinLowEdge(new.GetNbinsX() + 1), r))
            new_values.append(old.GetBinContent(iold))
            new_errors.append(old.GetBinError(iold))

    new_edges = array('f', [x[0] for x in new_binning] + [new_binning[-1][1]])
    histo_type = type(new)
    result = histo_type(new.GetName(), new.GetTitle(),
                        len(new_edges) - 1, new_edges)
    for i, (v, e) in enumerate(zip(new_values, new_errors), 1):
        result.SetBinContent(i, v)
        if merge_error:
            result.SetBinError(i, e)
    return result


histo_scale = merge_histograms(old_scale_histogram, scales_christophe)
histo_ct = merge_histograms(old_ct_histogram, ct_christophe)


copy_bin(histo_scale, np.arange(2, 2.7, 0.001))
copy_bin(histo_scale, np.arange(-2.7, -2, 0.001))

copy_bin(histo_ct, np.arange(2, 2.7, 0.001))
copy_bin(histo_ct, np.arange(-2.7, -2, 0.001))


histo_scale.SetTitle("Zee energy scale factor - es2015 prerecommendation")
histo_ct.SetTitle("Zee additional constant term - es2015 prerecommendation")

for h in histo_scale, histo_ct:
    h.SetMarkerColor(ROOT.kBlack)
    h.SetMarkerStyle(20)

canvas1 = ROOT.TCanvas()
histo_scale.Draw("")
old_scale_histogram.Draw("sameL")
scales_christophe.Draw("sameL")

canvas2 = ROOT.TCanvas()
histo_ct.Draw("L")
old_ct_histogram.Draw("sameL")
ct_christophe.Draw("sameL")

histo_scale.SetName("alphaZee_errStat")
histo_ct.SetName("ctZee_errStat")

# this created a file structured as the official one, with empty directories
# output_file = create_structured_file("calibration_constants_run2.root")
print(old_filename)
shutil.copy2(old_filename, "xxx.root")
output_file = ROOT.TFile("xxx.root", "update")
create_new_directories(output_file)

output_file.GetDirectory("Scales").GetDirectory(
    "es2015PRE").WriteObject(histo_scale, "alphaZee_errStat")
output_file.GetDirectory("Resolution").GetDirectory(
    "es2015PRE").WriteObject(histo_ct, "ctZee_errStat")

# systematics

new_scale_sys = qsum_histograms(scales_sys_christophe, old_scale_sys)
new_ct_sys = qsum_histograms(ct_sys_christophe, old_ct_sys)
new_scale_sys = merge_histograms(old_scale_sys, new_scale_sys, False)
new_ct_sys = merge_histograms(old_ct_sys, new_ct_sys, False)
new_scale_sys.SetTitle(
    "es2015PRE scales sys = es2012c sys + 7/13 TeV diff + 34/68 bin diff")
new_ct_sys.SetTitle("es2015 ct sys = es2012c sys + 7/13 TeV diff")
output_file.GetDirectory("Scales").GetDirectory(
    "es2015PRE").WriteObject(new_scale_sys, "alphaZee_errSyst")
output_file.GetDirectory("Resolution").GetDirectory(
    "es2015PRE").WriteObject(new_ct_sys, "ctZee_errSyst")

legend3 = ROOT.TLegend(0.6, 0.7, 0.9, 0.9)
legend3.SetBorderSize(0)
canvas3 = ROOT.TCanvas()
old_scale_sys.GetXaxis().SetTitle("#eta")
old_scale_sys.GetYaxis().SetTitle("scale systematics")
old_scale_sys.SetLineColor(ROOT.kRed)
old_scale_sys.Draw()
scales_sys_christophe.SetLineColor(ROOT.kBlue)
scales_sys_christophe.Draw("same")
new_scale_sys.Draw("same")
legend3.AddEntry(scales_sys_christophe,  "8/13 TeV diff + 34/68 bin diff", "L")
legend3.AddEntry(old_scale_sys, "2012c systematics", "L")
legend3.AddEntry(new_scale_sys, "sum (2015PRE)", "L")
legend3.Draw()

legend4 = ROOT.TLegend(0.6, 0.7, 0.9, 0.9)
legend4.SetBorderSize(0)
canvas4 = ROOT.TCanvas()
old_ct_sys.GetXaxis().SetTitle("#eta")
old_ct_sys.GetYaxis().SetTitle("ct systematics")
old_ct_sys.SetLineColor(ROOT.kRed)
old_ct_sys.Draw()
ct_sys_christophe.SetLineColor(ROOT.kBlue)
ct_sys_christophe.Draw("same")
new_ct_sys.Draw("same")
legend4.AddEntry(ct_sys_christophe,  "8/13 TeV diff", "L")
legend4.AddEntry(old_ct_sys, "2012c systematics", "L")
legend4.AddEntry(new_ct_sys, "sum (2015PRE)", "L")
legend4.Draw()

# stefano input for sensitivity

stefano_file = ROOT.TFile("egammaEnergyCorrectionDataMC15.root")
copy_dir(stefano_file.GetDirectory("PSRecalibration"),
         output_file.GetDirectory("PSRecalibration").GetDirectory("es2015PRE"))
copy_dir(stefano_file.GetDirectory("S1Recalibration"),
         output_file.GetDirectory("S1Recalibration").GetDirectory("es2015PRE"))

# correction for uA2MeV first week 2015

f_ua2mev = ROOT.TFile.Open("~/uA2MeV.root")
histo_ua2mev = f_ua2mev.Get("histo_uA2MeV_week12")
output_file.Get("Scales").Get("es2015PRE").WriteTObject(histo_ua2mev)

input("Press Key")
