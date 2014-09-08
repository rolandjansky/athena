#!/bin/bash

#Hard-coded variables are the devil's work, but we use them anyway
#Luminosity points in e30 units
POINTS="200 170 150 140 120 100 90 80 70 60 55 50 45 40 35 30 25 20 15 10 9 8 7 6 5 4 3 2 1"

#Rulebook to use
BOOK=$TestArea/Trigger/TriggerCommon/TriggerMenu/rules/Physics_pp_rulebook_PT.txt
#Bandwidth file for the express stream
ES_REF1=$TestArea/Trigger/TriggerCommon/TriggerMenu/rules/bw_express.txt
#Rates for the bandwidth calculation
ES_REF2=$TestArea/Trigger/TriggerCommon/TriggerMenu/rules/online_rates_1_39e31_165632_350_400.xml
INPUT_RATE=13900

#Get the menu from the release
L1XML=`ls ${AtlasArea}/Trigger/TriggerCommon/TriggerMenuXML/data/LVL1config_Physics_pp_v1_*.xml`
HLTXML=`ls ${AtlasArea}/Trigger/TriggerCommon/TriggerMenuXML/data/HLTconfig_Physics_pp_v1_*.xml`

#Ntuples for rate calculation
EBNTP=/castor/cern.ch/atlas/atlascerngroupdisk/trig-rates/ntuples/data/data10_7TeV.166658.166466.166383.physics_EnhancedBias.daq.RAW_13-10-2010_MCppv1_AtlasCAFHLT-rel1-15.6.X.Y.Z_TMP001278
#Account for dead-time, etc...
DQFILE=$TestArea/Trigger/TrigCost/TrigCostRate/data/dq_166383.xml

#Enhanced bias XML file for rate calculation
EBXML=$TestArea/Trigger/TrigCost/TrigCostRate/data/enhanced_bias_run_166383.xml

usage() {
    echo "usage:"
    echo "${0} OUTDIR EMAIL"
    echo "   OUTDIR - The location where you wish the rate calculation to be output, the luminosity point will be appended to this string."
    echo "   EMAIL  - Notices will be sent to this address as each lumi point is completed."
    echo ""
    echo "Alternatively, you can set EMAIL as an environment variable."
    echo ""
}

echo "$0 $1 $2"

if [ "$1" == "" ]; then
    echo "No OUTDIR passed on the command line. You don't want to overwrite Emanuel's files, do you?"
    usage
    echo ${EXIT:?"Job failed"}
else
    OUTDIR=$1
fi



if [ "$2" == "" ]; then 
    echo "No EMAIL passed on the command line. You don't want to send emails to Emanuel after each of your jobs, do you?"
    if [ "$EMAIL" == "" ]; then
	usage
	echo ${EXIT:?"Job failed"}
    fi
else
	EMAIL=$2
fi

#Dump job information
 INFO="Output directory       = $OUTDIR \n"
INFO+="Lumi points            = $POINTS \n"
INFO+="Email notification to  = $EMAIL \n"
INFO+="Bandwidth file         = $ES_REF1 \n"
INFO+="Rates file             = $ES_REF2 \n"
INFO+="Input rate             = $INPUT_RATE \n"
INFO+="LVL1 Menu              = $L1XML \n"
INFO+="HLT Menu               = $HLTXML \n"
INFO+="Rate ntuples           = $EBNTP $EBNTP1 $EBNTP2 \n"
INFO+="Rate xml               = $EBXML \n"

#Environment Info
INFO+="TrigCostBase Version   = "`cmt show versions Trigger/TrigCost/TrigCostBase | head -n1`
INFO+="\n"
INFO+="TrigCostRate Version   = "`cmt show versions Trigger/TrigCost/TrigCostRate | head -n1`
INFO+="\n"
INFO+="TrigCostPython Version = "`cmt show versions Trigger/TrigCost/TrigCostPython | head -n1`
INFO+="\n"
INFO+="\n"
INFO+=`uname -a`
INFO+="\n"
INFO+=`date`
INFO+="\n"

echo -e "${INFO}"
echo -e "${INFO}" &> ${OUTDIR}log

#Prepare the prescale XMLs for each lumi point
for LUMI in $POINTS; do
    echo "Prepare xml for ${LUMI}"
    python fineTune_ES.py -l ${LUMI}000 ${BOOK} -b $ES_REF1 -r $ES_REF2 -i $INPUT_RATE &> xml_${LUMI}.log
    python $TestArea/Trigger/TriggerCommon/TriggerMenu/scripts/cnvXml_express.py \
        --lv1xml=$L1XML \
        --hltxml=$HLTXML \
        --ps_name=Physics_pp_v1_${LUMI}e30 \
        --ps_xml=output_xml_${LUMI}000.xml
done

#Calculate the rates for each lumi point
for LUMI in ${POINTS}; do
    echo "Calculate rates for ${LUMI}"

    cmd="$EBNTP $EBNTP1 $EBNTP2\
       -o ${OUTDIR}${LUMI} \
       --data \
       --do-bg \
       --bg-scaleXML=$EBXML \
       --predict-rates \
       --default-ps -1 \
       --default-pt -1 \
       --ignore='EF_mbRd0_eff_unpaired,EF_mbMbts_1_eff_unpaired,EF_mbRd1_eff,L1_RD1_FILLED' \
       --groupXML /afs/cern.ch/user/o/olya/www/menu2/GroupRatesDef.xml
       --do-cps \
       --dq-xml=${DQFILE} \
       --prescaleXML=output_xml_${LUMI}000.xml \
       --pred-lumi=${LUMI}e30 \
"

    echo $cmd
    $TestArea/Trigger/TrigCost/TrigCostRate/share/runRates.py $cmd &> rates_${LUMI}.log
    cp output_xml_${LUMI}000.xml ${OUTDIR}${LUMI}
    echo "Finished calculating rates for ${LUMI} in ${PWD} from ${HOSTNAME}" | mail -s "Done with ${LUMI}" ${EMAIL}

done

echo "done"
date
