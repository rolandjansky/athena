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
#outdir = "~/public/Zcounting/Plots/"

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
    for channel in ['Zee', 'Zmumu']: 
        dfz_small = dfz
        dfz_small['ZLumi'] = dfz_small[channel + 'Lumi']
        dfz_small['ZLumiErr'] = dfz_small[channel + 'LumiErr']
        dfz_small = dfz_small.drop(dfz_small[dfz_small.ZLumi == 0].index)

        livetime = {}
        zlumi    = {}
        zerr     = {}
        olumi    = {}
        for index, event in dfz_small.iterrows():
            run_number = str(int(event.RunNum))
            lhc_fill   = str(int(event.FillNum))
            if args.usemu:
                pileup = int(event.OffMu)
            else: 
                pileup = (event.LBNum//20)*20

            if pileup not in olumi:
                zlumi[pileup] = event.ZLumi*event.LBLive
                zerr[pileup]  = pow(event.ZLumiErr*event.LBLive, 2)
                olumi[pileup] = event.OffLumi*event.LBLive
                livetime[pileup] = event.LBLive
            else:
                zlumi[pileup] += event.ZLumi*event.LBLive
                zerr[pileup]  += pow(event.ZLumiErr*event.LBLive, 2)
                olumi[pileup] += event.OffLumi*event.LBLive
                livetime[pileup] += event.LBLive
        
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


        normalisation = sum(zlumi.values())/sum(olumi.values())
        arr_zlumi     = array('d')
        arr_zlumi_err = array('d')
        arr_olumi     = array('d')
        arr_rat       = array('d')
        arr_rat_err   = array('d')
        arr_bins      = array('d')
        for key in sorted(zlumi.keys()): 
            arr_bins.append(key)
            arr_olumi.append(olumi[key]/livetime[key])
            arr_zlumi.append(zlumi[key]/livetime[key]/normalisation)
            arr_zlumi_err.append(math.sqrt(zerr[key])/livetime[key]/normalisation)
            arr_rat.append(zlumi[key]/olumi[key]/normalisation)
            arr_rat_err.append(math.sqrt(zerr[key])/olumi[key]/normalisation)

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

        hr.Draw("ap")
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
        hr.Draw("same ep")
          
        c1.SaveAs(outfile + ".pdf")


if __name__ == "__main__":
    pt.setAtlasStyle()
    R.gROOT.SetBatch(R.kTRUE)
    main()
