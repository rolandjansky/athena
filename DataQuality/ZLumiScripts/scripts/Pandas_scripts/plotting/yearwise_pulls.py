#!/usr/bin/env python
"""
Calculate the pull for every LB in each data taking period.
Pull = ( L(Z, normalised) - L(ATLAS) ) / error(Z). 
This is done for all three channels, Zee, Zmumu, Zll. 
"""

import numpy as np
import pandas as pd
import ROOT as R
import plotting.python_tools as pt
import time
import math
from array import array

indir  = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu/"
outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2/"

#indir  = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputsToys/HighMu/"
#outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2Toys/"


def make_pull_plot(channel, year, color, df_total=None):
    maindir  = indir + "data"+year+"_13TeV/"
    grl = pt.get_grl(year)
    df = pd.DataFrame()
    for run in grl: 
        dfz             = pd.read_csv(maindir + "run_" + run + ".csv")
        dfz['ZLumi']    = dfz[channel + 'Lumi']
        dfz['ZLumiErr'] = dfz[channel + 'LumiErr']
        # Drop LBs that don't have info in all channels
        dfz = dfz.drop(dfz[(dfz.PassGRL == 0) | (dfz.ZeeLumi == 0) | (dfz.ZmumuLumi == 0)].index)
        
        # Make small dataframe to append later
        dfz_small = dfz[['RunNum','ZLumi','ZLumiErr','OffLumi','ZeeLumi','ZeeLumiErr','ZmumuLumi','ZmumuLumiErr','ZllLumi','ZllLumiErr','LBStart']]
        df = df.append(dfz_small)

    # Ad-hoc temporary vdM rescaling
    if year == "15": 
        df['OffLumi'] *= 1.0133
    elif year == "16": 
        df['OffLumi'] *= 1.0115
    elif year == "17":
        df['OffLumi'] *= 1.00775
    elif year == "18": 
        df['OffLumi'] *= 1.0045


    # Normalise the Z-counting luminosity
    #normalisation    = df['ZLumi'].sum()/df['OffLumi'].sum()
    normalisation    = 1.0
    df['ZLumi']     /= normalisation
    df['ZLumiErr']  /= normalisation
    # Calculate pull
    df['Sigma'] = (df['ZLumi'] - df['OffLumi'])/df['ZLumiErr']

    histo = R.TH1F("h_" + channel + "_" + year, "", 50, -5, 5)
    histo.SetLineColor(color)
    histo.SetMarkerColor(color)
    for index, entry in df.iterrows():
        histo.Fill(entry.Sigma)
   
    # Just append the dataframe once, see call for zee below
    # ToDo: only call this function once and do all three channels at once, 
    # returning 3 histos. That way there is no need to read the file 3 times...
    if df_total is not None: 
        df_total = df_total.append(df)
        return histo, df_total
    else:
        return histo


def main():
    df_total = pd.DataFrame()
    for year in ["15", "16", "17", "18"]:
        # Fill 1 histo per channel per year
        zee, df_total = make_pull_plot("Zee",   year, R.kRed, df_total)
        zmm = make_pull_plot("Zmumu", year, R.kBlue)
        zll = make_pull_plot("Zll",   year, R.kBlack)
        
        zee.GetYaxis().SetTitle("Entries")
        zee.GetXaxis().SetTitle("(Normalised L_{Z} - L_{ATLAS}) / #Delta L_{Z}")
        zee.GetYaxis().SetRangeUser(0, zll.GetMaximum()*1.4)
        zee.SetMarkerStyle(4)
        zmm.SetMarkerStyle(4)
        
        c1 = R.TCanvas()
        zee.Draw('p')
        zmm.Draw("same p")
        zll.Draw('same p')

        # Fit each histogram with a Gaussian
        zee.Fit('gaus')
        zee.GetFunction('gaus').SetLineColor(R.kRed)
        zee.GetFunction('gaus').SetLineStyle(2)
        
        zmm.Fit('gaus')
        zmm.GetFunction('gaus').SetLineColor(R.kBlue)
        zmm.GetFunction('gaus').SetLineStyle(2)

        zll.Fit('gaus')
        zll.GetFunction('gaus').SetLineColor(R.kBlack)
        
        # Print the width of the Gaussian to the terminal
        print("grepme: Year", year, "Zee", round(zee.GetFunction("gaus").GetParameter(2),2), "Zmm", round(zmm.GetFunction("gaus").GetParameter(2),2), "Zll", round(zll.GetFunction("gaus").GetParameter(2),2))

        # Draw Gaussian width on plot
        pt.drawText(0.44, 0.45, "#sigma_{Z#rightarrow ee} = " + str(round(zee.GetFunction("gaus").GetParameter(2), 2)))
        pt.drawText(0.44, 0.35, "#sigma_{Z#rightarrow #mu#mu} = " + str(round(zmm.GetFunction("gaus").GetParameter(2), 2)))
        pt.drawText(0.44, 0.25, "#sigma_{Z#rightarrow ll}   = " + str(round(zll.GetFunction("gaus").GetParameter(2), 2)))

        
        leg = R.TLegend(0.67, 0.6, 0.88, 0.8)
        leg.SetBorderSize(0)
        leg.SetTextSize(0.05)
        leg.AddEntry(zee, "Z #rightarrow ee", "p")
        leg.AddEntry(zmm, "Z #rightarrow #mu#mu", "p")
        leg.AddEntry(zll, "Z #rightarrow ll", "p")
        leg.Draw()

        pt.drawAtlasLabel(0.2, 0.86, "Internal")
        pt.drawText(0.2, 0.80, "Data 20" + year + ", #sqrt{s} = 13 TeV")
        #pt.drawText(0.2, 0.73, "N_{Z #rightarrow ee} > 100")
        #pt.drawText(0.2, 0.66, "and")
        #pt.drawText(0.2, 0.59, "N_{Z #rightarrow #mu#mu} > 100")
        zee.GetYaxis().SetRangeUser(0.1, zll.GetMaximum()*1000)
        R.gPad.SetLogy()
        c1.SaveAs(outdir + "yearwise_pulls_data"+year+".pdf")

    # Make Zee - Zmumu pull plot
    df_total['ZeeZmumuPull'] = (df_total['ZeeLumi'] - df_total['ZmumuLumi']) / np.sqrt(np.power(df_total['ZeeLumiErr'], 2) + np.power(df_total['ZmumuLumiErr'], 2))
    df_total['ZeePull'] = (df_total['ZeeLumi'] / (df_total['ZeeLumi'].sum()/df_total['OffLumi'].sum()) - df_total['OffLumi']) / df_total['ZeeLumiErr']
    df_total['ZmumuPull'] = (df_total['ZmumuLumi'] / (df_total['ZmumuLumi'].sum()/df_total['OffLumi'].sum()) - df_total['OffLumi']) / df_total['ZmumuLumiErr']
    df_total['ZllPull'] = (df_total['ZllLumi'] / (df_total['ZllLumi'].sum()/df_total['OffLumi'].sum()) - df_total['OffLumi']) / df_total['ZllLumiErr']
    h_zee_pull = R.TH1F("h_zee_pull", "", 50, -5, 5)
    h_zmm_pull = R.TH1F("h_zmm_pull", "", 50, -5, 5)
    h_zll_pull = R.TH1F("h_zll_pull", "", 50, -5, 5)
    h_zee_zmm_pull = R.TH1F("h_zee_zmm_pull", "", 50, -5, 5)
    for index, entry in df_total.iterrows():
        h_zee_pull.Fill(entry.ZeePull)
        h_zmm_pull.Fill(entry.ZmumuPull)
        h_zll_pull.Fill(entry.ZllPull)
        h_zee_zmm_pull.Fill(entry.ZeeZmumuPull)
        
    h_zee_pull.SetMarkerStyle(4)
    h_zee_pull.SetMarkerColor(R.kRed)
    h_zmm_pull.SetMarkerStyle(4)
    h_zmm_pull.SetMarkerColor(R.kBlue)
    h_zee_pull.GetYaxis().SetRangeUser(0.1, h_zee_zmm_pull.GetMaximum()*100)
    h_zee_zmm_pull.GetYaxis().SetRangeUser(0.1, h_zee_zmm_pull.GetMaximum()*100)
   
    c1 = R.TCanvas()
    h_zee_zmm_pull.Draw(" p")
    h_zee_zmm_pull.Fit("gaus")
    h_zee_zmm_pull.GetFunction('gaus').SetLineColor(R.kBlack)
    h_zee_zmm_pull.GetFunction('gaus').SetLineStyle(2)
    h_zee_zmm_pull.GetYaxis().SetTitle("Entries")
    h_zee_zmm_pull.GetXaxis().SetTitle("(L_{ Z #rightarrow ee} - L_{ Z #rightarrow #mu#mu}) / #sqrt{ #Delta L_{ Z #rightarrow ee}^{2} + #Delta L_{ Z #rightarrow #mu#mu}^{2} }")

    pt.drawText(0.44, 0.45, "#sigma = " + str(round(h_zee_zmm_pull.GetFunction("gaus").GetParameter(2), 2)))
    pt.drawAtlasLabel(0.2, 0.86, "Internal")
    pt.drawText(0.2, 0.80, "Run-2, #sqrt{s} = 13 TeV")
    R.gPad.SetLogy()
    c1.SaveAs(outdir + "Zee_Zmumu_pull_data_run2.pdf")
    
    # Make full Run-2 single channel plots
    c1 = R.TCanvas()
    h_zee_pull.Draw('p')
    h_zmm_pull.Draw("same p")
    h_zll_pull.Draw('same p')

    h_zee_pull.Fit('gaus')
    h_zee_pull.GetFunction('gaus').SetLineColor(R.kRed)
    h_zee_pull.GetFunction('gaus').SetLineStyle(2)
    
    h_zmm_pull.Fit('gaus')
    h_zmm_pull.GetFunction('gaus').SetLineColor(R.kBlue)
    h_zmm_pull.GetFunction('gaus').SetLineStyle(2)

    h_zll_pull.Fit('gaus')
    h_zll_pull.GetFunction('gaus').SetLineColor(R.kBlack)

    h_zee_pull.GetYaxis().SetTitle("Entries")
    h_zee_pull.GetXaxis().SetTitle("(Normalised L_{Z} - L_{ATLAS}) / #Delta L_{Z}")

    pt.drawText(0.44, 0.45, "#sigma_{Z#rightarrow ee} = " + str(round(h_zee_pull.GetFunction("gaus").GetParameter(2), 2)))
    pt.drawText(0.44, 0.35, "#sigma_{Z#rightarrow #mu#mu} = " + str(round(h_zmm_pull.GetFunction("gaus").GetParameter(2), 2)))
    pt.drawText(0.44, 0.25, "#sigma_{Z#rightarrow ll} = " + str(round(h_zll_pull.GetFunction("gaus").GetParameter(2), 2)))
    pt.drawAtlasLabel(0.2, 0.86, "Internal")
    pt.drawText(0.2, 0.80, "Run-2, #sqrt{s} = 13 TeV")
    R.gPad.SetLogy()
    c1.SaveAs(outdir + "FullRun2_pull_data_run2.pdf")
    return


    # Now make the full Run-2 plot using the large dataframe which we appended to
    # Grab first LB timestamp before we do groupby and sum
    timestamps = df_total.groupby('RunNum', sort=False)['LBStart'].min()
    for channel in ['Zee','Zmumu','Zll']:
        df_total[channel+'LumiErr'] *= df_total[channel+'LumiErr']

    # Integrated lumi per run, i.e. sum everything
    df_total = df_total.groupby(['RunNum']).sum()

    for channel in ['Zee','Zmumu','Zll']:
        # Calculate normalisation per channel
        norm = (df_total[channel+'Lumi'].sum() / df_total['OffLumi'].sum())
        df_total[channel+'Lumi'] /= norm
        
        # Calculate error
        df_total[channel+'LumiErr'] = np.power(df_total[channel+'LumiErr'], 0.5)
        df_total[channel+'LumiErr'] /= norm

        # Calculate pull
        df_total[channel+'Sigma'] = (df_total[channel+'Lumi'] - df_total['OffLumi'])/df_total[channel+'LumiErr']

    # Make TGraph per channel
    vec_times   = array('d')
    vec_zee     = array('d')
    vec_zmm     = array('d')
    vec_zll     = array('d')
    for index, entry in df_total.iterrows():
        vec_times.append(timestamps[index])
        vec_zee.append(entry.ZeeSigma)
        vec_zmm.append(entry.ZmumuSigma)
        vec_zll.append(entry.ZllSigma)
    
    tg_zee = R.TGraphErrors(len(vec_times), vec_times, vec_zee, R.nullptr, R.nullptr)
    tg_zmm = R.TGraphErrors(len(vec_times), vec_times, vec_zmm, R.nullptr, R.nullptr)
    tg_zll = R.TGraphErrors(len(vec_times), vec_times, vec_zll, R.nullptr, R.nullptr)
        
    tg_zee.SetMarkerStyle(4)
    tg_zmm.SetMarkerStyle(4)
    tg_zee.SetMarkerColor(R.kRed)
    tg_zmm.SetMarkerColor(R.kBlue)
        
    tg_zee.GetYaxis().SetRangeUser(-7, 7)
    tg_zee.GetXaxis().SetTitle("Month / Year")
    tg_zee.GetYaxis().SetTitle("Pull")
    tg_zee.GetXaxis().SetTimeDisplay(2)
    tg_zee.GetXaxis().SetNdivisions(509)
    tg_zee.GetXaxis().SetTimeFormat("%m/%y")
    tg_zee.GetXaxis().SetTimeOffset(0,"gmt")
            
    c1 = R.TCanvas("c1", "c1", 2000, 1000)
    tg_zee.Draw("ap")
    tg_zmm.Draw("same p")
    tg_zll.Draw("same p")
    #pt.drawAtlasLabel(0.3, 0.86, "Internal")
    #pt.drawText(0.3, 0.80, "Run-2, #sqrt{s} = 13 TeV", 1)
    c1.SaveAs(outdir + "FullRun2Pulls.pdf")


if __name__ == "__main__":
    pt.setAtlasStyle()
    R.gROOT.SetBatch(R.kTRUE)
    main()
