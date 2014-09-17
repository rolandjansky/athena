#!/bin/bash

echo "Cleaning All Packages:"

WORKDIR=${TestArea}
cd ${WORKDIR}/Control/AthenaMonitoring/cmt ; make clean
cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils/cmt ; make clean
cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonFlags/cmt ; make clean
cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonitoring/cmt ; make clean
cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonTrkPhysMonitoring/cmt ; make clean
cd ${WORKDIR}/Reconstruction/RecJobTransforms/cmt ; make clean
cd ${WORKDIR}/Tools/PyJobTransformsCore/cmt ; make clean
cd ${WORKDIR}/DataQuality/DataQualityUtils/cmt ; make clean
cd ${WORKDIR}


