#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                    

import os
import argparse
import pandas as pd

parser = argparse.ArgumentParser()
parser.add_argument('--year', type=str, help='15-18')
args    = parser.parse_args()
year    = args.year

indir  = "/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu/"
outdir = "/eos/user/m/miokeefe/public/ZC_BenedettoFiles/"
indir  = os.path.join(indir, "data" + year + "_13TeV")
outdir = os.path.join(outdir, "data" + year + "_13TeV")

os.system("mkdir -p " + outdir)

def main():
    for file in os.listdir(indir):
        if "run" not in file: 
            continue
        
        df = pd.read_csv(os.path.join(indir, file))
        df = df.drop(df[df.ZllLumi == 0].index)
        df = df.drop(df[df.PassGRL == 0].index)
        
        df['InstZllLumi']    = df['ZllLumi']
        df['InstZllLumiErr'] = df['ZllLumiErr']
        df['IntZllLumi']     = df['ZllLumi'] * df['LBLive']
        df['IntZllLumiErr']  = df['ZllLumiErr'] * df['LBLive']

        df = df[['LBNum','LBLive','InstZllLumi','InstZllLumiErr','IntZllLumi','IntZllLumiErr']]
        df.to_csv(os.path.join(outdir, file), index = False)

if __name__ == "__main__":
    main()
