#!/bin/bash 

if [[ $# -eq 0 ]]; then
    echo "Too few arguments: Specify prediction version of reprocessing (e.g. 429), predicted luminosity (e.g. 1.0e33),  the run number if using newer run (e.g. 178044 or 179939 or else use 166466), if you want an EF processing add EF else L2, and ros-rob map (ros prediction (e.g. 2010, 2011, 2012))"

    echo "New option to disable the prefetching in Cost data by specifying a 6th option: no or you can try a new prefetching by specifying: new"
    return 1
fi

version=$1
pred_lumi=$2
runnr=$3
level=$4
#ros_rob_map=rob-ros-$5.xml
ros_rob_map=rob-ros-robin-$5.xml
storage=perm
dataSet=data10_7TeV

ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_166383.xml
dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_166383-167844.xml

if [[ $runnr -gt 170000 ]]
then
    dataSet=data11_7TeV
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_177682.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi

if [[ $runnr -gt 179930 ]]
then
    dataSet=data11_7TeV
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_179938.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi

if [[ $runnr -gt 185643 ]]
then
    dataSet=data11_7TeV
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_$runnr.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi

if [[ $runnr -gt 189420 ]]
then
    dataSet=data11_7TeV
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_$runnr.xml
    #ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_191426.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi

if [[ $runnr -gt 200862 ]]
then
    dataSet=data12_8TeV
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_$runnr.xml
    #ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_191426.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi

if [[ $runnr -gt 210180 ]]
then
    dataSet=data12_hip
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_HI_$runnr.xml
    #ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_191426.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi


if [[ $runnr -gt 212980 ]]
then
    dataSet=data12_8TeV
    storage=temp
    ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_$runnr.xml
    #ebxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_191426.xml
    dqxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_$runnr.xml

    echo "Using dataset $dataSet for run $runnr"

fi

#
# For the old CAF storage style
#
#ebntp=/castor/cern.ch/grid/atlas/caf/atlcal/$storage/trigger/$dataSet/physics_EnhancedBias/00$runnr/$dataSet.00$runnr.physics_EnhancedBias.TrigCost$level.NTUP.c$version
#ebntp=/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data11_7TeV.00179939.physics_EnhancedBias.TrigCostL2.NTUP.c$version

#
# Example of find the new style on eos directory
#
#ebntp=/eos/atlas/atlasdatadisk/${dataSet}/NTUP_TRIGCOST${level}/$version/data11_7TeV.00${runnr}.physics_EnhancedBias.recon.NTUP_TRIGCOST${level}.r2804_tid535652_00

ebntp=/eos/atlas/atlasdatadisk/${dataSet}/NTUP_TRIGCOST${level}/$version/

tig_dir=`eos ls $ebntp`

ebntp=/eos/atlas/atlasdatadisk/${dataSet}/NTUP_TRIGCOST${level}/$version/$tig_dir

echo "Using Data from castor path: $ebntp"

rosxml=$TestArea/Trigger/TrigCost/TrigCostRate/data/

logname="log_"${pred_runnum}
outputdir=/tmp/schae
outputdir=/afs/cern.ch/user/t/trigcomm/scratch0

if [ $# -eq 4 ]; then
    echo "Predicting L1 and L2 rates to version c$version at Luminosity $pred_lumi"
    $TestArea/Trigger/TrigCost/TrigCostRate/share/runPreds.py \
	$ebntp \
	-o $outputdir/rate-$runnr_${version}-${pred_lumi}-${level} \
	--data \
	-k TRIGCOST \
	--bg-scaleXML=$ebxml \
	--predict-rates \
	--pred-lumi=$pred_lumi \
        --default-ps=1 \
        --default-pt=0 \
        --dq-xml=$dqxml

elif [ $# -eq 5 ]; then
    echo "Predicting ROS rates to Luminosity $pred_lumi"
    $TestArea/Trigger/TrigCost/TrigCostRate/share/runCosts.py \
	$ebntp \
	-o $outputdir/cost-$runnr_${version}-${pred_lumi}-${level} \
	--prescaleXML=output_xml_${lumi_input}.xml \
	--data \
	-k TRIGCOST \
	--rosXML=$rosxml$ros_rob_map \
	--do-bg \
	--bg-scaleXML=$ebxml \
	--pred-lumi=$pred_lumi \
	--default-ps=1 \
	--default-pt=0 \
	--dq-xml=$dqxml \
	#--nevent=33000 \
        --full-lb 500 \
	-r $outputdir/cost-$runnr_${version}-${pred_lumi}-${level}.root 

elif [ $# -eq 6 ]; then

    PreFetchType=$6"Prefetch"
    
    echo "Predicting ROS rates to Luminosity $pred_lumi and Checking the ROS Prefetching"
    $TestArea/Trigger/TrigCost/TrigCostRate/share/runCosts.py \
	$ebntp \
	-o $outputdir/cost-$runnr_${version}-${pred_lumi}-${level}-${PreFetchType} \
	--prescaleXML=output_xml_${lumi_input}.xml \
	--data \
	-k TRIGCOST \
	--rosXML=$rosxml$ros_rob_map \
	--do-bg \
	--bg-scaleXML=$ebxml \
	--pred-lumi=$pred_lumi \
	--default-ps=1 \
	--default-pt=0 \
	--dq-xml=$dqxml \
	--test=$PreFetchType \
	-r $outputdir/cost-$runnr_${version}-${pred_lumi}-${level}-${PreFetchType}.root 
else
    echo "Error incorrect number of inputs: Specify prediction version of reprocessing (e.g. 429), predicted luminosity (e.g. 1.0e33), EF or L2, ros-rob map (ros prediction (e.g. rob-ros-Feb2011.xml))"
fi

echo "Please find the trigger rates here: $outputdir/rate-$runnr_${version}-${pred_lumi}-${level}-${PreFetchType}"
echo "Please find the ros rates here    : $outputdir/cost-$runnr_${version}-${pred_lumi}-${level}-${PreFetchType}"

