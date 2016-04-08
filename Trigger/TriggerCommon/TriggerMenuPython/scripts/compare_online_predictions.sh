
if [[ $# -eq 0 ]]; then
    echo "Too few arguments: Specify  prediction run_number , prediction lumi block , output directory"
    return 1
fi

pred_runnum=$1
pred_lumi_lb=$2
out_dir=$3

mkdir ${out_dir}
mkdir ${out_dir}/${pred_runnum}_${pred_lumi_lb}
output_dir=${out_dir}/${pred_runnum}_${pred_lumi_lb}

ebntp=/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_7TeV.00162843.physics_EnhancedBias.merge.RAW_27-09-2010_Physicsppv1_AtlasCAFHLT-rel_1-15.6.X.Y.Z-VAL
ebntp1=/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_7TeV.00162882.physics_EnhancedBias.merge.RAW_27-09-2010_Physicsppv1_AtlasCAFHLT-rel_1-15.6.X.Y.Z-VAL

runnum=162764
lumi_lb=152

ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_162882.xml

logname="log_"$runnum

echo "Prediction to Run number $pred_runnum at Lumi block $pred_lumi_lb"
echo "Luminosity will come from online DB"
$TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py \
    $ebntp $ebntp1 \
    -o ${output_dir}/predRates_${pred_runnum}-${pred_lumi_lb} \
    --data \
    --do-bg \
    --bg-scale=$ebxml \
    --predict-rates \
    --pred-run=$pred_runnum \
    --pred-lb=$pred_lumi_lb



$TestArea/Trigger/TrigCost/TrigCostPython/macros/exampleCost.py --trp --dorates --run ${pred_runnum} --lb-beg=${pred_lumi_lb} --lb-end=${pred_lumi_lb} --writexml ${output_dir}/online.xml

luminosity=`$TestArea/Trigger/TrigCost/TrigCostPython/macros/exampleCost.py --cool --dolumi --run ${pred_runnum} --lb-beg=${pred_lumi_lb} --lb-end=${pred_lumi_lb} | grep Instantaneous`

lumi=`echo ${luminosity:(-24)} | tr -d ' '` 
lumi=${lumi:0:9}

$TestArea/Trigger/TrigCost/TrigCostPython/macros/compRates.py ${output_dir}/online_${pred_runnum}_${pred_lumi_lb}_${pred_lumi_lb}.xml ${output_dir}/predRates_${pred_runnum}-${pred_lumi_lb}/TriggerCosts.xml -o ${output_dir}/compare_${lumi} --labels=Run_${pred_runnum}_${pred_lumi_lb},Prediction_${lumi} --ratio-column=2 --show-pspt --show-err --do-groups --show-diff

$TestArea/Trigger/TrigCost/TrigCostPython/macros/histRates.py  ${output_dir}/online_${pred_runnum}_${pred_lumi_lb}_${pred_lumi_lb}.xml ${output_dir}/predRates_${pred_runnum}-${pred_lumi_lb}/TriggerCosts.xml --labels=Run_${pred_runnum}_${pred_lumi_lb}_${lumi},Prediction_${lumi} --ratelist L1_EM5,L1_EM10,L1_2EM5,L1_MU0,L1_MU6,L1_MU10,L1_2MU20,L1_XE10,L1_XE30,L1_TE100,L1_J55,L1_J95,L1_3J10,L1_3J30,L1_TAU5,L1_TAU50,L1_2TAU11,L1_MBTS_1,L1_ZDC --outname ${output_dir}/comparePlot_L1.pdf

$TestArea/Trigger/TrigCost/TrigCostPython/macros/histRates.py  ${output_dir}/online_${pred_runnum}_${pred_lumi_lb}_${pred_lumi_lb}.xml ${output_dir}/predRates_${pred_runnum}-${pred_lumi_lb}/TriggerCosts.xml --labels=Run_${pred_runnum}_${pred_lumi_lb},Prediction_${lumi} --ratelist EF_str_Egamma,EF_str_JetTauEtmiss,EF_str_Muons,EF_str_MinBias --outname ${output_dir}/comparePlot_PhysicsStreams.pdf
