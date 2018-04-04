#! /bin/bash

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh
asetup AthAnalysis,21.2,latest  # needed to use getMetadata.py
localSetupPyAMI --skipConfirm 
myproxy-logon -d -v -s myproxy.cern.ch --stdin_pass < /afs/cern.ch/user/m/mcgensvc/delegation.pwd 
voms-proxy-init -voms atlas -valid 24:0 -noregen

echo "Started Central Page table generation script"
date

rm -rf XsecFiles/
rm -rf AllSampleInfo*
mkdir -p AllSampleInfo_mc15_13TeV/
mkdir -p AllSampleInfo_mc15_13TeV/SummaryInfo/
mkdir -p AllSampleInfo_mc16_13TeV/
mkdir -p AllSampleInfo_mc16_13TeV/SummaryInfo/

cp ScriptsCentralPage/KeywordList.txt AllSampleInfo_mc15_13TeV/SummaryInfo/
cp ScriptsCentralPage/KeywordList.txt AllSampleInfo_mc16_13TeV/SummaryInfo/

# this creates the main central page twikis
python ScriptsCentralPage/MainScript.py --mainFolder /afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/
python ScriptsCentralPage/MainScript.py --mainFolder /afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ --production mc15_13TeV

# creates the summary xsec pages
python ScriptsCentralPage/MTscripts/GetAllXsec.py

python ScriptsCentralPage/MergeDescriptions.py

# copy the summary xsec twiki pages to the public area
cp XsecFiles/* /afs/cern.ch/user/m/mcgensvc/www/mc15twiki/CentralMC15ProductionPage/XsecFiles/

