#!/usr/bin/env bash

year=$1
batch=$2

CVMFS="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists"
indir="/eos/user/m/miokeefe/public/FullRun2_Reproc/MergedOutputs/HighMu/data${year}_13TeV"
out_dir="/afs/cern.ch/user/m/miokeefe/public/Zcounting/CSVOutputs/HighMu"

for infile in $(ls $indir | grep 340030)
do
    infile=${indir}/${infile}
    [ $year == 15 ] && grl="${CVMFS}/data15_13TeV/20190708/data15_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns.xml" && campaign=mc16a
    [ $year == 16 ] && grl="${CVMFS}/data16_13TeV/20190708/data16_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_WITH_IGNORES.xml" && campaign=mc16a
    [ $year == 17 ] && grl="${CVMFS}/data17_13TeV/20190708/data17_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml"  && campaign=mc16d
    [ $year == 18 ] && grl="${CVMFS}/data18_13TeV/20200426/data18_13TeV.periodAllYear_DetStatus-v102-pro22-04_PHYS_StandardGRL_All_Good_25ns_BjetHLT.xml" && campaign=mc16e
    
    if [[ $batch = "local" ]]
    then
        python -u dqt_zlumi_pandas.py --dblivetime --useofficial --infile $infile --grl $grl --campaign $campaign --outdir ${out_dir}/data${year}_13TeV
    elif [[ $batch = "batch" ]] 
    then
        run_dir=$(pwd) 
        run_num=$(echo $infile | sed 's/.*tree_//;s/.root//')
        mkdir -p batch_jobs/20${year}/${run_num}
        cd batch_jobs/20${year}/${run_num}

        echo "#!/usr/bin/env bash"                                                           >  batcher.sh 
        echo "export ATLAS_LOCAL_ROOT_BASE=\"/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase\"" >> batcher.sh 
        echo "source \${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh -q"                   >> batcher.sh 
        #echo "asetup 21.0.77,Athena,here"                                                    >> batcher.sh 
        echo "asetup 21.0,latest,Athena"                                                    >> batcher.sh 
        echo "python -u ${run_dir}/dqt_zlumi_pandas.py --dblivetime --useofficial --infile $infile --grl $grl --campaign $campaign --outdir ${out_dir}/data${year}_13TeV" >> batcher.sh 
        chmod +x batcher.sh
        
        echo "executable            = batcher.sh"       >  batcher.sub
        echo "arguments             = "                 >> batcher.sub
        echo "output                = batcher.out"      >> batcher.sub
        echo "error                 = batcher.err"      >> batcher.sub
        echo "log                   = batcher.log"      >> batcher.sub
        echo "+JobFlavour           = \"microcentury\"" >> batcher.sub
        #echo "+JobFlavour           = \"tomorrow\""     >> batcher.sub
        echo "queue"                                    >> batcher.sub

        condor_submit batcher.sub
        cd ../../../
    else
        echo "Unrecognised job option!!!"
        echo "Please resubmit with option either 'local' or 'batch'"
        exit
    fi
done
