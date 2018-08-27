#!/bin/bash

BOOLT="true"
BOOLF="false"


function ResTest \
{
    jetDefinition="AntiKt4EMTopo"
    MCtype="MC16"
    CalibArea=""
    configFile="JER/R4_GlobalReduction_JER.config"
    outFile="JER-smear.pdf"
    component="MassResUncWZ,-2"
    isData="true"
    options="prefix=JET_;path=\"/afs/cern.ch/work/s/sschramm/private/rel21/athena/Reconstruction/Jet/JetUncertainties/share/\";fixedEtaVals=0,0.1,0.45,1.0,1.55,2.15,2.85,3.35,4.0"
}

function TestRelJMR \
{
    jetDefinition="AntiKt10LCTopoTrimmedPtFrac5SmallR20"
    MCtype="MC16a"
    CalibArea="CalibArea-05"
    configFile="rel21/Moriond2018/R10_CaloMass_strong.config"
    outFile="JMR-smear.pdf"
    component="MassRes_Top,+1"
    isData="false"
    #options="prefix=JET_;path=\"/eos/atlas/atlascerngroupdisk/perf-jets/JetUncertainties/CalibArea-05\";fixedEtaVals=0,0.4,1.2,1.8"
    options="prefix=JET_;fixedEtaVals=0,0.4,1.2,1.8"
}



# Check arguments
if test $# -lt 1 ; then
    echo "USAGE: $0 <scenario to run> (options addition or overwrite)"
    exit 1
fi

firstArg=$BOOLT
#echo "$BOOLT"
for anArg in $@ ; do
    #echo "start loop"
    #echo "$firstArg"
    #echo "$BOOLT"
    #echo "$anArg"
    if [ "$firstArg" = "$BOOLT" ] ; then
        # Run the specified scenario
        #echo "running"
        #echo "$1"
        $1
        
        # Check that it looks semi-reasonable
        if [ -z "$outFile" ] ; then
            echo "Output file is not specified"
            echo "Check that you spelled the desired scenario correctly"
            exit 2
        fi
        
        # Move on to options
        firstArg=$BOOLF
        continue
    elif ! [ -z "$anArg" ] ; then
        # Overwrite option(s) if needed
        key=`echo $anArg | tr "=" " " | awk '{print $1}'`
        val=`echo $anArg | tr "=" " " | awk '{print $2}'`
        if [ -z "$key" ] || [ -z "$val" ] || [ "$key" = "$val" ] ; then
            echo "Bad option formatting: $anArg"
            exit 3
        fi
        if [[ "$options" = *$key* ]] ; then
            #Overwrite
            options=`echo "$options" | sed s/"${key}=[^;]*"/"${key}=${val}"/g`
        else
            # Extend
            options="${options};${key}=${val}"
        fi
    fi
done

echo "Running with options: $options"
JetUncertainties_testResolution "$outFile" "$jetDefinition" "$MCtype" "$configFile" "$component" "$isData" "$options"

