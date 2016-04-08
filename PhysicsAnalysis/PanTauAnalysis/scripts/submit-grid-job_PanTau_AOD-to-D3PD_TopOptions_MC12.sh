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
JOBOPTIONSFILE="PanTau_ESD-to-D3PD_TopOptions.py"
EXCLUDEDSITES="SLAC,SARA,ANALY_IFIC,ANALY_FZK"
CLOUD="DE"
THESITE=""
NMAXFILES="20"


# DESTINATIONSITE="UNI-BONN_LOCALGROUPDISK" # or something similar



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
    echo "INFO       Allowed values for samples   | Note"
    echo "INFO        -MC Samples:"
    echo "INFO          ZTauTau"
    echo "INFO          ZPrime250TauTau"
    echo "INFO          ZPrime750TauTau"
    echo "INFO          WTauNu"
    echo "INFO          ZMuMu"
    echo "INFO          ZMuMuNp1pt20Jimmy"
    echo "INFO          -----------------"
    echo "INFO          ZTauTau6M_Mark            | 6.5 Million events in AOD with calib hits, latest eflowRec"
    echo "INFO                                    | Used to produce eflowStudy D3PDs for Peter"
    echo "INFO          ZTauTau_bwinter           | Official sample from benedict"
    echo "INFO    "
    echo "INFO        -Data"
    echo "INFO          Data201556Muons             | 2012 8TeV Muons"
    echo "INFO          "
    echo "INFO          "
    echo "INFO          "
    echo "INFO          "
    echo "INFO          "
    echo "INFO    ___________________________________________"
fi






INPUTDATA=""
OUTPUTDATA=""

#############################################
#                Input Samples
#
#############################################

VALIDSAMPLE="true"

if [ "$DATASETTOUSE" == "ZTauTau" ]; then               
    INPUTDATA="mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.merge.AOD.e1176_s1479_s1470_r3553_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_147818_Pythia8_AU2CTEQ6L1_Ztautau_e1176_s1479_s1470_r3553_r3549_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZPrime250TauTau" ]; then 
    INPUTDATA="mc12_8TeV.170201.Pythia8_AU2CTEQ6L1_Zprime250tautau.merge.AOD.e1176_s1479_s1470_r3553_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_170201_Pythia8_AU2CTEQ6L1_Zprime250tautau_e1176_s1479_s1470_r3553_r3549_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZPrime750TauTau" ]; then
    INPUTDATA="mc12_8TeV.170203.Pythia8_AU2CTEQ6L1_Zprime750tautau.merge.AOD.e1176_s1479_s1470_r3553_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_170203_Pythia8_AU2CTEQ6L1_Zprime750tautau_e1176_s1479_s1470_r3553_r3549_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "WTauNu" ]; then 
    INPUTDATA="mc12_8TeV.147812.Pythia8_AU2CTEQ6L1_Wtaunu.merge.AOD.e1176_s1479_s1470_r3553_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_147812_Pythia8_AU2CTEQ6L1_Wtaunu_e1176_s1479_s1470_r3553_r3549_$UNIQUEIDTOUSE.D3PD.root"
    
# ### ### Attention ### This has not the same pdfs and reco tags as the ones above! ### #
elif [ "$DATASETTOUSE" == "ZMuMu" ]; then
    INPUTDATA="mc12_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.merge.AOD.e1169_s1469_s1470_r3542_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_147807_PowhegPythia8_AU2CT10_Zmumu_e1169_s1469_s1470_r3542_r3549_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZMuMuNp1pt20Jimmy" ]; then
    INPUTDATA="mc12_8TeV.107661.AlpgenJimmy_AUET2CTEQ6L1_ZmumuNp1.merge.AOD.e1218_s1469_s1470_r3542_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_107661_AlpgenJimmy_AUET2CTEQ6L1_ZmumuNp1_e1218_s1469_s1470_r3542_r3549_$UNIQUEIDTOUSE.D3PD.root"
    
# ### ### Marks AOD with Pi0 Tag ### ### #
elif [ "$DATASETTOUSE" == "ZTauTau6M_Mark" ]; then
    INPUTDATA="user.mhodgkin.20121123181942.2368/"
    OUTPUTDATA="user.limbach.ZTauTau_6M_AODfromMark_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "ZTauTau_bwinter" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid778651_00.v00-00/"
    OUTPUTDATA="user.limbach.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid778651_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZTauTau_bwinter2" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_00.v00-01/"
    OUTPUTDATA="user.limbach.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_$UNIQUEIDTOUSE.D3PD.root"
    
    
    
# ### Data ### #
elif [ "$DATASETTOUSE" == "Data201556Muons" ]; then
    INPUTDATA="data12_8TeV.00201556.physics_Muons.merge.AOD.f437_m1126"
    OUTPUTDATA="user.limbach.data12_8TeV_00201556_physics_Muons_f437_m1126_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "Data12PeriodA" ]; then
    INPUTDATA="data12_8TeV.periodA.physics_Muons.PhysCont.AOD.t0pro13_v01/"
    OUTPUTDATA="user.limbach.data12_8TeV_periodA_physics_Muons_t0pro13_v01_$UNIQUEIDTOUSE.D3PD.root"
    
    

    

    
    
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
    
    USESHORTLIVED=" --useShortLivedReplicas"
#     TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${TMPDIR}${FILESPERJOB}${MAXFILES}${USESHORTLIVED}${SITE}${INDS}${OUTDS}${DESTSE}${JOBOPTIONS}
    TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${TMPDIR}${FILESPERJOB}${MAXFILES}${USESHORTLIVED}${INDS}${OUTDS}${DESTSE}${JOBOPTIONS}
    
    
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
