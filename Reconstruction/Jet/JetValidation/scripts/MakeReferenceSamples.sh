#!/bin/bash



#Helper pseudo-booleans for my sanity
BOOLT="true"
BOOLF="false"


# Useful values
startDir=`pwd`
maxEvents=50
skipEvents=0
logFile="output.log"
resultsDir="$startDir"

#Geometry/conditions tags:
#    Matched what was done for the official production of the sample at the time of this writing
#    geometryVersion=ATLAS-GEO-20-00-01
#    conditionsTagMC=OFLCOND-MC12-SDR-06
#    conditionsTagData=COMCOND-BLKPA-RUN1-01
#    http://www-f9.ijs.si/atlpy/atlprod/prodrequest/?q=mc12_8TeV.117050.PowhegPythia_P2011C_ttbar
#    http://panda.cern.ch/server/pandamon/query/?reqid=1162571&mode=showtask0
#    https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/CoolProdTags
specifyCondAndGeo="$BOOLF" # true to use the tags below, false to autoconfigure everything
geometryVersion="ATLAS-GEO-20-00-01"
conditionsTagMC="OFLCOND-MC12-SDR-06"
conditionsTagData="COMCOND-BLKPA-RUN1-01"
IsCOMMON="$BOOLT"

#NoRun="$BOOLT"
NoRun="$BOOLF"

# Default input files
#defaultMC="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/HITS/mc12_8TeV.CurrentRef.HITS.pool.root"
#defaultMC="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/MC/HITS/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.simul.HITS.e1728_s1581.039772.pool.root.1"
defaultMC="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/MC/RDO/mc12_8TeV.CurrentRef.RDO.pool.root"
defaultData="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/RAW/data12_8TeV.CurrentRef.RAW"
#defaultData="/afs/cern.ch/atlas/groups/JetEtmiss/ReferenceFiles/RTT/DATA/RAW/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1"





MakeDirAndCd () \
{
    if test $# -ne 1 ; then
        echo "ERROR MakeDirAndCd: invalid number of arguments"
        exit -1
    fi
    if [ -d $1 ] ; then
        if [ $NoRun = $BOOLT ] ; then 
            cd $1
            return 0
        fi
        echo "ERROR MakeDirAndCd: directory already exists"
        echo "Exiting to prevent overwriting files"
        exit -2
    fi

    mkdir -p $1
    cd $1
}

#PostRunChecksAndClean () \
#{
#    if test $# -ne 3 ; then
#        echo "ERROR PostRunChecksAndClean: Unexpected number of args, $#"
#        exit -1
#    elif [ ! -d "$1" ] ; then
#        echo "ERROR PostRunChecksAndClean: Arg1 is not a directory \"$1\""
#        exit -2
#    elif [ ! -f "$1/$2" ] ; then
#        echo "ERROR PostRunChecksAndClean: Arg2 is not a file in the specified dir, \"$1/$2\""
#        exit -3
#    elif [ ! -f "$1/$3" ] ; then
#        echo "ERROR PostRunChecksAndClean: Arg3 is not a file in the specified dir, \"$1/$3\""
#        exit -4
#    fi
#
#    # Check if the run was successful
#    if [ -z "`grep "ErrorCode=0 (OK)" "$1/$3"`" ] ; then
#        echo -e"ERROR PostRunChecksAndClean: Transform appears to have failed - exiting"
#        exit 1
#    fi
#
#    # Run was successful - save the log and output file, remove dir
#    mv "$1/$2" "$resultsDir/$2"
#    mv "$1/$3" "$resultsDir/${1##*/}_$3"
#    
#    # "Safely" remove the directory (ensure that it didn't have sub-dirs, and thus no malicious rm -rf / can occur)
#    safeRemove=$BOOLT
#    for aFile in "$1/*" ; do
#        if [ -d "$aFile" ] ; then
#            echo "WARNING PostRunChecksAndClean: Unexpected directory structure - not cleaning up directory just to be safe"
#            safeRemove=$BOOLF
#        fi
#    done
#    
#    if [[ $safeRemove = $BOOLT ]] ; then
#        rm -rf "$1"
#    fi
#
#}



RunStep () \
{

    # Check the arguments
    if test $# -ne 1 ; then
        echo "ERROR RunStep: Expected 1 argument, but got $#"
        exit -1
    elif [ ! -f "$1" ] && [ $NoRun = $BOOLF ] ; then
        echo "ERROR RunStep: Input file does not exist \"$1\""
        exit -2
    fi
    
    # Parse the arguments
    inputFile="${1##*/}"
    inputDir="${1%/*}"
    inputType=""
    outputType=""
    transform="Reco_trf.py"
    if [[ $IsCOMMON = $BOOLT ]] ; then
        preExec='from RecExConfig.RecFlags import rec;from JetValidation.RTTConfig import scheduleRTTJetTests;rec.UserExecs = ["scheduleRTTJetTests(useCOMMONrelease=True)"]'
    else
        preExec='from RecExConfig.RecFlags import rec;from JetValidation.RTTConfig import scheduleRTTJetTests;rec.UserExecs = ["scheduleRTTJetTests()"]'
    fi
    if [[ "$inputFile" = *HITS* ]] ; then
        inputType="HITS"
        inputDataType="Hits" #different capitalization
        outputType="RDO"
        transform="Digi_trf.py"
        preExec=""
    elif [[ "$inputFile" = *RDO* ]] ; then
        inputType="RDO"
        inputDataType="$inputType"
        outputType="ESD"
        preExec="'from RecExConfig.RecFlags import rec;rec.doTrigger=False'"
    elif   [[ "$inputFile" = *RAW* ]] ; then
        inputType="RAW"
        inputDataType="BS" #bytestream
        outputType="ESD"
        preExec="'from RecExConfig.RecFlags import rec;rec.doTrigger=False'"
    elif [[ "$inputFile" = *ESD* ]] ; then
        inputType="ESD"
        inputDataType="$inputType"
        outputType="AOD"
        preExec="'from RecExConfig.RecFlags import rec;rec.doTrigger=False;from JetValidation.RTTConfig import scheduleRTTJetTests;rec.UserExecs = [\"scheduleRTTJetTests()\"]'"

    #elif [[ "$inputFile" = *AOD* ]] ; then
    #    inputType="AOD"
    #    outputType=""
    #    transform="Reco_trf.py"
    else
        echo "ERROR RunStep: Input file does not appear to be a supported type (RAW, HITS, RDO, ESD)"
        echo -e "\tGot: $inputFile"
        exit -3
    fi
    
    echo "Starting $inputType to $outputType step"

    # Create output given input
    # Note that special treatment is needed for RAW (does not start as a pool.root file, but ends as one)
    outputFile="`echo "$inputFile" | sed s/${inputType}/${outputType}/g`"
    if [[ "$inputType" = RAW ]] ; then
        # Careful of some files ending with .# and some not, such as .1 --> .pool.root.1
        if [[ "$outputFile" = "${outputFile%.[0-9]}" ]] ; then
            outputFile="${outputFile}.pool.root"
        else
            tempVal="${outputFile##*.}"
            outputFile="${outputFile%.[0-9]}.pool.root.${tempVal}"
        fi
    fi

    # Now actually do everything
    runDir="${resultsDir}/${inputType}to${outputType}"
    MakeDirAndCd "${runDir}"
    if [[ $specifyCondAndGeo = $BOOLT ]] ; then
        fullComand="${transform} preExec=${preExec} input${inputDataType}File=${inputDir}/${inputFile} output${outputType}File=${outputFile} maxEvents=${maxEvents} skipEvents=${skipEvents} geometryVersion=$geometryVersion conditionsTag=$conditionsTag autoConfiguration=everything" 
    else
        fullComand="${transform} preExec=${preExec} input${inputDataType}File=${inputDir}/${inputFile} output${outputType}File=${outputFile} maxEvents=${maxEvents} skipEvents=${skipEvents} autoConfiguration=everything"
    fi

    echo $fullComand
    if [[ $NoRun = $BOOLT ]] ; then
        echo "ErrorCode=0 (OK)" > "$logFile"
    else
        eval $fullComand   > "$logFile" 2>&1
    fi
    cd $startDir
    

    # Check if the run was successful
    if [ -z "`grep "ErrorCode=0 (OK)" "${runDir}/${logFile}"`" ] ; then
        echo "ERROR RunStep: Transform appears to have failed - exiting"
        exit -4
    fi
    
    echo "Done ${inputType} to ${outputType} step"
    echo 

    export OUTPUTFILE="${runDir}/${outputFile}"
}



RunChain () \
{
    if test $# -ne 1 ; then
        echo "ERROR RunChain: Expected 1 argument, got $#"
        exit -1
    fi
    
    nextFile="$1"
    while [[ "${nextFile##*/}" = *HITS* ]] || [[ "${nextFile##*/}" = *RDO* ]] || [[ "${nextFile##*/}" = *RAW* ]] || [[ "${nextFile##*/}" = *ESD* ]] ; do
        RunStep "$nextFile"
        nextFile="$OUTPUTFILE"
    done
    
}





# Check arguments
if test $# -ne 1 && test $# -ne 2; then
    echo -e "USAGE: $0 <input file> (output dir)"
    echo -e "\tInput file can be the following:"
    echo -e "\t  1. "DATA", the default data input, \"$defaultData\""
    echo -e "\t  2. "MC", the default MC input, \"$defaultMC\""
    echo -e "\t  3. The full path to a desired input file (data or MC)"
    echo -e "\tOutput dir is where to store the resulting files"
    echo -e "\t  If not specified, uses current directory"
    exit 1
fi

# Check that athena is set up
if [[ -z "$TestArea" ]] ; then
    echo -e "ERROR: Athena does not appear to be set up"
    exit 2
fi

# Parse the arguments
inputPath=""
conditionsTag=""
if [[ "$1" = "data" ]] || [[ "$1" = "DATA" ]] || [[ "$1" = "Data" ]] ; then
    inputPath="$defaultData"
    conditionsTag="$conditionsTagData"
elif [[ "$1" = "mc" ]] || [[ "$1" = "MC" ]] ; then
    inputPath="$defaultMC"
    conditionsTag="$conditionsTagMC"
else
    inputPath="$1"
    if [[ "${inputPath##*/}" =~ mc ]] || [[ "${inputPath##*/}" = *mc12* ]] || [[ "${inputPath##*/}" = *mc11* ]] ; then
        conditionsTag="$conditionsTagMC"
    elif [[ "${inputPath##*/}" =~ data ]] || [[ "${inputPath##*/}" = *data12* ]] || [[ "${inputPath##*/}" = *data11* ]] ; then
        conditionsTag="$conditionsTagData"
    else
        echo -e "ERROR: Could not determine if input file was MC or data - required for the conditions tag"
        exit 10
    fi
fi

if test $# -eq 2 ; then
    resultsDir="$2"
    if [ ! -e $resultsDir ] ; then
        mkdir -p $resultsDir
    fi
fi

# Ensure that the input file exists
if [ ! -f "$inputPath" ] ; then
    echo "ERROR: Input path does not exist - \"$inputPath\""
    exit 3
fi

inputFile="${inputPath##*/}"
inputDir="${inputPath%/$inputFile}"




# Run the appropriate steps
if [[ "$inputFile" = *HITS* ]] || [[ "$inputFile" = *RDO* ]] || [[ "$inputFile" = *RAW* ]] || [[ "$inputFile" = *ESD* ]] ; then
    RunChain "$inputPath"
else
    echo -e "ERROR: Input file does not match the expected formats"
    echo -e "\tCould not find HITS, RAW, RDO, or ESD in the name"
    exit 4
fi
