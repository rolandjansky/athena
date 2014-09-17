#!/bin/bash

echo "Checkout Athena Packages:"
#GetPackages
cmt co Control/AthenaMonitoring
cmt co MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtil
cmt co MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonFlags
cmt co MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonitoring
cmt co MuonSpectrometer/MuonValidation/MuonDQA/MuonTrkPhysMonitoring
cmt co Reconstruction/RecJobTransforms
cmt co Tools/PyJobTransformsCore
cmt co DataQuality/DataQualityUtils
