#!/usr/bin/env python

import numpy as np
import pandas as pd
import ROOT as R
import argparse 
from array import array
from math import sqrt
import python_tools as pt

parser = argparse.ArgumentParser()
parser.add_argument('--infile', type=str, help='input file obvs')
parser.add_argument('--usemu', action='store_true', help='Plot vs. mu. Default == LB')

args = parser.parse_args()
infilename = args.infile

#outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2/"
outdir = "~/public/Zcounting/Plots/"

if "data15" in infilename: 
    year = "15"
elif "data16" in infilename: 
    year = "16"
elif "data17" in infilename: 
    year = "17"
elif "data18" in infilename: 
    year = "18"


def main():
    dfz = pd.read_csv(infilename, delimiter=",")
    for channel in ['Zee', 'Zmumu']: 
        dfz_small = dfz
        dfz_small['ZLumi']    = dfz_small[channel + 'Lumi']
        dfz_small['ZLumiErr'] = dfz_small[channel + 'LumiErr']
        dfz_small['EffTrig']  = dfz_small[channel + 'EffTrig']
        dfz_small['ErrTrig']  = dfz_small[channel + 'ErrTrig']
        dfz_small['EffReco']  = dfz_small[channel + 'EffReco']
        dfz_small['ErrReco']  = dfz_small[channel + 'ErrReco']
        
        dfz_small = dfz_small.drop(dfz_small[dfz_small.ZLumi == 0].index)
        if channel == "Zee": 
            channel_string = "Z #rightarrow ee"
            lep = "e"
            if year == "15": 
                ymax = 0.55
                leg = R.TLegend(0.7, 0.37, 0.8, 0.6)
            else: 
                ymax = 0.88
                leg = R.TLegend(0.7, 0.7, 0.8, 0.93)
        elif channel == "Zmumu": 
            channel_string = "Z #rightarrow #mu#mu"
            lep = "#mu"
            leg = R.TLegend(0.7, 0.37, 0.8, 0.6)
            ymax = 0.6

        dict_mu       = {}
        dict_trig     = {}
        dict_trig_err = {}
        dict_reco     = {}
        dict_reco_err = {}

        for index, event in dfz_small.iterrows():
            run_number = str(int(event.RunNum))
            lhc_fill   = str(int(event.FillNum))
            if args.usemu: 
                pileup = int(event.OffMu)
            else: 
                pileup = (event.LBNum // 20)*20

            if event.ErrTrig == 0.0 or event.ErrReco == 0.0: 
                continue

            weight_trig = 1/pow(event.ErrTrig, 2)
            weight_reco = 1/pow(event.ErrReco, 2)
            if pileup not in dict_mu: 
                dict_mu[pileup] = pileup
                dict_trig[pileup] = weight_trig * event.EffTrig
                dict_reco[pileup] = weight_reco * event.EffReco
                dict_trig_err[pileup] = weight_trig 
                dict_reco_err[pileup] = weight_reco 
            else:
                dict_trig[pileup] += weight_trig * event.EffTrig
                dict_reco[pileup] += weight_reco * event.EffReco
                dict_trig_err[pileup] += weight_trig 
                dict_reco_err[pileup] += weight_reco 

        vec_trig     = array('d')
        vec_trig_err = array('d')
        vec_reco     = array('d')
        vec_reco_err = array('d')
        vec_mu       = array('d')

        for pileup in dict_mu: 
            trig_weighted_average = dict_trig[pileup]/dict_trig_err[pileup]
            trig_error = sqrt(1/dict_trig_err[pileup])
            vec_trig.append(trig_weighted_average)
            vec_trig_err.append(trig_error)
            
            reco_weighted_average = dict_reco[pileup]/dict_reco_err[pileup]
            reco_error = sqrt(1/dict_reco_err[pileup])
            vec_reco.append(reco_weighted_average)
            vec_reco_err.append(reco_error)

            vec_mu.append(pileup)

        trig_graph = R.TGraphErrors(len(vec_trig), vec_mu, vec_trig, R.nullptr, vec_trig_err);
        trig_graph.GetHistogram().SetYTitle("Efficiency")
        trig_graph.GetHistogram().GetYaxis().SetRangeUser(0.7, 1.0)
        trig_graph.SetMarkerSize(1)
        
        reco_graph = R.TGraphErrors(len(vec_reco), vec_mu, vec_reco, R.nullptr, vec_reco_err);
        reco_graph.GetHistogram().GetYaxis().SetRangeUser(0.7, 1.0)
        reco_graph.SetMarkerSize(1)
        reco_graph.SetMarkerStyle(21)
        reco_graph.SetMarkerColor(R.kRed)
        reco_graph.SetLineColor(R.kRed)


        leg.SetBorderSize(0)
        leg.SetTextSize(0.07)
        leg.AddEntry(reco_graph, "#varepsilon_{reco}^{single-"+lep+"}", "ep")
        leg.AddEntry(trig_graph, "#varepsilon_{trig}^{single-"+lep+"}", "ep")

        c1 = R.TCanvas()
        trig_graph.Draw("ap")
        reco_graph.Draw("p")
        
        pt.drawAtlasLabel(0.2, ymax, "Internal")
        pt.drawText(0.2, ymax-0.06, "Data 20" + year + ", #sqrt{s} = 13 TeV")
        pt.drawText(0.2, ymax-0.12, "LHC Fill " + lhc_fill)
        pt.drawText(0.2, ymax-0.18, channel_string + " counting")
        leg.Draw()

        if args.usemu: 
            trig_graph.GetHistogram().SetXTitle("<#mu>")
            c1.SaveAs(outdir + "eff_v_mu_"+channel+"_data"+year+"_"+run_number+".pdf")
        else: 
            trig_graph.GetHistogram().SetXTitle("Luminosity Block Number")
            c1.SaveAs(outdir + "eff_v_lb_"+channel+"_data"+year+"_"+run_number+".pdf")


if __name__ == "__main__":
    R.gROOT.SetBatch(R.kTRUE)
    pt.setAtlasStyle()
    main()
