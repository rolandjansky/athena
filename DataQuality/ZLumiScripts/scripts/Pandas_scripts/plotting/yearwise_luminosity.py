#!/usr/bin/env python

"""
Plot comparisons of Zee/Zmumu and Z/ATLAS over entire data-periods. 
This can be done as a function of time (validated, working perfectly), 
and pileup (not yet fully validated).
"""

import numpy as np
import pandas as pd
import ROOT as R
import python_tools as pt
import math
from array import array
import time
import argparse

indir  = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu/"
outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2/"

#indir  = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputsToys/HighMu/"
#outdir = "/eos/user/m/miokeefe/ZcountingPlots/FullRun2Toys/"
    
parser = argparse.ArgumentParser()
parser.add_argument('--year', type=str, help='15-18, all for full Run-2')
parser.add_argument('--channel', type=str, help='Zee or Zmumu')
parser.add_argument('--comp', action='store_true', help='Compare Zee and Zmumu?')

args    = parser.parse_args()
year    = args.year
channel = args.channel

# Do all of the ugly plot stlying here
if year == "all": 
    years = ["15", "16", "17", "18"]
    out_tag = "_run2"
    time_format = "%m/%y"
    ymin, ymax = 0.95, 1.05
    xtitle = 'Month / Year'
    date_tag = "Run 2, #sqrt{s} = 13 TeV"
    norm_type = "Run2"
    if channel != None: 
        xval = 0.30
        yval = 0.33
    else:
        xval = 0.43
        yval = 0.33
    set_size = 1
else: 
    years = [year]
    out_tag = year
    time_format = "%d/%m"
    ymin, ymax = 0.90, 1.10
    xtitle = 'Date in 20' + year
    date_tag = "Data 20" + year  + ", #sqrt{s} = 13 TeV"
    norm_type = "year"
    xval = 0.235
    yval = 0.86
    set_size = 0


if channel == "Zee": 
    zstring   = "Z #rightarrow ee counting"
    ytitle    = 'L_{Z #rightarrow ee}/L_{ATLAS}'
    leg_entry = "L_{Z #rightarrow ee}^{"+norm_type+"-normalised}/L_{ATLAS}"
elif channel == "Zmumu":
    zstring   = "Z #rightarrow #mu#mu counting"
    ytitle    = 'L_{Z #rightarrow #mu#mu}/L_{ATLAS}'
    leg_entry = "L_{Z #rightarrow #mu#mu}^{"+norm_type+"-normalised}/L_{ATLAS}"
elif channel == "Zll":
    zstring   = "Z #rightarrow ll counting"
    ytitle    = 'L_{Z #rightarrow ll}/L_{ATLAS}'
    leg_entry = "L_{Z #rightarrow ll}^{"+norm_type+"-normalised}/L_{ATLAS}"


def main():
    if args.comp: 
        channel_comparison(years)
    else: 
        zcounting_vs_atlas(channel, years)


def channel_comparison(years):
    dict_zlumi = {}
    for year in years: 
        for channel in ["Zee", "Zmumu"]: 
            maindir  = indir + "data"+year+"_13TeV/"
            grl = pt.get_grl(year)
            for run in grl: 
                dfz = pd.read_csv(maindir + "run_" + run + ".csv")
                dfz_small = dfz
                dfz_small['ZLumi'] = dfz_small[channel + 'Lumi']
                dfz_small['ZLumiErr'] = dfz_small[channel + 'LumiErr']
                dfz_small = dfz_small.drop(dfz_small[dfz_small.ZLumi == 0].index)
                    
                # Cut out all runs shorter than 40 minutes
                if dfz_small['LBLive'].sum()/60 < 40: 
                    continue
                
                # Cut out early 2016 runs with "strange" bunch structure
                if (year == "16" and dfz_small['LBStart'].iloc[0] < 1463184000) or run == "310247": 
                    continue 

                    
                dfz_small['ZLumi'] *= dfz_small['LBLive']
                dfz_small['ZLumiErr'] *= dfz_small['LBLive']
                # If plotting vs. date simply fill the arrays here
                zlumi = dfz_small['ZLumi'].sum()
                
                dfz_small['ZLumiErr'] *= dfz_small['ZLumiErr']
                zerr = math.sqrt(dfz_small['ZLumiErr'].sum())
            
                # Grab start of the run for plotting later on
                run_start = dfz_small['LBStart'].iloc[0]
                timestamp = time.gmtime(run_start)
                timestamp = R.TDatime(timestamp[0], timestamp[1], timestamp[2], timestamp[3], timestamp[4], timestamp[5])
                timestamp = timestamp.Convert()
                dict_zlumi[channel, run] = (zlumi, zerr, timestamp)
            
    
    vec_times     = array('d')
    vec_times_err = array('d')
    vec_ratio     = array('d')
    vec_ratio_err = array('d')
    keys = [key[1] for key in dict_zlumi if "Zee" in key]

    # If plotting vs. date simply calculate integrated lumi per run and fill array
    for key in sorted(keys): 
        ratio = dict_zlumi["Zee", key][0]/dict_zlumi["Zmumu", key][0]
        error = ratio * math.sqrt( pow(dict_zlumi["Zee", key][1]/dict_zlumi["Zee", key][0], 2) + pow(dict_zlumi["Zmumu", key][1]/dict_zlumi["Zmumu", key][0], 2) )
        date  = dict_zlumi["Zee", key][2]

        vec_times.append(date)
        vec_ratio.append(ratio)
        vec_ratio_err.append(error)

    tg = R.TGraphErrors(len(vec_times), vec_times, vec_ratio, R.nullptr, vec_ratio_err)
    leg = R.TLegend(0.645, 0.72, 0.805, 0.91)

    # Depending if we're plotting over whole Run-2, change canvas size
    if out_tag == "_run2":
        c1 = R.TCanvas("c1", "c1", 2000, 1000)
    else:
        c1 = R.TCanvas()

    tg.Draw('ap')
    tg.GetYaxis().SetTitle('L_{Z #rightarrow ee} / L_{Z #rightarrow #mu#mu}')
    tg.Fit('pol0', '0q')
    tg.GetFunction('pol0').SetLineColor(R.kRed)
    
    mean =  round(tg.GetFunction('pol0').GetParameter(0), 4)
    err  =  round(tg.GetFunction('pol0').GetParError(0), 4)

    # Plot 68% percentile band
    stdev    = np.percentile(abs(vec_ratio - np.median(vec_ratio)), 68)
    line1 = pt.make_bands(vec_times, stdev, mean)
    line1.Draw("same 3")
    tg.GetFunction('pol0').Draw("same l")
    tg.Draw('same ep')

    print "#### STDEV =", round(stdev, 3)
   
    leg.SetBorderSize(0)
    leg.SetTextSize(0.045)
    leg.AddEntry(tg, "L_{Z #rightarrow ee}/L_{Z #rightarrow #mu#mu}", "ep")
    leg.AddEntry(tg.GetFunction("pol0"), "Mean = " + str(round(mean, 3)), "l")
    leg.AddEntry(line1, "68% band", "f")
    leg.Draw()

    pt.drawAtlasLabel(xval, 0.86, "Internal")
    pt.drawText(xval, 0.80, date_tag, set_size)
    tg.GetYaxis().SetRangeUser(ymin, ymax)
    tg.GetXaxis().SetTitle(xtitle)
    tg.GetXaxis().SetTimeDisplay(2)
    tg.GetXaxis().SetNdivisions(509)
    tg.GetXaxis().SetTimeFormat(time_format)
    tg.GetXaxis().SetTimeOffset(0,"gmt")
    c1.SaveAs(outdir + "channel_comp_data"+out_tag+".pdf")


def zcounting_vs_atlas(channel, years):
    """
    Plot normalised comparison of Z-counting luminosity to ATLAS luminosity.
    This can be done as a function of time and pileup.
    """
    arr_date  = array('d')
    arr_olumi = array('d')
    arr_zlumi = array('d')
    arr_zerr  = array('d')
    
    for year in years:
        maindir  = indir + "data"+year+"_13TeV/"
        grl = pt.get_grl(year)
        for run in grl: 
            dfz = pd.read_csv(maindir + "run_" + run + ".csv")
            dfz_small = dfz
            dfz_small['ZLumi'] = dfz_small[channel + 'Lumi']
            dfz_small['ZLumiErr'] = dfz_small[channel + 'LumiErr']
            dfz_small = dfz_small.drop(dfz_small[dfz_small.ZLumi == 0].index)

            # Cut out all runs shorter than 40 minutes
            if dfz_small['LBLive'].sum()/60 < 40: 
                continue
                
            # Cut out early 2016 runs with "strange" bunch structure
            if (year == "16" and dfz_small['LBStart'].iloc[0] < 1463184000) or run == "310247": 
                continue 
            
            # Grab start of the run for plotting later on
            run_start = dfz_small['LBStart'].iloc[0]
            timestamp = time.gmtime(run_start)
            timestamp = R.TDatime(timestamp[0], timestamp[1], timestamp[2], timestamp[3], timestamp[4], timestamp[5])
            timestamp = timestamp.Convert()
           
            # Calculate integrated ATLAS luminosity
            dfz_small['OffLumi'] *= dfz_small['LBLive']
            olumi = dfz_small['OffLumi'].sum()

            # Ad-hoc temporary vdM rescaling
            #if year == "15": 
            #    olumi *= 1.0133
            #elif year == "16": 
            #    olumi *= 1.0115
            #elif year == "17":
            #    olumi *= 1.00775
            #elif year == "18": 
            #    olumi *= 1.0045
            
            # Calculate integrated Z-counting luminosity
            dfz_small['ZLumi'] *= dfz_small['LBLive']
            zlumi = dfz_small['ZLumi'].sum()
            
            # Calculate uncertainty on Z-counting
            dfz_small['ZLumiErr'] *= dfz_small['LBLive']
            dfz_small['ZLumiErr'] *= dfz_small['ZLumiErr']
            zerr = math.sqrt(dfz_small['ZLumiErr'].sum())


            # If plotting vs. date simply fill the arrays here
            arr_date.append(timestamp)
            arr_olumi.append(olumi)
            arr_zlumi.append(zlumi)
            arr_zerr.append(zerr)

           
    # Calculate and apply normalisation
    normalisation = sum(arr_zlumi)/sum(arr_olumi)
    arr_zlumi = array('d', [x/normalisation for x in arr_zlumi])
    arr_zerr  = array('d', [x/normalisation for x in arr_zerr])
    arr_zlumi = np.divide(arr_zlumi, arr_olumi)
    arr_zerr  = np.divide(arr_zerr, arr_olumi)
    tg = R.TGraphErrors(len(arr_date), arr_date, arr_zlumi, R.nullptr, arr_zerr)

    # Depending if we're plotting over whole Run-2, change canvas size
    if out_tag == "_run2":
        c1 = R.TCanvas("c1", "c1", 2000, 1000)
    else:
        c1 = R.TCanvas()
    tg.Draw('ap')
    tg.GetXaxis().SetTitle(xtitle)

    # Fit over the different data periods to extract a mean per year
    # (but still with the normalisation done globally for full Run-2)
    #local_fit(tg, 1420109676, 1451645676, '2015')
    #local_fit(tg, 1451645676, 1483268076, '2016')
    #local_fit(tg, 1483268076, 1514804076, '2017')
    #local_fit(tg, 1514804076, 1546340076, '2018')
    #local_fit(tg, 1420109676, 1546340076, 'Run-2')
    tg.GetYaxis().SetRangeUser(ymin, ymax)
    
    # Plot 68% percentile band
    stdev = np.percentile(abs(arr_zlumi - np.median(arr_zlumi)), 68)
    line1 = pt.make_bands(arr_date, stdev, 1.0)
    line1.Draw("same 3")
    tg.Draw('same ep')
    
    print "#### STDEV =", stdev
    
    leg = R.TLegend(0.65, 0.18, 0.79, 0.43)
    leg.SetBorderSize(0)
    leg.SetTextSize(0.045)
    tg.GetYaxis().SetTitle(ytitle)
    leg.AddEntry(tg, leg_entry, "ep")
    leg.AddEntry(line1, "68% band", "f")
    leg.Draw()
    

    pt.drawAtlasLabel(xval, yval, "Internal")
    pt.drawText(xval, yval-0.06, date_tag, set_size)
    pt.drawText(xval, yval-0.12, zstring, set_size)
    tg.GetYaxis().SetRangeUser(ymin, ymax)
    tg.GetXaxis().SetTitle(xtitle)
    tg.GetXaxis().SetTimeDisplay(2)
    tg.GetXaxis().SetNdivisions(509)
    tg.GetXaxis().SetTimeFormat(time_format)
    tg.GetXaxis().SetTimeOffset(0,"gmt")
    c1.SaveAs(outdir + channel + "_counting_data"+out_tag+".pdf")

def local_fit(tg, start, end, year):
    """
    Fit over a sub-range of the data and print the mean and chi^2/NDF. 
    Useful to test the remaining trends after the global Run-2 normalisation.
    """

    tg.Fit('pol0', 'Rq0','0', start, end)
    mean = round(tg.GetFunction('pol0').GetParameter(0), 3)
    chi2 = tg.GetFunction('pol0').GetChisquare()
    ndf  = tg.GetFunction('pol0').GetNDF()
    print("|", year, "|", mean, "|", round(chi2/ndf, 3), "|")


if __name__ == "__main__":
    pt.setAtlasStyle()
    R.gROOT.SetBatch(R.kTRUE)
    main()
