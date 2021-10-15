#!/usr/bin/env python

import numpy as np
import pandas as pd
import ROOT as R
import python_tools as pt
import math
import argparse
import glob

indir  = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu/"
outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2/"

parser = argparse.ArgumentParser()
parser.add_argument('--year', type=str, help='15-18, all for full Run-2')
parser.add_argument('--channel', type=str, help='Zee or Zmumu')
parser.add_argument('--comp', action='store_true', help='Compare Zee and Zmumu?')

args    = parser.parse_args()
year    = args.year
channel = args.channel
comp = args.comp

    
if year == "15":
    xmin = 0.2
    bins = np.concatenate((np.array([0, 10]), np.linspace(11, 17, 7), np.array([18, 22])))
elif year == "16":
    xmin = 0.4
    bins = np.concatenate((np.array([0, 10]), np.linspace(11, 39, 29), np.array([40, 45])))
elif year == "17":
    xmin = 0.4
    bins = np.concatenate((np.array([0, 15]), np.linspace(16, 59, 44), np.array([60, 70])))
elif year == "18":
    xmin = 0.2
    bins = np.concatenate((np.array([0, 15]), np.linspace(16, 55, 40), np.array([56, 70])))
else:
    xmin = 0.2
    bins = np.concatenate((np.array([0, 10]), np.linspace(11, 59, 49), np.array([60, 70])))

if year == "run2":
    date_string = "Run 2, #sqrt{s} = 13 TeV"
    grl = pt.get_grl("15")
    grl.extend(pt.get_grl("16"))
    grl.extend(pt.get_grl("17"))
    grl.extend(pt.get_grl("18"))
    outfile = "ZeeZmm_counting_data_ratio_v_mu.pdf"
else: 
    date_string = "Data 20"+year+", #sqrt{s} = 13 TeV"
    grl = pt.get_grl(year)
    outfile = "ZeeZmm_counting_data_ratio_v_mu_20"+year+".pdf"


def main():
    df = pd.DataFrame()
   
    for run in grl: 
        dfz = pd.read_csv(glob.glob(indir + "data1?_13TeV/" + "run_" + run + ".csv")[0])
        dfz_small = dfz
        if comp: 
            dfz_small = dfz_small.drop(dfz_small[(dfz_small.ZeeLumi == 0) | (dfz_small.ZmumuLumi == 0)].index)
            dfz_small['ZeeLumi']    *= dfz_small['LBLive']
            dfz_small['ZeeLumiErr'] *= dfz_small['LBLive']
            dfz_small['ZeeLumiErr'] *= dfz_small['ZeeLumiErr']
            dfz_small['ZmumuLumi']    *= dfz_small['LBLive']
            dfz_small['ZmumuLumiErr'] *= dfz_small['LBLive']
            dfz_small['ZmumuLumiErr'] *= dfz_small['ZmumuLumiErr']
        else:
            dfz_small['ZLumi'] = dfz_small[channel + 'Lumi']
            dfz_small['ZLumiErr'] = dfz_small[channel + 'LumiErr']
            dfz_small = dfz_small.drop(dfz_small[dfz_small.ZLumi == 0].index)
            dfz_small['OffLumi']  *= dfz_small['LBLive']
            dfz_small['ZLumi']    *= dfz_small['LBLive']
            dfz_small['ZLumiErr'] *= dfz_small['LBLive']
            dfz_small['ZLumiErr'] *= dfz_small['ZLumiErr']

        # Cut out all runs shorter than 40 minutes
        if dfz_small['LBLive'].sum() == 0: 
            continue
            
        # Cut out early 2016 runs with "strange" bunch structure
        if (year == "16" and dfz_small['LBStart'].iloc[0] < 1463184000) or run == "310247": 
            continue 
        
        df = df.append(dfz_small)
   

    df['OffMu'] = df['OffMu'].round(0)
    df = df.groupby(pd.cut(df.OffMu, bins, right=False)).sum()
    df.reset_index(drop=True, inplace=True)
    if comp:
        df['ZeeLumiErr'] = np.sqrt(df['ZeeLumiErr'])
        df['ZmumuLumiErr'] = np.sqrt(df['ZmumuLumiErr'])
        df['Ratio']    = df['ZeeLumi'] / df['ZmumuLumi']
        df['RatioErr'] = df['Ratio'] * np.sqrt(pow(df['ZeeLumiErr'] / df['ZeeLumi'], 2) + pow(df['ZmumuLumiErr'] / df['ZmumuLumi'], 2))
    else:
        df['ZLumiErr'] = np.sqrt(df['ZLumiErr'])
        df['Bin'] = pd.Series(bins)
        
        norm = df['ZLumi'].sum() / df['OffLumi'].sum()
        df['Ratio']    = df['ZLumi'] / df['OffLumi'] / norm
        df['RatioErr'] = df['ZLumiErr'] / df['OffLumi'] / norm

        print(df[['Bin','ZLumi','OffLumi','Ratio']])
        print("Norm =", norm)

    h_total = R.TH1F("h_total", "", len(bins)-1, bins)
    for xbin in range(0, h_total.GetNbinsX()):
        h_total.SetBinContent(xbin+1, df['Ratio'][xbin])
        h_total.SetBinError(xbin+1, df['RatioErr'][xbin])

    stdev = np.percentile(abs(df['Ratio'] - np.median(df['Ratio'])), 68)
    
    c1 = R.TCanvas()
    h_total.GetXaxis().SetTitle("<#mu>")
    h_total.Draw("E0")
    R.gStyle.SetErrorX()
    
    if comp: 
        h_total.Fit('pol0', '0')
        h_total.GetFunction('pol0').SetLineColor(R.kRed)
        h_total.GetFunction('pol0').Draw("same l")
        h_total.GetYaxis().SetRangeUser(0.97, 1.01)
        if year != "run2":
            leg = R.TLegend(0.4, 0.2, 0.65, 0.43)
        else: 
            leg = R.TLegend(0.2, 0.2, 0.45, 0.43)
        leg.AddEntry(h_total, "L_{Z #rightarrow ee}/L_{Z #rightarrow #mu#mu}", "ep")
        leg.AddEntry(h_total.GetFunction('pol0'), "Mean = " + str(round(h_total.GetFunction('pol0').GetParameter(0), 3)), "l")
        mean = h_total.GetFunction('pol0').GetParameter(0)
    else: 
        h_total.GetYaxis().SetRangeUser(0.97, 1.03)
        leg = R.TLegend(0.20, 0.18, 0.45, 0.35)
        mean = 1.0

    print("Year =", year, "channel =", channel, "Stdev =", round(stdev, 4), "mean =", mean)
    line1 = pt.make_bands(bins, stdev, mean)
    line1.Draw("same 3")
    if comp: 
        h_total.GetFunction('pol0').Draw("same l")
        h_total.Draw("same E0")
    else: 
        h_total.Draw("same E0")
    
    leg.SetBorderSize(0)
    leg.SetTextSize(0.05)
    if channel == "Zee": 
        h_total.GetYaxis().SetTitle("L_{Z #rightarrow ee} / L_{ATLAS}")
        leg.AddEntry(h_total, "L_{Z #rightarrow ee}^{year-normalised}/L_{ATLAS}", "ep")
        zstring = "Z #rightarrow ee counting"
    elif channel == "Zmumu": 
        h_total.GetYaxis().SetTitle("L_{Z #rightarrow #mu#mu} / L_{ATLAS}")
        leg.AddEntry(h_total, "L_{Z #rightarrow #mu#mu}^{year-normalised}/L_{ATLAS}", "ep")
        zstring = "Z #rightarrow #mu#mu counting"
    elif channel == "Zll": 
        h_total.GetYaxis().SetTitle("L_{Z #rightarrow ll} / L_{ATLAS}")
        leg.AddEntry(h_total, "L_{Z #rightarrow ll}^{year-normalised}/L_{ATLAS}", "ep")
        zstring = "Z #rightarrow ll counting"
    elif comp: 
        h_total.GetYaxis().SetTitle("L_{Z #rightarrow ee} / L_{Z #rightarrow #mu#mu}")
        zstring = ""

    pt.drawAtlasLabel(xmin, 0.88, "Internal")
    pt.drawText(xmin, 0.82, date_string)
    pt.drawText(xmin, 0.76, zstring)
        
    leg.AddEntry(line1, "68% band", "f")
    leg.Draw()

    if comp:
        c1.SaveAs("/eos/user/m/miokeefe/ZcountingPlots/FullRun2/" + outfile)
    else: 
        c1.SaveAs("/eos/user/m/miokeefe/ZcountingPlots/FullRun2/" + channel + "_counting_data_ratio_v_mu_20"+year+".pdf")

if __name__ == "__main__":
    pt.setAtlasStyle()
    R.gROOT.SetBatch(R.kTRUE)
    main()
