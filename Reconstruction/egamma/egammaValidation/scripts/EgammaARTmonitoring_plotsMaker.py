#!/usr/bin/env python
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#

import sys
from ROOT import gDirectory, gROOT, gStyle, kTRUE, kFALSE, \
    TCanvas, TFile, TH1, \
    TH1D, TLegend, TPad, kBlack, kBlue, kRed, kGreen, kOrange, kCyan, kPink

# gROOT.SetBatch(kTRUE)
gStyle.SetOptStat(0)

cluster_list = [

    {'name': 'clusterAll', 'title': 'Clusters - Inclusive'},
    {'name': 'cluster10GeV', 'title': 'Clusters - 10 GeV'},
    {'name': 'clusterPromptAll', 'title': 'Clusters from Prompt - Inclusive'},
    {'name': 'clusterPrompt10GeV', 'title': 'Clusters from Prompt  - 10 GeV'},


]
electron_comparison_list = [
    {'name': 'showerShapesAll', 'title': 'Shower Shape - Inclusive'},
    {'name': 'showerShapes10GeV', 'title': 'Shower Shape - 10 GeV'},
    {'name': 'isolationAll', 'title': 'Isolation'},
    {'name': 'recoElectronAll', 'title': 'Reconstructed Electron'},
    {'name': 'truthRecoElectronLooseLH', 'title': 'Reconstructed Electron LooseLH'},
    {'name': 'truthRecoElectronMediumLH', 'title': 'Reconstructed Electron MediumLH'},
    {'name': 'truthRecoElectronTightLH', 'title': 'Reconstructed Electron TightLH'},
    {'name': 'truthElectronAll', 'title': 'True Electron'},
    {'name': 'truthPromptElectronAll', 'title': 'True Prompt Electron'},
    {'name': 'truthElectronRecoElectronAll', 'title': 'True Electron Reconstructed as Electron'},
    {'name': 'truthPromptElectronWithTrack', 'title': 'True Prompt Electron with Track'},
    {'name': 'truthPromptElectronWithGSFTrack', 'title': 'True Prompt Electron with GSFTrack'},
    {'name': 'truthPromptElectronWithReco', 'title': 'True Prompt Electron with Reco Electron'},
    {'name': 'recoElectronIsoFixedCutTight', 'title': 'Reconstructed Electron FixedCutTight'},
    {'name': 'recoElectronIsoFixedCutTightTrackOnly', 'title': 'Reconstructed Electron FixedCutTightTrackOnly'},
    {'name': 'recoElectronIsoFixedCutLoose', 'title': 'Reconstructed Electron FixedCutLoose'},
    {'name': 'trackingEfficiency', 'title': 'Tracking Efficiency'},
    {'name': 'GSFEfficiency', 'title': 'GSF Efficiency'},
    {'name': 'matchingEfficiency', 'title': 'Matching  Efficiency'},
    {'name': 'reconstructionEfficiency', 'title': 'Reconstruction Efficiency'},
    {'name': 'recoElectronLooseLHEfficiency', 'title': 'Reconstructed Electron LooseLH Efficiency'},
    {'name': 'recoElectronMediumLHEfficiency', 'title': 'Reconstructed Electron MediumLH Efficiency'},
    {'name': 'recoElectronTightLHEfficiency', 'title': 'Reconstructed Electron TightLH Efficiency'},
    {'name': 'recoElectronIsoFixedCutTightEfficiency', 'title': 'Reconstructed Electron FixedCutTight Efficiency'},
    {'name': 'recoElectronIsoFixedCutTightTrackOnlyEfficiency', 'title': 'Reconstructed Electron FixedCutTighTrackOnly Efficiency'},
    {'name': 'recoElectronIsoFixedCutLooseEfficiency', 'title': 'Reconstructed Electron FixedCutLoose Efficiency'},
]

photon_comparison_list = [
    {'name': 'recoPhotonAll', 'title': 'Reconstructed Photon'},
    {'name': 'truthPhotonRecoPhoton', 'title': 'True photon reconstructed as photon'},
    {'name': 'truthConvPhoton', 'title': 'True converted photon'},
    {'name': 'truthConvRecoConv', 'title': 'True conversion reconstructed as converted photon'},
    {'name': 'truthConvRecoConv1Si', 'title': 'True conversion reconstructed as 1 Si conv'},
    {'name': 'truthConvRecoConv1TRT', 'title': 'True conversion reconstructed as 1 TRT conv'},
    {'name': 'truthConvRecoConv2Si', 'title': 'True conversion reconstructed as Si-Si conv'},
    {'name': 'truthConvRecoConv2TRT', 'title': 'True conversion reconstructed as TRT-TRT conv'},
    {'name': 'truthConvRecoConv2SiTRT', 'title': 'True conversion reconstructed as Si-TRT conv'},
    {'name': 'truthConvRecoUnconv', 'title': 'True conversion reconstructed as unconverted photon'},
    {'name': 'truthUnconvPhoton', 'title': 'True unconverted photon'},
    {'name': 'truthUnconvRecoConv', 'title': 'True unconverted reconstructed as conv photon'},
    {'name': 'truthUnconvRecoUnconv', 'title': 'True unconverted reconstructed as unconverted photon'},
    {'name': 'showerShapesAll', 'title': 'Shower Shape - Inclusive'},
    {'name': 'showerShapes10GeV', 'title': 'Shower Shape - 10 GeV'},
    {'name': 'isolationAll', 'title': 'Isolation'},
    {'name': 'recoPhotonUnconvIsoFixedCutTight', 'title': 'FixedCutTight Unconverted Photon'},
    {'name': 'recoPhotonUnconvIsoFixedCutTightCaloOnly', 'title': 'FixedCutTightCaloOnly Unconverted Photon'},
    {'name': 'recoPhotonUnconvIsoFixedCutLoose', 'title': 'FixedCutLoose Unconverted Photon'},
    {'name': 'recoPhotonConvIsoFixedCutTight', 'title': 'FixedCutTight Converted Photon'},
    {'name': 'recoPhotonConvIsoFixedCutTightCaloOnly', 'title': 'FixedCutTightCaloOnly Converted Photon'},
    {'name': 'recoPhotonConvIsoFixedCutLoose', 'title': 'FixedCutLoose Converted Photon'},
    {'name': 'truthPhotonUnconvRecoUnconvEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'truthPhotonRecoConvEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'recoPhotonUnconvIsoFixedCutTightEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'recoPhotonUnconvIsoFixedCutTightCaloOnlyEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'recoPhotonUnconvIsoFixedCutLooseEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'recoPhotonConvIsoFixedCutTightEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'recoPhotonConvIsoFixedCutTightCaloOnlyEfficiency', 'title': 'True Conv #rightarrow Conv'},
    {'name': 'recoPhotonConvIsoFixedCutLooseEfficiency', 'title': 'True Conv #rightarrow Conv'},
]

photon_fraction_list = [
    {'name': 'truthPhotonConvRecoConvEfficiency', 'color': kBlack, 'title': 'True Conv #rightarrow Conv'},
    {'name': 'truthPhotonConvRecoConv1SiEfficiency', 'color': kBlue + 2, 'title': 'True Conv #rightarrow 1 Si Conv'},
    {'name': 'truthPhotonConvRecoConv1TRTEfficiency', 'color': kRed + 2, 'title': 'True Conv #rightarrow 1 TRT Conv'},
    {'name': 'truthPhotonConvRecoConv2SiEfficiency', 'color': kGreen + 2, 'title': 'True Conv #rightarrow Si-Si Conv'},
    {'name': 'truthPhotonConvRecoConv2TRTEfficiency', 'color': kOrange + 2,
     'title': 'True Conv #rightarrow TRT-TRT Conv'},
    {'name': 'truthPhotonConvRecoConv2SiTRTEfficiency', 'color': kCyan + 2,
     'title': 'True Conv #rightarrow Si-TRT Conv'},
    {'name': 'truthPhotonConvRecoUnconvEfficiency', 'color': kPink + 2, 'title': 'True Conv #rightarrow Unconv'}
]


def get_key_names(file, directory=""):
    """
    Function to get the key elements name from a given directory
    :param file: TFile
    :param directory: Directory
    :return:
    """
    file.cd(directory)
    return [key.GetName() for key in gDirectory.GetListOfKeys()]


def make_comparison_plots(type, f_base, f_nightly, result_file):
    """

    :param type: electron or gamma
    :param f_base: TFile with the baseline plots
    :param f_nightly: TFile with the nightly plots
    :param result_file: TFile with the resulting comparison
    """
    comparison_list = photon_comparison_list if type == 'gamma' else electron_comparison_list
    for folder in comparison_list:
        for histo in get_key_names(f_nightly, folder['name']):
            h_base = f_base.Get(folder['name'] + '/' + histo)
            h_nightly = f_nightly.Get(folder['name'] + '/' + histo)
            make_ratio_plot(h_base, h_nightly, folder['title'], result_file)


def make_profile_plots(f_base, f_nightly, result_file):

    for i, folder in enumerate(cluster_list):
        for histo in get_key_names(f_nightly, folder['name']):
            print(histo)
            if '2D' not in histo:
                continue
            h_base = f_base.Get(folder['name'] + '/' + histo)
            h_base_profile = h_base.ProfileX(histo+"_ProfileB")
            h_nightly = f_nightly.Get(folder['name'] + '/' + histo)
            h_nightly_profile = h_nightly.ProfileX(histo+"_Profile")
            h_base_profile.SetDirectory(0)
            h_nightly_profile.SetDirectory(0)

            make_ratio_plot(h_base_profile, h_nightly_profile, folder['title'], result_file, "Mean E_{raw}/E_{truth}")




def make_photon_fraction_plot(f_base, f_nightly, result_file):
    """
    This functions created a photon validation plot with efficiencies
    and fractions

    :param f_base TFile with the baseline histograms:
    :param f_nightly TFile with the nightly histograms:
    """
    for histo in get_key_names(f_nightly, 'truthPhotonConvRecoConvEfficiency'):

        variable_name = histo.split("_", 1)[1]

        c1 = TCanvas()

        leg = TLegend(0.1, 0.75, 0.9, 0.9)
        leg.SetNColumns(2)

        for i, folder in enumerate(photon_fraction_list):

            baseline = f_base.Get(folder['name'] + '/' + folder['name'] + "_" + variable_name)
            baseline.SetDirectory(0)
            nightly = f_nightly.Get(folder['name'] + '/' + folder['name'] + "_" + variable_name)
            nightly.SetDirectory(0)

            baseline.GetYaxis().SetTitle("Efficiency and fraction")

            baseline.SetLineColor(folder['color'])
            nightly.SetLineColor(folder['color'])
            baseline.SetMarkerColor(folder['color'])
            nightly.SetMarkerColor(folder['color'])

            baseline.SetMarkerStyle(1)
            nightly.SetMarkerStyle(20)

            leg.AddEntry(nightly, folder['title'], "p")

            if i == 0:
                baseline.Draw("hist ")
            else:
                baseline.Draw("same hist")

            nightly.Draw("p same")

        leg.Draw()

        c1.Update()

        result_file.cd()

        c1.SaveAs("ConvertionEff_" + variable_name + ".png")

        c1.Write("ConvertionEff_" + variable_name)


def make_ratio_plot(h_base, h_nightly, name, result_file, y_axis_label=None):
    """

    :param h_base: Baseline histogram
    :param h_nightly: Nightly histogram
    :param name: Human-readable name of the histogram
    :param result_file: TFile where the output is saved
    :param y_axis_label: Y axis label is case is needed (fraction vs efficiency)
    """
    histogram_name = h_nightly.GetName()

    type_name = histogram_name.split("_", 1)[0]
    variable_name = histogram_name.split("_", 1)[1]

    c1 = TCanvas()

    main_pad = TPad("main_pad", "top", 0.00, 0.25, 1.00, 1.00)
    main_pad.SetLeftMargin(0.12)
    main_pad.SetRightMargin(0.04)
    main_pad.SetTopMargin(0.02)
    main_pad.SetBottomMargin(0.02)
    main_pad.SetTicky(0)
    main_pad.SetTickx(0)
    main_pad.Draw()

    ratio_pad = TPad("ratio_pad", "bottom", 0.00, 0.00, 1.00, 0.25)
    ratio_pad.SetLeftMargin(0.12)
    ratio_pad.SetRightMargin(0.04)
    ratio_pad.SetTopMargin(0.03)
    ratio_pad.SetTickx(0)
    ratio_pad.SetBottomMargin(0.36)
    ratio_pad.Draw()

    h_base.SetLineColor(4)
    h_base.SetLineWidth(2)


    h_nightly.SetMarkerStyle(8)
    h_nightly.SetMarkerSize(0.5)

    main_pad.cd()




    if y_axis_label != None:
        h_base.GetYaxis().SetTitle(y_axis_label)
        h_base.GetYaxis().SetTitle(y_axis_label)

    if not '2D' in variable_name or 'Profile' in variable_name:
        h_base.Draw()

    h_nightly.Draw("same p" if not '2D'  in variable_name or 'Profile' in variable_name else 'colz')

    c1.Update()

    h_base.GetXaxis().SetLabelSize(0)
    h_base.GetXaxis().SetLabelOffset(999)

    h_base.SetMinimum(min(h_base.GetMinimum(), h_nightly.GetMinimum()) * 0.7)
    h_base.SetMaximum(max(h_base.GetMaximum(), h_nightly.GetMaximum()) * 1.3)


    leg = TLegend(0.4, 0.88, 0.9, 0.95)
    leg.SetHeader(name, "C")
    leg.SetNColumns(2)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.AddEntry(h_base, "Baseline", "l")
    leg.AddEntry(h_nightly, "Nightly", "p")
    leg.Draw()

    c1.Update()

    ratio_pad.cd()

    h1clone = h_nightly.Clone()
    h1clone.Sumw2()
    h1clone.SetStats(0)
    h1clone.Divide(h_base)
    h1clone.SetMarkerColor(1)
    h1clone.SetMarkerStyle(20)
    if "Efficiency" in histogram_name:
        h1clone.GetYaxis().SetRangeUser(h1clone.GetMinimum() * 0.7, h1clone.GetMaximum() * 1.3)
        gStyle.SetOptStat(0)
    h1clone.GetXaxis().SetLabelSize(0.10)
    h1clone.GetXaxis().SetTitleSize(0.17)
    h1clone.GetYaxis().SetLabelSize(0.10)
    h1clone.GetYaxis().SetRangeUser(0.75, 1.25)
    h1clone.GetYaxis().SetTitle("Ratio")
    h1clone.GetYaxis().CenterTitle(1)
    h1clone.GetYaxis().SetTitleSize(0.15)
    h1clone.GetYaxis().SetTitleOffset(0.3)
    h1clone.GetYaxis().SetNdivisions(505)

    h1clone.Draw("hist")

    c1.Update()



    result_file.cd()

    c1.SaveAs(type_name + '_' + variable_name + ".png")

    c1.Write(type_name + '_' + variable_name)



if __name__ == '__main__':

    gROOT.SetBatch(kTRUE)
    gStyle.SetOptStat(0)

    baseline_file = TFile(sys.argv[1])
    nightly_file = TFile(sys.argv[2])
    particle_type = sys.argv[3] # it can be 'electron' or 'gamma'

    output_file = TFile("BN_ComparisonPlots_" + particle_type + ".hist.root", "RECREATE")

    make_comparison_plots(particle_type, baseline_file, nightly_file, output_file)

    make_profile_plots(baseline_file, nightly_file, output_file)


    if particle_type == 'gamma':

        make_photon_fraction_plot(baseline_file, nightly_file,output_file)














