#############################################
# Mechanism to get a unique number 
# in the output ds name of each submission
# No need to change this...
#############################################

a=0
while read line
do a=$(($a+1));
# echo $a;
done < "SubmitVersion.source"
# echo "Final line count is: $a";
a=`expr $a + 1`
echo "v$a" >> "SubmitVersion.source"

UNIQUEIDTOUSE="v$a"
# echo "Will append this to output name: "$UNIQUEIDTOUSE


#############################################
#                Settings
#
# Adapt to your neeeds
#############################################


JOBOPTIONSDIR="/afs/cern.ch/user/l/limbach/scratch0/PanTauTest/PhysicsAnalysis/PanTauAnalysis/share/" # do NOT forget the / at the end!
JOBOPTIONSFILE="PanTau_AOD-to-eflowStudyD3PD_TopOptions.py"
# EXCLUDEDSITES="ANALY_INFN-MILANO-ATLASC"
EXCLUDEDSITES=""


# ##########################################
#
# Set Maximum number of files to be used
#
# ##########################################
NMAXFILES="1000"
# NEVENTSPERJOB="5000"


#############################################
#                Automatic part
#
# 
#############################################

echo "Option 1 (Dataset to use) specified to be:" $1
DATASETTOUSE=$1
if [ "$DATASETTOUSE" = "" ]; then
    echo "INFO    No sample given!"
fi


echo "Option 2 (do local test)  specified to be:" $2
EXECUTEOPTION=$2
if [ "$EXECUTEOPTION" = "local" ]; then
    echo "INFO    Will do local test"
elif [ "$EXECUTEOPTION" = "grid" ]; then
    echo "INFO    Will submit the job to the grid using pathena"
else
    echo "INFO    No second argument provided => Display commands for local & pathena submission and show valid sample names."
    echo "INFO    ___________________________________________"
    echo "INFO    "
    echo "INFO    Command Line options:"
    echo "INFO       1. Option: Name of sample (required, will do nothing if not specified"
    echo "INFO       2. Option: ExecutionOption"
    echo "INFO                  local   -> Do not submit to grid but execute athena locally"
    echo "INFO                  grid    -> Submit to grid"
    echo "INFO    "
    echo "INFO    "
    echo "INFO       Allowed values for samples                 | Note"
    echo "INFO        -MC Samples:"
    echo "INFO          ZTauTau6M_Mark                          | 6.5 Million events in AOD with calib hits, latest eflowRec"
    echo "INFO                                                  | Used to produce eflowStudy D3PDs for Peter"
    echo "INFO          ZTauTau_Mark_16Dec                      | 10k events in AOD with config from 16Dec"
    echo "INFO          ZTauTau_Mark_17Dec_k1_0                 | 10k events in AOD with config from 17Dec, k_1 set to 0"
    echo "INFO          ZTauTau_Mark_1Feb_WithoutSplitShower    | 10k events in AOD without split shower algorithm"
    echo "INFO    ___________________________________________"
fi


VALIDSAMPLE="true"




if [ "$DATASETTOUSE" == "ZTauTau6M_Mark" ]; then
     INPUTDATA="user.markhod.20121102155340.2340/"
     OUTPUTDATA="user.cLimbach.ZTauTau_6M_AODfromMark_eflowStudy_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZTauTau_Mark_16Dec" ]; then
    INPUTDATA="user.limbach.eflowStudy_16Dec_withSpitShower_AOD"
#     OUTPUTDATA="user.limbach.eflowStudy_16Dec_withSpitShower_AOD_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZTauTau_Mark_17Dec_k1_0" ]; then
    INPUTDATA="user.limbach.eflowStudy_17Dec_k1_0_AODs3"
    OUTPUTDATA="user.limbach.eflowStudy_4Feb_k1_0_AODs3_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZTauTau_Mark_1Feb_WithoutSplitShower" ]; then
    INPUTDATA="user.limbach.1Feb_WithoutSplitShower_AOD"
    OUTPUTDATA="user.limbach.eflowStudy_1Feb_WithoutSplitShower_$UNIQUEIDTOUSE.D3PD.root"
    
    
    
else
    echo "ERROR   The sample $DATASETTOUSE is not valid."
    VALIDSAMPLE="false"
fi



if [ "$VALIDSAMPLE" = "true" ]; then

    #############################################
    #                Submission
    #
    #############################################
    
    
    
    #Get command line options correct:
    
    JOBOPTIONS=" "$JOBOPTIONSDIR$JOBOPTIONSFILE
    GBPERJOB=" --nGBPerJob=MAX"
    SITE=" --site="$THESITE
    TMPDIR=" --tmpDir=/tmp"
    FILESPERJOB=" --nFilesPerJob=1"
#     NEVENTS=" --nEventsPerJob="$NEVENTSPERJOB
    EXCLUDED=" --excludedSite="$EXCLUDEDSITES
    USECLOUD=" --cloud="$CLOUD
    INDS=" --inDS="$INPUTDATA
    OUTDS=" --outDS="$OUTPUTDATA
    
    #get number of files correct
    MAXFILES=""
    if [ "$NMAXFILES" = "-1" ]; then
        MAXFILES=""
    else
        MAXFILES=" --nFiles="$NMAXFILES
    fi
    
    #get destination site correct
    DESTSE=""
    if [ "$DESTINATIONSITE" = "" ]; then
        DESTSE=""
    else
        DESTSE=" --destSE="$DESTINATIONSITE
    fi
    
    
    
    echo "INFO    ___________________________________________"
    echo "INFO    "
    echo "INFO    Submitting a job to panda..."
    echo "INFO    Options:"
    echo "INFO          $JOBOPTIONS"
    echo "INFO          $GBPERJOB"
    echo "INFO          $SITE"
    echo "INFO          $TMPDIR"
    echo "INFO          $FILESPERJOB"
    echo "INFO          $EXCLUDED"
    echo "INFO          $USECLOUD"
    echo "INFO          $MAXFILES"
    echo "INFO          $DESTSE"
    echo "INFO          $INDS"
    echo "INFO          $OUTDS"
    echo "INFO    ___________________________________________"
    echo "INFO    "
    
    
    TRAFOCOMMAND_LOCAL=""
    TRAFOCOMMAND_GRID=""
    
    TRAFOCOMMAND_LOCAL="athena.py "$JOBOPTIONS
    
    # ===> submit command
    # ${EXCLUDED}
    USESHORTLIVED=" --useShortLivedReplicas"
    TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${TMPDIR}${FILESPERJOB}${MAXFILES}$USESHORTLIVED${INDS}${OUTDS}${DESTSE}${JOBOPTIONS}

    
    if [ "$EXECUTEOPTION" = "local" ]; then
        echo "INFO    Will run athena locally to test..."
        echo "INFO    Command that is executed:"
        echo "INFO       $TRAFOCOMMAND_LOCAL"
        $TRAFOCOMMAND_LOCAL
    
        
    elif [ "$EXECUTEOPTION" = "grid" ]; then 
        echo "INFO    Will submit pathena job"
        echo "INFO    Command that is executed:"
        echo "INFO        $TRAFOCOMMAND_GRID"
        $TRAFOCOMMAND_GRID
    
        
    else
        echo "INFO    Not submitting anything, only displaying commands:"
        echo "INFO    "
        echo "INFO    Local athena command:"
        echo "INFO        $TRAFOCOMMAND_LOCAL"
        echo "INFO    "
        echo "INFO    GRID command for pathena:"
        echo "INFO        $TRAFOCOMMAND_GRID"
        echo "INFO    "
    
    fi
    
else 
    echo "ERROR   No valid sample given!"
fi


echo "INFO    End of script."

# end of file

