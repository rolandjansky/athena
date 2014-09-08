#!/usr/bin/env zsh

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <lumi(E27)>"
    echo "#-----"
    echo "Example: $0 20 # to estimate the rate for 20*10^27 (cm-2s-1)"
    exit 1
fi

RateScale=$1
echo 'ratescale',$RateScale
# Input and output data directories
#datadir=/u2/snelson/data10_7TeV.00152845.calibration_L2CostMonitoring.Reproc.NTUP_COSTMON.c135/
#datadir=/rscratch/pcatr18/tkohno/work/menu/data10_7TeV.00152845.calibration_L2CostMonitoring.Reproc.NTUP_COSTMON.c135/
#n=300000
#rate_scale = 1.

n=1000000
datadir=/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_7TeV.00152845.physics_MinBias.daq.RAW_02-05-2010_MCInitialBeamV3_AtlasCAFHLT-rel_6-15.5.X.Y.Z
#rate_scale=1.46
#looks better for whole samples
rate_scale=1.8

outDir=/tmp/$USER/rates/testData_$1

#group_def=/afs/cern.ch/user/t/tkohno/public/menu/GroupRatesDef_Data.xml \
group_def=GroupRatesDef_Data.xml \
dq_xml=/afs/cern.ch/user/t/tkohno/public/menu/exampleDQ.xml
l1xml=L1Menu_30.xml
hltxml=HLTMenu_30.xml
#l1xml=L1Menu_1364_1E27.xml
#hltxml=HLTMenu_1301_1E27.xml
InputMenuLumi=30
#l1xml=L1Menu_1405_20E27_J5E10.xml
#hltxml=HLTMenu_1358_20E27.xml
#InputMenuLumi=20

ps_rules=$TestArea/Trigger/TriggerCommon/TriggerMenu/python/PrescaleRules_InitialBeam_v3.py

#suffix="_STANDBY_CaloNoise33"
#suffix="_CaloNoise33"
#suffix="_CaloNoise33_vdM"
suffix=""
#suffix="_STANDBY"
#suffix="_vdM"
suffixB=""
#suffixB="_4b_2_2_2"
#suffixB="_6b_3_3_3"

echo 'here: make sets'
# Generate prescale sets
python addPS.py --l1xml=$l1xml \
    --hltxml=$hltxml \
    --target-lumi=$RateScale \
    --current-lumi=$InputMenuLumi \
    --rules=$ps_rules \
    -o ps_online${RateScale}${suffix}${suffixB}.xml
#    --vdM \
#    --CaloNoise33 \
#    --standby
echo 'here: made sets'
#ln -s /afs/cern.ch/user/t/tkohno/public/menu/cnvXml.py . 
python cnvXml_express.py --l1xml=${l1xml} --hltxml=${hltxml} --ps_name=IB_v3_${RateScale}${suffix}${suffixB} --psxml=ps_online${RateScale}${suffix}${suffixB}.xml
#


rm -fr $outDir
mkdir $outDir

MyRateScale=$(($RateScale*$rate_scale))
#echo rate: $MyRateScale

$TestArea/PhysicsNtuple/TrigMonCosts/macros/runRates.py \
    --groupXML=$group_def \
    --prescaleXML=ps_online${RateScale}.xml \
    --rate-scale=$MyRateScale \
    --dq-xml=$dq_xml \
    --data \
    -k TrigRate \
    --predict-rates \
    -n $n \
    -o $outDir \
    $datadir
   
echo "Write output to " $outDir
