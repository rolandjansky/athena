#!/bin/bash

cp $1/* . > /dev/null
han-config-gen histogramAnalysis.config || exit 1
han-config-gen histogramAnalysisRegex.config || exit 2
han histogramAnalysis.hcfg Data.root "" Data_han.root || exit 3
han histogramAnalysisRegex.hcfg Data.root "" Data_han_regex.root || exit 4
han-config-print histogramAnalysis.hcfg || exit 5
han-config-print histogramAnalysisRegex.hcfg || exit 6
han-config-gen histogramAnalysisGivenRef.config || exit 7
han-config-print histogramAnalysisGivenRef.hcfg || exit 8
han histogramAnalysisGivenRef.hcfg Data.root "" Data_han_givenref.root || exit 9