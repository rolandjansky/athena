#!/bin/bash
###--------------------------------------------------------
### Run job
# bsub -q 1nd source pileup_cost.sh 20 0 1000 &> /tmp/logt & tail -f /tmp/logt
#
###--------------------------------------------------------

###--------------------------------------------------------
### Check input options
if [ "$#" -ne "3" ]
then
    echo "source grid_trigmc.sh nPileUp collectMC nevents"
    echo "Incorrect number of parameters: $#"; exit 1
fi

#
# Set variables
#
user=`whoami`
mu=$1
RDOFILE=RDO.TMP._000003_mu${mu}.pool.root
nevents=$3
DIR=grid_"${mu}"
OUTDIR=/afs/cern.ch/user/t/trigcomm/w0/rates/${user}/testarea/GenTopTriggerMC

#OUTDIR=/project/data_d02_0/schaefer/testarea/GenTopTriggerMC 

cd /tmp/$user
mkdir -p $DIR
cd $DIR

echo "pwd: "`pwd`

#if [[ "$2" == *collectMC* ]]
#then

xrdcp  root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.105200.T1_McAtNlo_Jimmy.simul.HITS.e603_s932_tid168415_00/HITS.168415._000003.pool.root.1 .
xrdcp  root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.105200.T1_McAtNlo_Jimmy.simul.HITS.e603_s932_tid168415_00/HITS.168415._000083.pool.root.1 .
xrdcp  root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.105200.T1_McAtNlo_Jimmy.simul.HITS.e603_s932_tid168415_00/HITS.168415._000143.pool.root.1 .
xrdcp  root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/valid1.105200.T1_McAtNlo_Jimmy.simul.HITS.e603_s932_tid168415_00/HITS.168415._000583.pool.root.1 .


xrdcp root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952_tid277248_00/HITS.277248._000202.pool.root.1 .

xrdcp root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952_tid277248_00/HITS.277248._000062.pool.root.1 .

xrdcp root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952_tid277248_00/HITS.277248._000491.pool.root.1 .

xrdcp root://castoratlas//castor/cern.ch/atlas/atlascerngroupdisk/trig-daq/validation/test_data/mc10_7TeV.105000.pythia_minbias_inelastic.merge.HITS.e723_s932_s952_tid277248_00/HITS.277248._000415.pool.root.1 .

#fi

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'
source $AtlasSetup/scripts/asetup.sh TrigMC,16.6.7.3.1 --testarea $OUTDIR

# leave DBRelease out for the default
digi="Digi_trf.py inputHitsFile='HITS.168415._[000003,000083,000143,000583].pool.root.1' outputRDOFile=$RDOFILE maxEvents=$nevents  preInclude='SimuJobTransforms/preInclude.PileUpBunchTrains2011Config3_DigitConfig.py' postExec='ToolSvc.LArAutoCorrTotalToolDefault.NMinBias=0' geometryVersion=ATLAS-GEO-16-00-00 conditionsTag=OFLCOND-SDR-BS7T-04-11 AMITag=tmp digiSeedOffset1=207 digiSeedOffset2=207 NDMinbiasHitsFile='HITS.277248._[000415,000062,000491,000202].pool.root.1' numberOfNDMinBias=$mu.0 bunchSpacing=50 DataRunNumber=-1 jobNumber=207"

echo `pwd`
echo "$digi"
#####################################
#UNCOMMENT FOR FULL RUN!!!!!!!
#
Digi_trf.py inputHitsFile='HITS.168415._[000003,000083,000143,000583].pool.root.1' outputRDOFile=$RDOFILE maxEvents=$nevents  preInclude='SimulationJobOptions/preInclude.PileUpBunchTrains2011Config3_DigitConfig.py' postExec='ToolSvc.LArAutoCorrTotalToolDefault.NMinBias=0' geometryVersion=ATLAS-GEO-16-00-00 conditionsTag=OFLCOND-SDR-BS7T-04-11 AMITag=tmp digiSeedOffset1=207 digiSeedOffset2=207 NDMinbiasHitsFile='HITS.277248._[000415,000062,000491,000202].pool.root.1' numberOfNDMinBias=$mu.0 bunchSpacing=50 DataRunNumber=-1 jobNumber=207
#$digi
#
# Run Trigger
#
#cd /project/data_d02_0/schaefer/testarea/GenTopTriggerMC 
cd $OUTDIR

#####################################
#UNCOMMENT FOR FULL RUN!!!!!!!                                                                                  
## 
cp /tmp/$user/$DIR/$RDOFILE grid/.

#source setup_pro.sh
source $AtlasSetup/scripts/asetup.sh TrigMC,16.6.7.3.1 --testarea $OUTDIR

mkdir run_$mu

cd run_$mu

#get_files -jo testAthenaTrigRDO_MC_pp_v2_tight_mc_prescale.py
#get_files -jo RunTrigCost.py
cp $OUTDIR/Trigger/TrigCost/TrigCostRate/Pileup_Timing/RunTrigCost.py .
cp ${OUTDIR}/grid/RDO.TMP._000003_mu${mu}.pool.root /tmp/.

output="'${OUTDIR}/grid/RDO.TMP._000003_mu${mu}.pool.root'"
output="'/tmp/RDO.TMP._000003_mu${mu}.pool.root'"
ls /tmp/RDO.TMP._000003_mu${mu}.pool.root
athena.py -c "EvtMax=10000;from AthenaCommon.AthenaCommonFlags import athenaCommonFlags;athenaCommonFlags.FilesInput=[${output}]" RunTrigCost.py 

cp TrigCostAL.root TrigCostAL_mu"$mu".root

#echo "cost ntuple: /project/data_d02_0/schaefer/testarea/GenTopTriggerMC/run_$mu/TrigCostAL_mu$mu.root"
echo "cost ntuple: ${OUTDIR}/run_$mu/TrigCostAL_mu$mu.root"

#
# put your cost job here running on the output file RDO.TMP._000001_tmp.pool.root
#


