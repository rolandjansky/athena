#!/bin/sh

#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# 
# This script is a temporary workaround to generate counts file HLTChain.txt from TrigUpgradeTest expert-monitoring.root
# It will be needed only as long as we don't implement the ChainAcceptance histogram in the HLT monitoring
#

histFile=$1
if [ ! -f ${histFile} ]; then
  echo "Usage: $0 file.root"
  exit 1
fi

rootScript="TFile f(\"$histFile\"); \
TH2* h2d=(TH2*)f.Get(\"TrigSteer_HLT/SignatureAcceptance\"); \
int biny=h2d->GetYaxis()->FindBin(\"Output\"); \
TH1* h1d=(TH1*)h2d->ProjectionX(\"\",biny,biny); \
for (int i=0; i<=h1d->GetNbinsX(); ++i) {int num=h1d->GetBinContent(i); printf(\"%s %d\\n\",h1d->GetXaxis()->GetBinLabel(i),num);}"

root -b -l -q -e "${rootScript}" | grep 'HLT_' | sort > HLTChain.txt

