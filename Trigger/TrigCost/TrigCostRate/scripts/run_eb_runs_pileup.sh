#!/bin/bash

if [[ $# == 0 ]]; then
    echo "Too few arguments: Specify prediction tag_name, lumi, prediction run_number (optional), prediction lumi block (optional)"
    return 1
fi

ebntp=/eos/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data11_7TeV.189421.physics_EnhancedBias.merge.RAW.27-09-2011_Physicsppv3_AtlasP1HLT-16.1.3.14
ebxml=${TestArea}/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_189421.xml
dqxml=${TestArea}/Trigger/TrigCost/TrigCostRate/data/dq_189421.xml
readTinymulbXml=189421_lb_mu.xml
readTinyEosXml=ReadTiny_eos.xml
readTinyCastorXml=ReadTiny_BCID.xml

logname="log_"${pred_runnum}

echo $#" arguments"

if [ $# == 6 ] ; then

	doWithPileup=1
	doWithoutPileup=1
	doData=1
	doComparison=1

    tag_name=$1
    lumi_input=$2
    pred_run=$3
    pred_lb=$4
    pred_mu=$5
    powerOffset=$6

    echo "Prediction to Run number $pred_run at Lumi block $pred_lb with pileup corrections"
    echo "Luminosity will come from online DB"
    
    output=${tag_name}
    rm -rf ${output}
    mkdir ${output}
	
    output_1=${output}/Rates_withPileup_${pred_run}_${pred_lb}
    output_2=${output}/Rates_withoutPileup_${pred_run}_${pred_lb}
    output_3=${output}/Rates_TRP
    
    mkdir ${output_1}
    mkdir ${output_2}
    mkdir ${output_3}
    
    if [ $doWithPileup == 1 ] ; then
   		# With pileup corrections
    	${TestArea}/Trigger/TrigCost/TrigCostRate/share/runRates.py \
    		${ebntp} \
    		-o ${output_1} \
    		--data \
    		--predict-rates \
			--pred-run=${pred_run} \
    		--pred-lb=${pred_lb} \
    		--default-ps=1 \
    		--default-pt=-1 \
    		--dq-xml=${dqxml} \
    		--do-bg \
    		--bg-scaleXML=${ebxml} \
    		--nColl=${pred_mu} \
    		--doPileup \
    		--pileupWeightFilename=${output_1}/pileupWeights.root \
    		--readTiny-lb-mu-xml=${readTinymulbXml} \
			--readTiny-eos-xml=ReadTiny_eos.xml \
			--readTiny-bcid-xml=ReadTiny_BCID.xml \
			--pileupMinProbCut=0.01 \
			--pileupPowerOffset=${powerOffset}
	fi
	
	echo ""
	echo "==========================="
	echo "==== End of first job  ===="
	echo "==========================="
	echo ""
    
    if [ $doWithoutPileup == 1 ] ; then
    	# Without pileup corrections
    	${TestArea}/Trigger/TrigCost/TrigCostRate/share/runRates.py \
    		${ebntp} \
    		-o ${output_2} \
    		--data \
    		--predict-rates \
    		--pred-run=${pred_run} \
    		--pred-lb=${pred_lb} \
    		--dq-xml=${dqxml} \
    		--do-bg \
    		--bg-scaleXML=${ebxml} \
    		--default-ps=1 \
    		--default-pt=-1 \
    		--readTiny-lb-mu-xml=${readTinymulbXml}
   	fi
   	
   	echo ""
   	echo "==========================="
	echo "==== End of second job ===="
	echo "==========================="
	echo ""
   	
   	if [ $doData == 1 ] ; then
    	# Data
    	${TestArea}/Trigger/TrigCost/TrigCostPython/macros/exampleCost.py \
    		--trp \
    		--lb-beg=${pred_lb} --lb-end=${pred_lb} \
    		--run ${pred_run} \
    		--dorates \
    		--writexml ${output_3}/output.xml
    fi
    
    echo ""
	echo "==========================="
	echo "==== End of third job  ===="
	echo "==========================="
	echo ""
    
    ratename="output_"${pred_run}"_"${pred_lb}"_"${pred_lb}".xml"
    
    if [ $doComparison == 1 ] ; then
	    # Comparisons
	    $TestArea/Trigger/TrigCost/TrigCostPython/macros/compRates.py \
   	 		${output_1}/TriggerCosts.xml \
    		${output_2}/TriggerCosts.xml \
    		${output_3}/${ratename} \
    		-o ${output}/compare \
    		--labels="With pileup","Without pileup","Online (TRP)" \
    		--ratio-columns=3 \
        	--show-err \
    		--show-diff \
        	--summary-html \
        	--summary-name=compare_rates.html
	fi
	
fi


if [ $# == 3 ] ; then
    echo "Prediction to lumi $2 at mu $3 with pileup corrections"
    echo "Luminosity will come from online DB"
    
    tag_name=$1
    pred_lumi=$2
    pred_mu=$3
    
    output=${tag_name}
    rm -rf ${output}
    mkdir ${output}

    output_1=${output}/Rates_withPileup
    mkdir ${output_1}
    ${TestArea}/Trigger/TrigCost/TrigCostRate/share/runRates.py \
    	${ebntp} \
    	-o ${output_1} \
        --data \
        --lumi=${pred_lumi} \
    	--predict-rates \
    	--default-ps=1 \
        --default-pt=-1 \
        --dq-xml=${dqxml} \
        --do-bg \
    	--bg-scaleXML=${ebxml} \
        --nColl=${pred_mu} \
    	--doPileup \
    	--pileupWeightFilename=${output_1}/pileupWeights.root \
    	--readTiny-lb-mu-xml=${readTinymulbXml} \
		--readTiny-eos-xml=ReadTiny_eos.xml \
		--readTiny-bcid-xml=ReadTiny_BCID.xml  
fi
