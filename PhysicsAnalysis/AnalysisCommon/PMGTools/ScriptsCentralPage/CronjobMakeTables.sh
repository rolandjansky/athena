#! /bin/bash

export RUCIO_ACCOUNT=aknue
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
localSetupPyAMI

cd /afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/

cd ../

#svn co svn+ssh://svn.cern.ch/reps/atlasoff/Generators/MC15JobOptions/trunk MC15JobOptions

#cd ScriptsCentralPage

# if MC15JobOptions already exists
cd MC15JobOptions/
svn update
cd ../ScriptsCentralPage

#localSetupPyAMI

PROXY=/tmp/x509up_u`id -u`

localSetupDQ2Client --skipConfirm

timestamp=$(date +%s)

echo $timestamp

mkdir -p XsecFiles
mkdir -p AllSampleInfo/
mkdir -p AllSampleInfo/SummaryInfo/

cp KeywordList.txt AllSampleInfo/SummaryInfo/

python MainScript.py

python NewScripts/GetAllXsec.py

python MergeDescriptions.py

cp -r XsecFiles /afs/cern.ch/user/m/mcgensvc/www/mc15twiki/CentralMC15ProductionPage/

python MakeDSID_Dictionary.py

#/afs/cern.ch/user/a/atltopmc/www/TopMC12twiki/TestCentralMC15ProductionPage/

#/afs/cern.ch/user/m/mcgensvc/www/CentralMC15ProductionPage/

#/afs/cern.ch/user/a/atltopmc/www/TopMC12twiki/TestCentralMC15ProductionPage/

#cp -r /afs/cern.ch/user/m/mcgensvc/www/CentralMC15ProductionPage/* /afs/cern.ch/user/a/atltopmc/www/TopMC12twiki/TestCentralMC15ProductionPage/