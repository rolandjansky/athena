OUT=Feb07_A

runJob(){
	input=$1
	output=$2
	otherOpt=$3
# Reco_tf.py --inputFileValidation False --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True)' --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON1 --maxEvents 2000
Reco_tf.py --inputFileValidation False --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True)' --inputAODFile $input --outputDAODFile ${output}.pool.root --reductionConf MUON0 MUON1 $otherOpt

mv log.AODtoDAOD log_${output}.AODtoDAOD
}

# INPUT=root://eosatlas//eos/atlas/user/m/mdobre/forRTTmc15/MC15_207.AOD.pool.root
# runJob $INPUT ${OUT}1 "--passThrough True"
# 
# INPUT=/net/s3_datad/Data15/MC15/AOD/mc15_13TeV.424107.Pythia8B_A14_CTEQ6L1_Jpsimu8mu8.merge.AOD.e5290_s2726_r7772_r7676/AOD.09264436._001199.pool.root.1
# runJob $INPUT ${OUT}2 "--passThrough True"

INPUT=root://eosatlas//eos/atlas/user/m/mdobre/forRTTdata15/Data16_207.AOD.pool.root
runJob $INPUT ${OUT}3

INPUT4=/home/dzhang/links/SAMPLES/R20/MC15/AOD/mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.AOD.e3601_s2576_s2132_r7773_r7676/AOD.07981188._000211.pool.root.1
runJob $INPUT4 ${OUT}4 "--passThrough True"

for i in `ls DAOD_MUON*.${OUT}*.pool.root`; do j=${i##DAOD_}; k=${j%%.pool.root}; checkSG.py $i > ${k}_SG.txt; checkFile.py $i > ${k}_cF.txt; done

grep "(B)" ${k}_cF.txt |sed 's/Aux.*$//'|awk '{a[$10]+=$3}END{for(x in a){print a[x]" "x}}'|sort -n|sed 's/HLT_.*/HLT/' |awk '{a[$2]+=$1}END{for(x in a)print a[x]" "x}'|sort -n >${k}_cFsummary.txt
