WORKDIR=${TestArea}

cd ${TestArea}

rm -rf InstallArea

cd ${TestArea}/MuonSpectrometer/MuonValidation/MuonDQA/MuonTrkPhysMonitoring;
svn up ;

cd cmt ;
#cmt config ;

#source setup.sh ;
make clean ;
make ;

 cd ${TestArea}




#cd ${WORKDIR}/Control/AthenaMonitoring/cmt ; cmt config ; source setup.sh ; make
#cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAUtils/cmt ; cmt config ; source setup.sh ; make
#cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonFlags/cmt ; cmt config ; source setup.sh ; make
#cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonDQAMonitoring/cmt ; cmt config ; source setup.sh ; make 
#cd ${WORKDIR}/MuonSpectrometer/MuonValidation/MuonDQA/MuonTrkPhysMonitoring/cmt ; cmt config ; source setup.sh ; make
#cd ${WORKDIR}/Reconstruction/RecJobTransforms/cmt ; cmt config ; source setup.sh ; make
#cd ${WORKDIR}/Tools/PyJobTransformsCore/cmt ; cmt config ; source setup.sh ; make 
#cd ${WORKDIR}/DataQuality/DataQualityUtils/cmt ; cmt config ; source setup.sh ; make
cd ${WORKDIR}

