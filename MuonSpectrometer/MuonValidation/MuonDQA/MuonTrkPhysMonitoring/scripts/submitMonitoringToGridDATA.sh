#!/bin/sh
 
####source setpanda.sh
export date="$(date '+%y%m%d')"
export inputdata="data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_SGLMU.t0pro12_v01/"

### use your lxplus login name here !!!!
export outdata="user.nectar.data12_8TeV.periodA4.physics_Muons.PhysCont.DESD_SGLMU.t0pro12_v01.$date"
echo "--------------------------------------------------------------------------------------------------------"
echo "Submitting job with input dataset ${inputdata}"
echo "and output dataset ${outdata}"
pathena --nFilesPerJob 5 runargs.MuonMon.ESDtoAOD.py RecJobTransforms/skeleton.ESDtoAOD.py ESDtoAOD_messageSvc_jobOptions.py  --inputType=ESD --nGBPerJob=10 --inDS $inputdata --outDS $outdata --site=AUTO --dbRelease=LATEST     

 
