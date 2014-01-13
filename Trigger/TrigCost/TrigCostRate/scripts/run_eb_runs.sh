#!/usr/bin/env zsh 

if [[ $# -eq 0 ]]; then
    echo "Too few arguments: Specify either\n run_eb_run.sh <directory name> <prediction lumi>\n run_eb_run.sh <directory name> 0 <prediction run_number> <prediction lumi block>\n e.g. run_eb_runs.sh testdir 1e33\n e.g. run_eb_runs.sh testdir 0 178044 270"
    return 1
fi

tag_name=$1
lumi_input=$2
pred_runnum=$3
pred_lumi_lb=$4

ebntp=/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_7TeV.00166383.166466.166658.physics_EnhancedBias.merge.RAW.15-02-2011_mcppv2_AtlasP1HLT-16.1.1.2/

ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_166383.xml
dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_166383-167844.xml

logname="log_"${pred_runnum}

if [[ $# -eq 4 ]] 
then

    echo "Prediction to Run number $pred_runnum at Lumi block $pred_lumi_lb"
    echo "Luminosity will come from online DB"

    output=${tag_name}Rates_${pred_runnum}-${pred_lumi_lb}

    $TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py \
	$ebntp \
	-o $output \
        --data \
	--do-bg \
	--bg-scaleXML=$ebxml \
	--predict-rates \
	--pred-run=$pred_runnum \
	--pred-lb=$pred_lumi_lb \
        --default-ps=-1 \
        --default-pt=-1 \
        --dq-xml=$dqxml 


#    echo "Doing Compare"
    $TestArea/Trigger/TrigCost/TrigCostPython/macros/exampleCost.py \
	--trp \
	--lb-beg=${pred_lumi_lb} --lb-end=${pred_lumi_lb} \
	--run ${pred_runnum} \
	--dorates \
	--writexml $output/output.xml

    ratename="output_"${pred_runnum}"_"${pred_lumi_lb}"_"${pred_lumi_lb}".xml"
    echo "Output file name" $ratename
    
    $TestArea/Trigger/TrigCost/TrigCostPython/macros/compRates.py \
	$output/TriggerCosts.xml \
	$output/$ratename \
	-o $output/$dirname \
	--labels=PRED,ONLINE \
	--ratio-columns=2 \
	--show-err \
	--show-diff \
        --summary-html \
        --summary-name=compare_rates.html

else
    echo "Prediction to Luminosity $lumi_input"
    $TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py \
       $ebntp \
       -o ${tag_name}Rates_${lumi_input} \
	--prescaleXML=output_xml_${lumi_input}.xml \
       --data \
       --do-bg \
       --bg-scaleXML=$ebxml \
       --predict-rates \
       --default-ps=1 \
       --default-pt=-1 \
       --pred-lumi=$lumi_input \
       --dq-xml=$dqxml
       
fi
