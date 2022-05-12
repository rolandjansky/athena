#!/bin/bash
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

cp $1/../DataQualityInterfaces/share/* .
han-config-gen histogramAnalysis.config || exit 1
han histogramAnalysis.hcfg Data.root "" Data_han.root > /dev/null || exit 2
han-results-print Data_han.root || exit 3
han-config-gen histogramAnalysisRegex.config || exit 4
han histogramAnalysisRegex.hcfg Data.root "" Data_han_regex.root > /dev/null || exit 5
han-results-print Data_han_regex.root || exit 6