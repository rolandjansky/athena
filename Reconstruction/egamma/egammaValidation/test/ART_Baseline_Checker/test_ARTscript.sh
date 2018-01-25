#!/bin/sh

echo "**************Reco_tf**************"

art.py validate /afs/cern.ch/work/m/mscodegg/public/QualiTask/athena/Reconstruction/egamma/egammaValidation/test
art.py run /afs/cern.ch/work/m/mscodegg/public/QualiTask/athena/Reconstruction/egamma/egammaValidation/test outART

echo $'\n ************checkFile.py************ \n '

checkFile.py rootART/ART_Rec.root > checkFile_ART.txt
checkxAOD.py rootART/ART_Rec.root > checkxAOD_ART.txt

#checkFile.py rootBaseline/Base_Rec.root > checkFile_Baseline.txt
#checkxAOD.py rootBaseline/Base_Rec.root > checkxAOD_Baseline.txt

echo $'\n ************diff__Files************* \n '

diff checkFile_ART.txt checkFile_Baseline.txt > diffFile.txt
diff checkxAOD_ART.txt checkxAOD_Baseline.txt > diffxAOD.txt

echo $'\n ************job_options************* \n '

athena.py ../../share/job_options.py      
#athena.py ../../share/job_options_Base.py 

echo $'\n ************plotsMaker************* \n '

python ../../python/plotsMaker.py ART-monitoring.root Base-monitoring.root

echo $'\n *************FINISHED************** \n '