# Introduction
This framework runs over the merged outputs produced by running the DQTGlobalWZFinder tool over primary AODs.
Information on how to run the main code can be found here [https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ZCountingLumi](https://twiki.cern.ch/twiki/bin/viewauth/Atlas/ZCountingLumi). 
Once the merging step has been performed (an example of which is below), 
the dqt_zlumi_pandas.py script can be ran over the merged output to produce a single csv file containing all information.
Each row of the csv file corresponds to a single luminosity block, and contains all information for both channels;
such as the number of reconstructed Zs per channel, trigger efficiency, reconstruction efficiency, luminosity, 
as well as the arithmetic mean of the Zee and Zmumu luminosities and all auxilliary official information 
(livetime, pileup, luminosity, GRL).

```
ls grid-output/* > tomerge.txt
DQHistogramMerge.py tomerge.txt out.HIST_AOD.root
```

i.e. *out.HIST_AOD.root* here corresponds to *tree_340030.root* in the next example.

# Running the code
Using a single 2017 run as an illustrative example: 
```
source setup.sh
infile="/eos/user/m/miokeefe/public/FullRun2_Reproc/MergedOutputs/HighMu/data17_13TeV/tree_340030.root"
grl="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20190708/data17_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"
outdir="/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu/data17_13TeV"
python -u dqt_zlumi_pandas.py --dblivetime --useofficial --infile $infile --grl $grl --campaign mc16d --outdir $out_dir
```

# Making single run plots
_Note_: The output directory (outdir) will need to be changed at the top of both scripts.
Both of these scripts calculate an average over successive bunches of 20 luminosity blocks
to increase statistical precision. 
All other plots use the single-LB luminosity, and not the 20LB merged value, 
when calculating the integrated luminosity of an LHC fill/pileup bin.

```
# Time dependent efficiency and luminosity plots
python plotting/efficiency.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv
python plotting/luminosity.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv

# Pileup dependent efficiency and luminosity plots
python plotting/efficiency.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv --usemu
python plotting/luminosity.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv --usemu
```

# Running over the entire dataset
_Note_: The input (indir) and output (out_dir) directories will need to be changed at the top of the script run_code.sh.
```
submit=batch # set to local to run all jobs locally in a loop
for year in 15 16 17 18
do
    ./run_code.sh $year $submit
done
```

# Making plots for the entire dataset
_Note_: The input (indir) and output (outdir) directories will need to be changed in both scripts.
```
for year in 15 16 17 18
do
    # Yearwise L(ee) / L(mumu) comparison vs. time and pileup
    python -u plotting/yearwise_luminosity.py --year $year --comp
    python -u plotting/yearwise_luminosity_vs_mu.py --year $year --comp
    for channel in Zee Zmumu Zll
    do
        # Yearwise L_Z / L_ATLAS comparison vs. time and pileup
        python -u plotting/yearwise_luminosity.py --channel $channel --year $year
        python -u plotting/yearwise_luminosity_vs_mu.py --channel $channel --year $year
    done
done

```

# Making full Run-2 "super plots"
_Note_: The input (indir) and output (outdir) directories will need to be changed in both scripts.
```
# Full Run-2 L_Z / L_ATLAS plots vs. time
for channel in Zee Zmumu Zll
do
    python plotting/yearwise_luminosity.py --year all --channel $channel
done

# Full Run-2 L(ee) / L(mumu) plot vs. time and pileup
python -u plotting/yearwise_luminosity.py --year all --comp 
python -u plotting/yearwise_luminosity_vs_mu.py --year run2 --comp 
```

# Making Benedetto/LHCC format files
_Note_: The input (indir) and output (outdir) directories will need to be changed in utils/make_benedetto_files.py. 
```
for year in 15 16 17 18
do
    python -u utils/make_benedetto_files.py --year $year
done

# LHCC files are made one at a time, i.e.
python -u utils/make_lhc_files.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv
```
