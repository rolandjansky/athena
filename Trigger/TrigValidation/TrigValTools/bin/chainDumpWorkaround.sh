#!/bin/sh

#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

#
# This script is a temporary workaround to generate counts file HLTChain.txt from TrigUpgradeTest expert-monitoring.root
# It will be needed only as long as we don't implement the ChainAcceptance histogram in the HLT monitoring
#

histFile=$1
if [ -z "${histFile}" ] || [ ! -f ${histFile} ]; then
  echo "Usage: $(basename -- $0) file.root"
  exit 1
fi

#
# HLT counts
#
rootScript="TFile f(\"$histFile\"); \
TH2* h2d=(TH2*)f.Get(\"HLTFramework/TrigSignatureMoniMT/SignatureAcceptance\"); \
if (!h2d) exit(1); \
int biny=h2d->GetYaxis()->FindBin(\"Output\"); \
TH1* h1d=(TH1*)h2d->ProjectionX(\"\",biny,biny); \
for (int i=0; i<=h1d->GetNbinsX(); ++i) {int num=h1d->GetBinContent(i); printf(\"%s %d\\n\",h1d->GetXaxis()->GetBinLabel(i),num);}"

root -b -l -q -e "${rootScript}" | grep 'HLT_' | sort > HLTChain.txt
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
  echo "WARNING Failed to extract HLTChain counts from ${histFile}"
fi

#
# Total number of events
#
rootScript="TFile f(\"$histFile\"); \
TH2* h2d=(TH2*)f.Get(\"HLTFramework/TrigSignatureMoniMT/SignatureAcceptance\"); \
if (!h2d) exit(1); \
int binx=h2d->GetXaxis()->FindBin(\"All\"); \
int biny=h2d->GetYaxis()->FindBin(\"L1\"); \
printf(\"%d\\n\",(int)h2d->GetBinContent(binx,biny)); \
exit(0);"

root -b -l -q -e "${rootScript}" | grep -v '^$' > TotalEventsProcessed.txt
if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
  echo "WARNING Failed to extract TotalEventsProcessed from ${histFile}"
fi

