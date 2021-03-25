#!/usr/bin/env python

import math
import ROOT as R

def setAtlasStyle():
    R.gROOT.LoadMacro("/afs/cern.ch/user/m/miokeefe/AtlasStyle/AtlasStyle.C")
    R.SetAtlasStyle()


def drawAtlasLabel(x, y, text):
    R.gROOT.LoadMacro("/afs/cern.ch/user/m/miokeefe/AtlasStyle/AtlasLabels.C")
    R.ATLASLabel(x, y, text,1)


def drawText(x, y, text):
    R.gROOT.LoadMacro("/afs/cern.ch/user/m/miokeefe/AtlasStyle/AtlasUtils.C")
    R.myText(x, y, 1, text)

def GetAveMass(h_os, histo, binlow, binhigh, colour):
    m_zCutHigh_elTP = 250 
    m_zCutLow_elTP  = 66
    nzbins_elTP = int(m_zCutHigh_elTP - m_zCutLow_elTP)
    
    ave_masses = {}
    errors     = {}
    for bcid_bin in range(1, h_os.GetNbinsY()+1):
        entries = 0
        temp_histo = R.TH1F("temp_histo", "temp_histo", nzbins_elTP, m_zCutLow_elTP, m_zCutHigh_elTP)
        temp_histo.GetXaxis().SetRangeUser(binlow, binhigh)
        for mass_bin in range(1, h_os.GetNbinsX()+1):
            mass = 0
            for mu_bin in range(1, h_os.GetNbinsZ()+1):
                mass += h_os.GetBinContent(mass_bin, bcid_bin, mu_bin)
            temp_histo.SetBinContent(mass_bin, mass)

        if temp_histo.GetEntries() != 0:
            ave_masses[bcid_bin] = temp_histo.GetMean()
            errors[bcid_bin]     = temp_histo.GetStdDevError()

        del temp_histo

    for bcid_bin in ave_masses:
        histo.SetBinContent(bcid_bin, ave_masses[bcid_bin])
        histo.SetBinError(bcid_bin, errors[bcid_bin])
    
    histo.SetLineColor(colour)
    histo.SetMarkerColor(colour)
    histo.GetYaxis().SetRangeUser(89.6, 91)
    histo.GetYaxis().SetTitle("<m_{ee}> GeV")
    histo.GetXaxis().SetTitle("Position in train")
    
def DoFit(c1, histo): 
    funclo = R.TF1("funclo", "pol0", 0, 14)
    funchi = R.TF1("funchi", "pol0", 14, 48)
    funclo.SetLineStyle(2)
    funchi.SetLineStyle(2)

    histo_clone = histo.Clone()
    histo_clone.Draw("same p")
    histo.Fit(funclo, "R")
    histo_clone.Fit(funchi, "R")


def main():
    rfile = R.TFile("output_merged.root")

    h_os = rfile.Get("run_merged/GLOBAL/DQTGlobalWZFinder/m_ele_tight_bad_os_BCID_pileup")

    m_zCutHigh_elTP = 250 
    m_zCutLow_elTP  = 66
    nzbins_elTP = int(m_zCutHigh_elTP - m_zCutLow_elTP)

    h_BCID1 = R.TH1F("h_BCID1", "h_BCID1", nzbins_elTP, m_zCutLow_elTP, m_zCutHigh_elTP)
    h_BCID2t48 = R.TH1F("h_BCID2t48", "h_BCID2t48", nzbins_elTP, m_zCutLow_elTP, m_zCutHigh_elTP)
    h_BCID1.GetXaxis().SetRangeUser(70, 110) 
    h_BCID2t48.GetXaxis().SetRangeUser(70, 110) 
    h_BCID1.GetXaxis().SetTitle("Invariant mass [GeV]")
    h_BCID1.GetYaxis().SetTitle("Entries")
    h_BCID2t48.GetXaxis().SetTitle("Invariant mass [GeV]")
    h_BCID2t48.GetYaxis().SetTitle("Entries")

    # Invariant mass distribution of first BCID
    for xbin in range(1, h_os.GetNbinsX()+1):
        mass = 0
        for zbin in range(1, h_os.GetNbinsZ()+1):
            mass += h_os.GetBinContent(xbin, 1, zbin)

        h_BCID1.SetBinContent(xbin, mass)
    
    # Invariant mass distribution of all other BCIDs
    for xbin in range(1, h_os.GetNbinsX()+1):
        mass = 0
        for ybin in range(2, h_os.GetNbinsY()+1):
            for zbin in range(1, h_os.GetNbinsZ()+1):
                mass += h_os.GetBinContent(xbin, ybin, zbin)

        h_BCID2t48.SetBinContent(xbin, mass)
    
    # Average invariant mass across all BCID range
    h_81_101 = R.TH1F("h_81_101", "h_81_101", 48, 1, 48)
    GetAveMass(h_os, h_81_101, 81, 101, R.kBlack)
    
    h_86_96 = R.TH1F("h_86_96", "h_86_96", 48, 1, 48)
    GetAveMass(h_os, h_86_96, 86, 96, R.kRed)
    
    h_76_106 = R.TH1F("h_76_106", "h_76_106", 48, 1, 48)
    GetAveMass(h_os, h_76_106, 76, 106, R.kBlue)

    leg = R.TLegend(0.75, 0.6, 0.85, 0.80)
    leg.SetBorderSize(0)
    leg.SetTextSize(0.03)
    leg.AddEntry(h_76_106, "76 < m_{ee} < 106 GeV", "ep")
    leg.AddEntry(h_81_101, "81 < m_{ee} < 101 GeV", "ep")
    leg.AddEntry(h_86_96,  "86 < m_{ee} < 96 GeV", "ep")


    c1 = R.TCanvas()
    h_76_106.Draw("ep")
    DoFit(c1, h_76_106)
    h_81_101.Draw("same p")
    DoFit(c1, h_81_101)
    h_86_96.Draw("same p")
    DoFit(c1, h_86_96)
    leg.Draw()
    drawAtlasLabel(0.17, 0.85, "Work in progess")
    c1.SaveAs("Z_inv_mass_ave_BCID.eps")


    c1 = R.TCanvas()
    h_BCID1.Draw("ep")
    R.gPad.SetLogy()
    drawAtlasLabel(0.17, 0.85, "Work in progess")
    drawText(0.17, 0.78, "BCID 1")
    c1.SaveAs("Z_inv_mass_BCID1.eps")
    
    c1 = R.TCanvas()
    h_BCID2t48.Draw("ep")
    R.gPad.SetLogy()
    drawAtlasLabel(0.17, 0.85, "Work in progess")
    drawText(0.17, 0.78, "BCID 2-48")
    c1.SaveAs("Z_inv_mass_BCID2t48.eps")

    h_BCID1.Scale(1/h_BCID1.Integral())
    h_BCID2t48.Scale(1/h_BCID2t48.Integral())

    leg = R.TLegend(0.2, 0.6, 0.4, 0.8)
    leg.SetBorderSize(0)
    leg.AddEntry(h_BCID1, "BCID 1")
    leg.AddEntry(h_BCID2t48, "BCID 2-48")

    c1 = R.TCanvas()
    h_BCID1.Draw("ep")
    h_BCID2t48.SetLineColor(R.kRed)
    h_BCID2t48.SetMarkerColor(R.kRed)
    h_BCID2t48.Draw("same, ep")
    leg.Draw()
    drawAtlasLabel(0.17, 0.85, "Work in progess")
    c1.SaveAs("Z_inv_mass_norm.eps")

    
    h_BCID1.Divide(h_BCID2t48)
    h_BCID1.GetYaxis().SetTitle("Ratio")
    h_BCID1.SetMarkerColor(R.kBlack)
    h_BCID1.SetLineColor(R.kBlack)
    line = R.TLine(70, 1, 110, 1)
    line.SetLineColor(R.kRed)
    c1 = R.TCanvas()
    h_BCID1.Draw("ep")
    drawAtlasLabel(0.17, 0.85, "Work in progess")
    drawText(0.17, 0.75, "#frac{BCID 1}{BCID 2-48}")
    line.Draw()
    c1.SaveAs("Z_inv_mass_norm_ratio.eps")


if __name__ == "__main__":
    R.gROOT.SetBatch(R.kTRUE)
    setAtlasStyle()
    main()
