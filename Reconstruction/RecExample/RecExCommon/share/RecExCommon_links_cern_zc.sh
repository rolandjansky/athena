echo "See https://twiki.cern.ch/twiki/bin/view/Atlas/RecoReferenceFiles for more info "

echo "obsolete ! exiting now !"

exit 0
#OBSOLETE SHOULD USE ATLASGROUPDISK
# export STAGE_SVCCLASS=atlasgroupdisk
#echo "ATLAS-DC3-02 stream inclusive electron  "
#pool_insertFileToCatalog.py rfio:/castor/cern.ch/grid/atlas/dq2/streamtest/streamtest.004947.inclEle.digit.RDO.v12000399/streamtest.004947.inclEle.digit.RDO.v12000399._00002.pool.root.8
#FCregisterLFN -p rfio:/castor/cern.ch/grid/atlas/dq2/streamtest/streamtest.004947.inclEle.digit.RDO.v12000399/streamtest.004947.inclEle.digit.RDO.v12000399._00002.pool.root.8 -l streamInclEle_DC3-02_RDO.pool

#echo "ATLAS-DC3-02 stream inclusive muon  "
#pool_insertFileToCatalog.py rfio:/castor/cern.ch/grid/atlas/dq2/streamtest/streamtest.004893.inclMuo.digit.RDO.v12000399/streamtest.004893.inclMuo.digit.RDO.v12000399._00001.pool.root.8 
#FCregisterLFN -p rfio:/castor/cern.ch/grid/atlas/dq2/streamtest/streamtest.004893.inclMuo.digit.RDO.v12000399/streamtest.004893.inclMuo.digit.RDO.v12000399._00001.pool.root.8 -l streamInclMuo_DC3-02_RDO.pool

#echo "ATLAS-DC3-02 stream inclusive jet  "
#pool_insertFileToCatalog.py rfio:/castor/cern.ch/grid/atlas/dq2/streamtest/streamtest.004946.inclJet.digit.RDO.v12000399/streamtest.004946.inclJet.digit.RDO.v12000399._00002.pool.root.8 
#FCregisterLFN -p rfio:/castor/cern.ch/grid/atlas/dq2/streamtest/streamtest.004946.inclJet.digit.RDO.v12000399/streamtest.004946.inclJet.digit.RDO.v12000399._00002.pool.root.8 -l streamInclJet_DC3-02_RDO.pool


#echo "ATLAS-DC3-07 top  "
#pool_insertFileToCatalog.py rfio:/castor/cern.ch/atlas/csc/valiprod/sampleA/testIdeal_07/005200.T1_McAtNlo_Jimmy/digit/v12000201/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1
#FCregisterLFN -p rfio:/castor/cern.ch/atlas/csc/valiprod/sampleA/testIdeal_07/005200.T1_McAtNlo_Jimmy/digit/v12000201/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1 -l top_DC3-07_RDO.pool







#echo "Geant4 Rome-initial RDO castor files (only at cern)" 
#pool_insertFileToCatalog rfio:/castor/cern.ch/user/d/drebuzzi/q02initialprod/dig9.0.4/forAOD/q02initialprod.0001.H_2e2mu.q02dig.etacut.0001.pool.root
#FCregisterLFN -p rfio:/castor/cern.ch/user/d/drebuzzi/q02initialprod/dig9.0.4/forAOD/q02initialprod.0001.H_2e2mu.q02dig.etacut.0001.pool.root -l heemm_initial_RDO.pool



#echo bytestream file

#ln -fs /castor/cern.ch/grid/atlas/datafiles/dc2/mixing/dc2.003621.mixing.all_phys/dc2_mixing_all_phys_0003621_file100002.data.1 dc2_mixing_all_phys_0003621_file100002.data
