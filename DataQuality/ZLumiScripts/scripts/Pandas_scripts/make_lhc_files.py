#!/usr/bin/env python

import numpy as np
import pandas as pd
import ROOT as R
import argparse 
from array import array
from math import sqrt
import python_tools as pt
import time
import csv

parser = argparse.ArgumentParser()
parser.add_argument('--infile', type=str, help='input file obvs')

args       = parser.parse_args()
infilename = args.infile

def main():
    dfz = pd.read_csv(infilename, delimiter=",")
    dict_eff = {}
    for index, event in dfz.iterrows():
        lbzero = (event.LBNum // 20)*20
        
        if event.ZeeErrComb != 0.0:
            weight_zee = 1/pow(event.ZeeErrComb, 2)
        else:
            weight_zee = 0.0
        
        if event.ZmumuErrComb != 0.0:
            weight_zmumu = 1/pow(event.ZmumuErrComb, 2)
        else:
            weight_zmumu = 0.0

        if lbzero not in sorted([key[0] for key in dict_eff.keys() if "ZeeEff" in key]): 
            dict_eff[lbzero, "ZeeEff"] = weight_zee * event.ZeeEffComb
            dict_eff[lbzero, "ZeeErr"] = weight_zee 
            dict_eff[lbzero, "ZeeRaw"] = event.ZeeRaw
            dict_eff[lbzero, "ZmumuEff"] = weight_zmumu * event.ZmumuEffComb
            dict_eff[lbzero, "ZmumuErr"] = weight_zmumu 
            dict_eff[lbzero, "ZmumuRaw"] = event.ZmumuRaw
            dict_eff[lbzero, "lbstart"] = event.LBStart
            dict_eff[lbzero, "lbend"] = event.LBEnd
            dict_eff[lbzero, "lblive"] = event.LBLive
            dict_eff[lbzero, "offlumi"] = event.OffLumi
            dict_eff[lbzero, "lhcfill"] = int(event.FillNum)
        else:
            dict_eff[lbzero, "ZeeEff"] += weight_zee * event.ZeeEffComb
            dict_eff[lbzero, "ZeeErr"] += weight_zee
            dict_eff[lbzero, "ZeeRaw"] += event.ZeeRaw
            dict_eff[lbzero, "ZmumuEff"] += weight_zmumu * event.ZmumuEffComb
            dict_eff[lbzero, "ZmumuErr"] += weight_zmumu
            dict_eff[lbzero, "ZmumuRaw"] += event.ZmumuRaw
            dict_eff[lbzero, "lblive"] += event.LBLive
            dict_eff[lbzero, "lbend"] = event.LBEnd
            dict_eff[lbzero, "offlumi"] += event.OffLumi
            dict_eff[lbzero, "lhcfill"] = int(event.FillNum)

    print("Making merged file for run", infilename)
    outfile = infilename.replace("run", "merged")
    csvfile = open(outfile, 'w')
    csvwriter = csv.writer(csvfile, delimiter=',')
    csvwriter.writerow(['ZeeRate', 'ZmumuRate', 'LBStart','LBEnd','FillNum','LBLive','OffLumi'])
    timeformat = '%y/%m/%d %H:%M:%S'
    for lbzero in sorted([key[0] for key in dict_eff.keys() if "ZeeEff" in key]):
        if dict_eff[lbzero, "ZeeErr"] != 0:
            eff_zee = dict_eff[lbzero, "ZeeEff"]/dict_eff[lbzero, "ZeeErr"]
            err_zee = sqrt(1/dict_eff[lbzero, "ZeeErr"])
            zeerate = dict_eff[lbzero, "ZeeRaw"]/eff_zee/dict_eff[lbzero, "lblive"]
        else:
            zeerate = 0.0
       
        if dict_eff[lbzero, "ZmumuErr"] != 0:
            eff_zmumu = dict_eff[lbzero, "ZmumuEff"]/dict_eff[lbzero, "ZmumuErr"]
            err_zmumu = sqrt(1/dict_eff[lbzero, "ZmumuErr"])
            zmumurate = dict_eff[lbzero, "ZmumuRaw"]/eff_zmumu/dict_eff[lbzero, "lblive"]
        else:
            zmumurate = 0.0

        lblive  = dict_eff[lbzero, "lblive"]
        lbstart = time.strftime(timeformat, time.gmtime(dict_eff[lbzero, "lbstart"]))
        lbend   = time.strftime(timeformat, time.gmtime(dict_eff[lbzero, "lbend"]))
        offlumi = dict_eff[lbzero, "offlumi"]
        lhcfill = dict_eff[lbzero, "lhcfill"]

        csvwriter.writerow([zeerate, zmumurate, lbstart, lbend, lhcfill, lblive, offlumi])

    csvfile.close()


if __name__ == "__main__":
    R.gROOT.SetBatch(R.kTRUE)
    main()
