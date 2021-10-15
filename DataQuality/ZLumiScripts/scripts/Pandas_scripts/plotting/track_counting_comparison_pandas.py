#!/usr/bin/env python

import numpy as np
import pandas as pd
import ROOT as R
import python_tools as pt
import DQUtils
import math
import time
from array import array

def main():
    # Define required constants for TC conversion
    sigmaInel = 80.0  #unit mb
    revolutionFreq =  11245.5 # Hz
    unitConversion = 0.000001
    calib_factor_tight = 3.696
    calib_factor_tight_mod = 1.6852

    output_dict = {}
    for year in ["16", "17", "18"]:
        if year == "16": 
            tc_main_dir = "/eos/atlas/atlascerngroupdisk/data-prep/lumi_track_counting/TCLumiBenedettoInputs/TCLUMI2016_FALL20/v2/PerLB/"
            file_tag = "bFile_r%s_PixelBeam_FALL20_physicsGRL.txt"
        elif year == "17": 
            tc_main_dir = "/eos/atlas/atlascerngroupdisk/data-prep/lumi_track_counting/TCLumiBenedettoInputs/ORIGINAL2017/v3/PerLB/"
            file_tag = "bFile_r%s_ORIGINAL2017_physicsGRL.txt"
        elif year == "18": 
            tc_main_dir = "/eos/atlas/atlascerngroupdisk/data-prep/lumi_track_counting/TCLumiBenedettoInputs/TCLUMI2018_FALL18/v3/PerLB/"
            file_tag = "bFile_r%s_PixelBeam_FALL18_physicsGRL.txt"
        
        for run in pt.get_grl(year):
            run_tag = file_tag % (run)
            zc_file = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu/data"+year+"_13TeV/run_"+run+".csv"
            tc_file = tc_main_dir + run_tag

            dfzc = pd.read_csv(zc_file)
            dfzc = dfzc.drop(dfzc[dfzc.ZllLumi == 0].index)
            try: 
                fields = ['LBNum', 'TightLumi_muVis', 'TightLumi_muVis_error', 'TightModLumi_muVis', 'TightModLumi_muVis_error']
                dftc = pd.read_csv(tc_file, sep=" ", skiprows=1, names=fields, index_col=False)
            except: 
                print(tc_file, "does not exist!!!")
                continue
          
            # Cut out runs with live-lengths of sub 40 mins
            if dfzc['LBLive'].sum()/60 < 40: 
                continue

            # Cut out early 2016 with weird bunch structures
            if (year == "16" and dfzc['LBStart'].iloc[0] < 1463184000) or run == "310247": 
                continue 

            # Get number of bunches
            iovs  = DQUtils.fetch_iovs("COOLOFL_DCS::/LHC/DCS/FILLSTATE",since=(int(run), dfzc['LBNum'].iloc[0]),until=(int(run), dfzc['LBNum'].iloc[0]), convert_time=True)
            nbunches = float(iovs[0].NumBunchColl)

            # Merge the ZC and TC dataframes, so that we only use LBs that are defined in both analyses
            df_total = pd.merge(dfzc, dftc, on=["LBNum"])
            
            # Cut out runs with live-lengths of sub 40 mins AFTER merging, i.e. only LBs present in both analyses
            if df_total['LBLive'].sum()/60 < 40: 
                continue

            # Convert TC sigma_vis to lumi
            df_total['TightModLumi'] = df_total['TightModLumi_muVis'] * df_total['LBLive'] * nbunches  * revolutionFreq * unitConversion / sigmaInel / calib_factor_tight_mod
            df_total['TightModLumiErr'] = df_total['TightModLumi_muVis_error'] * df_total['LBLive'] * nbunches  * revolutionFreq * unitConversion / sigmaInel / calib_factor_tight_mod
            df_total['TightModLumiErr'] *= df_total['TightModLumiErr']

            # Calculate integrated ZC luminosity
            df_total['ZllLumi'] *= df_total['LBLive']
            df_total['ZllLumiErr'] *= df_total['LBLive']
            df_total['ZllLumiErr'] *= df_total['ZllLumiErr']

            # Calculate run-integrated luminosities
            zclumi = df_total['ZllLumi'].sum()
            tclumi = df_total['TightModLumi'].sum()

            zclumi_error = math.sqrt(df_total['ZllLumiErr'].sum())
            tclumi_error = math.sqrt(df_total['TightModLumiErr'].sum())

            # Grab the start time of the first LB to plot later on
            run_start = df_total['LBStart'].iloc[0]
            timestamp = time.gmtime(run_start)
            timestamp = R.TDatime(timestamp[0], timestamp[1], timestamp[2], timestamp[3], timestamp[4], timestamp[5])
            timestamp = timestamp.Convert()

            # Save to output dictionary
            output_dict[run] = [zclumi, zclumi_error, tclumi, tclumi_error, timestamp]

    print("Done looping, now calculate normalised ratios...")
    normalisation = sum([entry[0] for entry in output_dict.values()]) / sum([entry[2] for entry in output_dict.values()])

    vec_times     = array('d')
    vec_ratio     = array('d')
    vec_ratio_err = array('d')
    for run in output_dict:
        ratio = output_dict[run][0]/output_dict[run][2]/normalisation
        error = ratio * math.sqrt(pow(output_dict[run][1]/output_dict[run][0], 2) + pow(output_dict[run][3]/output_dict[run][2], 2))

        vec_times.append(output_dict[run][4])
        vec_ratio.append(ratio)
        vec_ratio_err.append(error)

    tg = R.TGraphErrors(len(vec_times), vec_times, vec_ratio, R.nullptr, vec_ratio_err)
    tg.GetYaxis().SetRangeUser(0.95, 1.05)
    tg.GetXaxis().SetTitle("Month / Year")
    tg.GetYaxis().SetTitle("L_{Z #rightarrow ll} / L_{track-counting}^{TightModLumi}")
    tg.GetXaxis().SetTimeDisplay(2)
    tg.GetXaxis().SetNdivisions(509)
    tg.GetXaxis().SetTimeFormat("%m/%y")
    tg.GetXaxis().SetTimeOffset(0,"gmt")
    
    stdev = np.percentile(abs(vec_ratio - np.median(vec_ratio)), 68)
    line1 = pt.make_bands(vec_times, stdev, 1.0)
    print("grepme:", stdev)
    local_fit(tg, 1451645676, 1483268076, '2016')
    local_fit(tg, 1483268076, 1514804076, '2017')
    local_fit(tg, 1514804076, 1546340076, '2018')
    local_fit(tg, 1420109676, 1546340076, 'Run 2')
    
    leg = R.TLegend(0.65, 0.18, 0.79, 0.43)
    leg.SetBorderSize(0)
    leg.SetTextSize(0.045)
    leg.AddEntry(tg, "L_{Z #rightarrow ll}^{Run2-normalised}/L_{track-counting}^{TightModLumi}", "ep")
    leg.AddEntry(line1, "68% band", "f")

    c1 = R.TCanvas("c1", "c1", 2000, 1000)
    tg.Draw("ap")
    line1.Draw("same 3")
    tg.Draw('same ep')
    pt.drawAtlasLabel(0.3, 0.33, "Internal")
    pt.drawText(0.3, 0.27, "Run 2, #sqrt{s} = 13 TeV", 1)
    pt.drawText(0.3, 0.21, "Z #rightarrow ll counting", 1)
    leg.Draw()
    c1.SaveAs("/eos/user/m/miokeefe/ZcountingPlots/FullRun2/TrackCounting/Zll_vs_TightModLumi.pdf")


def local_fit(tg, start, end, year):
    """
    Fit over a sub-range of the data and print the mean and chi^2/NDF. 
    Useful to test the remaining trends after the global Run 2 normalisation.
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
