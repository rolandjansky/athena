OUT=Mar03_A

runJob(){
	input=$1
	output=$2
# 	Reco_tf.py --inputFileValidation False --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON0 MUON1 MUON2 MUON3 MUON4 --maxEvents 1000
# 	Reco_tf.py --inputFileValidation False --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON0 MUON2 MUON3 --maxEvents 1000
# 	Reco_tf.py --inputFileValidation False --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON0 --maxEvents 1000
# 	Reco_tf.py --inputFileValidation False --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON0 --maxEvents 1000 --postExec="svcMgr.IOVDbSvc.CacheTime=100000"
	Reco_tf.py --inputFileValidation False --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON0
}

OUT1=${OUT}1
# INPUT=/home/dzhang/links/SAMPLES/R20/MC15/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s2576_s2132_r6630_tid05348608_00/AOD.05348608._004042.pool.root.1
INPUT=root://eosatlas//eos/atlas/user/m/mdobre/forRTTmc15/MC15_AOD.pool.root
runJob $INPUT $OUT1
mv log.AODtoDAOD log_${OUT1}.AODtoDAOD

# OUT2=${OUT}2
# INPUT=/home/dzhang/links/SAMPLES/R20/MC15/mc15_13TeV.424100.Pythia8B_A14_CTEQ6L1_Jpsimu4mu4.merge.AOD.e3735_s2608_s2183_r6630_r6264_tid05382368_00/AOD.05382368._000014.pool.root.1
# runJob $INPUT $OUT2
# mv log.AODtoDAOD log_${OUT2}.AODtoDAOD

OUT3=${OUT}3
# INPUT=~/links/SAMPLES/R20/Data15/AOD/data15_13TeV.00267639.physics_Main.merge.AOD.r6818_p2358_tid05695962_00/AOD.05695962._000894.pool.root.1
# INPUT=root://eosatlas//eos/atlas/user/m/mdobre/forRTTdata15/Data15_AOD.pool.root
INPUT=root://eosatlas//eos/atlas/user/m/mdobre/forRTTdata15/Data15_207.AOD.pool.root
runJob $INPUT $OUT3
mv log.AODtoDAOD log_${OUT3}.AODtoDAOD

for i in `ls DAOD_MUON*.${OUT}*.pool.root`; do j=${i##DAOD_}; k=${j%%.pool.root}; checkSG.py $i > ${k}_SG.txt; checkFile.py $i > ${k}_cF.txt; done
