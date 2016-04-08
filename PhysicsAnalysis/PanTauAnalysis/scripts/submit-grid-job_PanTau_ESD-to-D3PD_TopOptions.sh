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


JOBOPTIONSDIR="/afs/cern.ch/work/l/limbach/private/testarea/WorkAreaD3PD/PhysicsAnalysis/PanTauAnalysis/share/" # do NOT forget the / at the end!
#JOBOPTIONSFILE="PanTau_ESD-to-D3PD_TopOptions.py"
JOBOPTIONSFILE="TopOptions_NewPanTau.py"
EXCLUDEDSITES="SARA"
#SARA,ANALY_IFIC,ANALY_LAPP"

CLOUD="DE"
THESITE="ANALY_CERN_XROOTD,ANALY_DESY-ZN,ANALY_DESY-HH,ANALY_ARC,ANALY_AGLT2,ANALY_RHUL,ANALY_INFN-FRASCATI,ANALY_RALPP"
NMAXFILES="-1"


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
    echo "INFO       Allowed values for samples             | Note"
    echo "INFO    "
    echo "INFO        -Data"
    echo "INFO          Data11_PeriodM8_Muons_DESD_ZMUMU    | "
    echo "INFO          Data11_PeriodM10_Muons_DESD_ZMUMU   | "
    echo "INFO          Data11_PeriodX_Muons_DESD_ZMUMU     | X = D, E, F, G, H, I, J, K, L, M"
    echo "INFO                                              | DESD_ZMUMU selected muons from 2011"
    echo "INFO          "
    echo "INFO        -MC "
    echo "INFO          CellBasedTestSample                 | ESD for Corfu production"
    echo "INFO          CellBasedTestSample2                 | ESD for Corfu production"
    echo "INFO          CellBasedEtBiasFixA                 | ESD with neutral Et bias fix"
    echo "INFO          CellBasedEtBiasFixB                 | ESD with neutral Et bias fix"
    echo "INFO          CellBasedOctober"
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


if [ "$DATASETTOUSE" == "Data11_PeriodM10_Muons_DESD_ZMUMU" ]; then               
    INPUTDATA="data11_7TeV.periodM10.physics_Muons.PhysCont.DESD_ZMUMU.t0pro09_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodM10_physics_Muons_DESD_ZMUMU_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodM8_Muons_DESD_ZMUMU" ]; then               
    INPUTDATA="data11_7TeV.periodM8.physics_Muons.PhysCont.DESD_ZMUMU.t0pro09_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodM8_physics_Muons_DESD_ZMUMU_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"


elif [ "$DATASETTOUSE" == "Data11_PeriodD_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodD.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodD_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodE_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodE.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodE_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodF_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodF.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodF_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodG_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodG.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodG_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodH_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodH.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodH_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodI_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodI.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodI_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodJ_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodJ.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodJ_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodK_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodK.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodK_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodL_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodL.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodL_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data11_PeriodM_Muons_DESD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodM.physics_Muons.PhysCont.DESD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodM_physics_Muons_DESD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"


elif [ "$DATASETTOUSE" == "CellBasedTestSample" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid778651_00.v00-00/"
    OUTPUTDATA="user.limbach.TauPi0RecESD2ESD_147818_Pythia8_AU2CTEQ6L1_Ztautau_tid778651_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "CellBasedTestSample2" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999072_00.v00-01/"
    OUTPUTDATA="user.limbach.TauPi0RecESD2ESD_147818_Pythia8_AU2CTEQ6L1_Ztautau_tid00999072_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "CellBasedEtBiasFixA" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESDforD3PD_mp.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999076_00.v03-00/"
    OUTPUTDATA="user.limbach.TauPi0RecESD2ESDforD3PD_mp.147818.Pythia8_AU2CTEQ6L1_Ztautau_tid00999076_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "CellBasedEtBiasFixB" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESDforD3PD_mp.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid778651_00.v03-00/"
    OUTPUTDATA="user.limbach.TauPi0RecESD2ESDforD3PD_mp.147818.Pythia8_AU2CTEQ6L1_Ztautau_tid778651_$UNIQUEIDTOUSE.D3PD.root"
    
    
elif [ "$DATASETTOUSE" == "CellBasedSeptember" ]; then
    INPUTDATA="user.bwinter.TauPi0RecESD2ESDforD3PD.147818.Pythia8_AU2CTEQ6L1_Ztautau.digit.RDO.e1176_s1479_s1470_d700.v02-00/"
    OUTPUTDATA="user.limbach.September_TauPi0RecESD2ESDforD3PD.147818.Pythia8_AU2CTEQ6L1_Ztautau_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "CellBasedOctober" ]; then
    INPUTDATA="group.perf-tau.TauPi0Rec_D3PD.147818.Pythia8_AU2CTEQ6L1_Ztautau.recon.ESD.e1176_s1479_s1470_r3553_tid00999074_00.v05-01/"
    OUTPUTDATA="user.limbach.October_TauPi0RecESD2ESDforD3PD.147818.Pythia8_AU2CTEQ6L1_Ztautau_$UNIQUEIDTOUSE.D3PD.root"
    
    
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
#     FILESPERJOB=" --nFilesPerJob=1"
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
    
    # ===> cloud and site requirements
    # TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${SITE}${TMPDIR}${FILESPERJOB}${EXCLUDED}${USECLOUD}${MAXFILES}${INDS}${OUTDS}${JOBOPTIONS}
    
    # ===> cloud requirement
    # TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${TMPDIR}${FILESPERJOB}${EXCLUDED}${USECLOUD}${MAXFILES}${INDS}${OUTDS}${JOBOPTIONS}
    
    # ===> site exclusion
    TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${TMPDIR}${FILESPERJOB}${EXCLUDED}${MAXFILES}${INDS}${OUTDS}${DESTSE}${JOBOPTIONS}
    
    # ===> no location exclusion
    # TRAFOCOMMAND_GRID="pathena"${GBPERJOB}${TMPDIR}${FILESPERJOB}${MAXFILES}${INDS}${OUTDS}${JOBOPTIONS}
    
    
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

