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
```
# Time dependent efficiency and luminosity plots
python plotting/efficiency.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv
python plotting/luminosity.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv

# Pileup dependent efficiency and luminosity plots
python plotting/efficiency.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv --usemu
python plotting/luminosity.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv --usemu
```

# Running over the entire dataset
```
./run_code.sh 17 local # to run jobs locally in a loop
./run_code.sh 17 batch # to submit all jobs to the batch
```

# Making plots for the entire dataset
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
```
for year in 15 16 17 18
do
    python -u utils/make_benedetto_files.py --year $year
done

# LHCC files are made one at a time, i.e.
python -u utils/make_lhc_files.py --infile ~/public/Zcounting/CSVOutputs/HighMu/data17_13TeV/run_340030.csv
```
