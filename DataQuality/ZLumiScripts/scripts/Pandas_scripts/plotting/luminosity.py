#!/usr/bin/env python3

import numpy as np
import pandas as pd
import python_tools as pt
import ROOT as R
import os
import math
from array import array
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('--infile', type=str, help='input file obvs')
parser.add_argument('--usemu', action='store_true', help='Plot vs. mu. Default == LB')

args = parser.parse_args()
infilename = args.infile

outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2/"

if "data15" in infilename: 
    year = "15"
elif "data16" in infilename: 
    year = "16"
elif "data17" in infilename: 
    year = "17"
elif "data18" in infilename: 
    year = "18"
        

def main():
    dfz = pd.read_csv(infilename, delimiter=',')
    run_number = str(int(dfz.RunNum[0]))
    lhc_fill   = str(int(dfz.FillNum[0]))
    
    # Drop LBs with no Z-counting information
    dfz = dfz.drop(dfz[(dfz.ZeeLumi == 0) | (dfz.ZmumuLumi == 0)].index)
     
    # Scale by livetime
    for entry in ['ZeeLumi','ZmumuLumi','ZeeLumiErr','ZmumuLumiErr','OffLumi']:  
        dfz[entry] *= dfz['LBLive']

    # Square uncertainties
    dfz['ZeeLumiErr'] *= dfz['ZeeLumiErr']
    dfz['ZmumuLumiErr'] *= dfz['ZmumuLumiErr']

    # Merge by groups of 20 LBs or pileup bins
    if args.usemu:
        dfz['OffMu'] = dfz['OffMu'].astype(int)
        dfz = dfz.groupby(['OffMu']).sum()
    else: 
        dfz['LBNum'] = (dfz['LBNum']//20)*20
        dfz = dfz.groupby(['LBNum']).sum()
    
    dfz['ZeeLumiErr']   = np.sqrt(dfz['ZeeLumiErr'])
    dfz['ZmumuLumiErr'] = np.sqrt(dfz['ZmumuLumiErr'])


    print("Making Z-counting vs. ATLAS plots!")
    for channel in ['Zee', 'Zmumu']: 
        if channel == "Zee": 
            channel_string = "Z #rightarrow ee counting"
            lep = "e"
        elif channel == "Zmumu": 
            channel_string = "Z #rightarrow #mu#mu counting"
            lep = "#mu"
            ymax = 0.6

        if args.usemu: 
            leg = R.TLegend(0.6, 0.28, 0.75, 0.50)
            xtitle = "<#mu>"
            outfile = outdir + "data"+year+"_"+run_number+"_"+channel+"_vs_atlas_mu"
        else: 
            leg = R.TLegend(0.6, 0.65, 0.75, 0.87)
            xtitle = "Luminosity Block Number"
            outfile = outdir + "data"+year+"_"+run_number+"_"+channel+"_vs_atlas"


        normalisation = dfz[channel+'Lumi'].sum() / dfz['OffLumi'].sum()
        arr_bins      = array('d', dfz.index)
        arr_zlumi     = array('d', dfz[channel+'Lumi'] / dfz['LBLive'] / normalisation)
        arr_zlumi_err = array('d', np.sqrt(dfz[channel+'LumiErr']) / dfz['LBLive'] / normalisation)
        arr_rat       = array('d', dfz[channel+'Lumi'] / dfz['OffLumi'] / normalisation)
        arr_rat_err   = array('d', np.sqrt(dfz[channel+'LumiErr']) / dfz['OffLumi'] / normalisation)
        arr_olumi     = array('d', dfz['OffLumi'] / dfz['LBLive'])

        hz = R.TGraphErrors(len(arr_bins), arr_bins, arr_zlumi, R.nullptr, arr_zlumi_err)
        ho = R.TGraphErrors(len(arr_bins), arr_bins, arr_olumi, R.nullptr, R.nullptr)
        hr = R.TGraphErrors(len(arr_bins), arr_bins, arr_rat, R.nullptr, arr_rat_err)

        ho.SetLineColor(R.kAzure)
        ho.SetLineWidth(3)
        
        c1 = R.TCanvas()
        pad1 = R.TPad("pad1", "pad1", 0, 0, 1, 1)
        pad1.SetBottomMargin(0.25)
        pad1.SetFillStyle(4000)
        pad1.Draw()
        pad1.cd()
        pad1.RedrawAxis()

        xmin = hz.GetXaxis().GetXmin()
        xmax = hz.GetXaxis().GetXmax()

        hz.SetMarkerStyle(4)
        hz.GetXaxis().SetLabelSize(0)
        hz.Draw('ap')
        ho.Draw("same L")
        hz.Draw('same p')
        hz.GetYaxis().SetTitle("Luminosity [10^{33} cm^{-2}s^{-1}]")
        hz.GetXaxis().SetTitleOffset(0.8)
        ymax = hz.GetHistogram().GetMaximum()
        ymin = hz.GetHistogram().GetMinimum()
        if not args.usemu: 
            hz.GetYaxis().SetRangeUser(ymin-2, ymax*1.6)
       
        leg.SetBorderSize(0)
        leg.SetTextSize(0.06)
        if channel == "Zee": 
            leg.AddEntry(hz, "L_{Z #rightarrow ee}^{normalised to L_{ATLAS}^{fill}}", "ep")
        elif channel == "Zmumu": 
            leg.AddEntry(hz, "L_{Z #rightarrow #mu#mu}^{normalised to L_{ATLAS}^{fill}}", "ep")
        leg.AddEntry(ho, "L_{ATLAS}", "l")
        leg.Draw()
        
        pad2 = R.TPad("pad2", "pad2", 0, 0, 1, 1)
        pad2.SetTopMargin(0.78)
        pad2.SetBottomMargin(0.09)
        pad2.SetFillStyle(4000)
        pad2.Draw()
        pad2.cd()

        hr.Draw("ap0")
        hr.GetXaxis().SetTitleSize(0.05)
        hr.GetXaxis().SetTitleOffset(0.865)
        hr.GetYaxis().SetTitle("Ratio")
        hr.GetXaxis().SetTitle(xtitle)

        hr.GetYaxis().SetTitleSize(0.05)
        hr.GetYaxis().SetRangeUser(0.9, 1.1)
        hr.GetXaxis().SetLabelSize(0.045)
        hr.GetYaxis().SetLabelSize(0.045)
        hr.GetYaxis().SetNdivisions(3)

        pt.drawAtlasLabel(0.2, 0.86, "Internal")
        pt.drawText(0.2, 0.80, "Data 20" + year + ", #sqrt{s} = 13 TeV")
        pt.drawText(0.2, 0.74, "LHC Fill " + lhc_fill)
        pt.drawText(0.2, 0.68,  channel_string)
        
        line0 = R.TLine(xmin, 1.0, xmax, 1.0)
        line0.SetLineStyle(2)
        line0.Draw()

        line1 = R.TLine(xmin, 0.95, xmax, 0.95)
        line1.SetLineStyle(2)
        line1.Draw()
        
        line2 = R.TLine(xmin, 1.05, xmax, 1.05)
        line2.SetLineStyle(2)
        line2.Draw()
    
        
        mean = np.median(arr_rat)
        stdev = np.percentile(abs(arr_rat - np.median(arr_rat)), 68)
        print("channel =", channel, "run =", run_number, "mean = ", mean, "stdev =", stdev)
        line4 = pt.make_bands(arr_bins, stdev, mean)
        line4.Draw("same 3")
        hr.Draw("same ep0")
    
        arrow = {}
        for xbin in range(1, hr.GetXaxis().GetNbins()+1):
            ratio = hr.GetPointY(xbin)
            mid = hr.GetPointX(xbin)
            if mid == -1: 
                continue

            if ratio > 1.10: 
                arrow[xbin] = R.TArrow(mid, 1.025, mid, 1.03, 0.02, "|>")
                arrow[xbin].SetLineWidth(2)
                arrow[xbin].SetLineColor(R.kRed)
                arrow[xbin].SetFillColor(R.kRed)
                arrow[xbin].Draw("same")
            elif ratio < 0.90: 
                arrow[xbin] = R.TArrow(mid, 0.975, mid, 0.97, 0.02, "|>")
                arrow[xbin].SetLineWidth(2)
                arrow[xbin].SetLineColor(R.kRed)
                arrow[xbin].SetFillColor(R.kRed)
                arrow[xbin].Draw("same")
          
        c1.SaveAs(outfile + ".pdf")

    if args.usemu:
        return
   
    # Zee / Zmumu comparison
    print("Doing channel comparison!")
    arr_bins      = array('d', dfz.index)
    arr_rat       = array('d', dfz['ZeeLumi'] / dfz['ZmumuLumi'])
    arr_rat_err   = array('d', (dfz['ZeeLumi'] / dfz['ZmumuLumi']) * np.sqrt(pow(dfz['ZeeLumiErr']/dfz['ZeeLumi'], 2) + pow(dfz['ZmumuLumiErr']/dfz['ZmumuLumi'], 2)))

    c1 = R.TCanvas()
    gr = R.TGraphErrors(len(arr_rat), arr_bins, arr_rat, R.nullptr, arr_rat_err)
    gr.Draw("ap")
    gr.GetXaxis().SetTitle("Luminosity Block")
    gr.GetYaxis().SetTitle("L_{Z #rightarrow ee} / L_{Z #rightarrow #mu#mu}")
    if year == "15": 
        gr.GetYaxis().SetRangeUser(0.6, 1.4)
    else: 
        gr.GetYaxis().SetRangeUser(0.8, 1.2)
    gr.Fit("pol0", "0")
    gr.GetFunction("pol0").SetLineColor(R.kRed)
   
    mean = gr.GetFunction("pol0").GetParameter(0)
    stdev = np.percentile(abs(arr_rat - np.median(arr_rat)), 68)
    print("####")
    print("stdev =", stdev)
    print("####")
    line1 = pt.make_bands(arr_bins, stdev, mean)
    line1.Draw("same 3")
    gr.GetFunction("pol0").Draw("same l")
    gr.Draw("same ep")

    latex = R.TLatex()
    R.ATLASLabel(0.2, 0.86, "Internal")
    latex.DrawLatexNDC(0.2, 0.80, "Data 20" +year+ ", #sqrt{s} = 13 TeV")
    latex.DrawLatexNDC(0.2, 0.74, "LHC Fill " + lhc_fill)

    chi2 = gr.GetFunction('pol0').GetChisquare()
    ndf  = gr.GetFunction('pol0').GetNDF()
    print "####"
    print "chi2 =", chi2/ndf
    print "####"

    leg = R.TLegend(0.17, 0.2, 0.90, 0.3)
    leg.SetBorderSize(0)
    leg.SetTextSize(0.05)
    leg.SetNColumns(3)
    leg.AddEntry(gr, "L_{Z #rightarrow ee}/L_{Z #rightarrow #mu#mu}", "ep")
    leg.AddEntry(gr.GetFunction("pol0"), "Mean = {:.3f}".format(round(mean, 3)), "l")
    leg.AddEntry(line1, "68% band", "f")
    leg.Draw()

    c1.SaveAs(outdir + "data"+year+"_"+run_number+"_zeezmmratio.pdf")
     



if __name__ == "__main__":
    pt.setAtlasStyle()
    R.gROOT.SetBatch(R.kTRUE)
    main()
