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


JOBOPTIONSDIR="/afs/cern.ch/user/l/limbach/scratch0/FinalSetupTest/PhysicsAnalysis/PanTauAnalysis/share/" # do NOT forget the / at the end!
JOBOPTIONSFILE="TopOptions_NewPanTau.py"
EXCLUDEDSITES="SARA,ANALY_IFIC,ANALY_FZK,ANALY_NIKHEF-ELPROD"
CLOUD="DE"
THESITE="ANALY_DESY-ZN,ANALY_DESY-HH,ANALY_ARC,ANALY_AGLT2,ANALY_RHUL,ANALY_INFN-FRASCATI,ANALY_RALPP"
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
    echo "INFO       Allowed values for samples   | Note"
    echo "INFO        -MC Samples:"
    echo "INFO          ZTauTauOff                | Official sample to be used for substructure studies"
    echo "INFO          ZPrime500Off              | Official sample to be used for substructure studies"
    echo "INFO          JX                        | X = 0, 1, 2, 3, 4, 5"
    echo "INFO          ZTauTau"
    echo "INFO          ZPrime250TauTau"
    echo "INFO          ZPrime750TauTau"
    echo "INFO          WTauNu"
    echo "INFO          ZMuMuJet"
    echo "INFO          ZMuMu"
    echo "INFO          ZJetsX                    | X = 0, 1, 2, 3, 4, 5 (AlpgenJimmy)"
    echo "INFO          ZMuMuNp1pt20Jimmy"
    echo "INFO          ZMuMuNp1pt20Pythia"
    echo "INFO          ZmumugammaSherpa"
    echo "INFO          WmunuNp1pt20Jimmy"
    echo "INFO          WmunuNp1pt20Pythia"
    echo "INFO          "
    echo "INFO    "
    echo "INFO        -Data"
    echo "INFO          Data191933                          | 2011 7TeV JetTauEtmiss"
    echo "INFO          Data191933Muons                     | 2011 7TeV Muons"
    echo "INFO          Data191190Muons                     | 2011 7TeV Muons"
    echo "INFO          Data11_PeriodM6_Muons               | 2011 7TeV Period M6, Muonstream"
    echo "INFO          Data11_PeriodM8_Muons               | 2011 7TeV Period M8, Muonstream"
    echo "INFO          Data11_PeriodM9_Muons               | 2011 7TeV Period M9, Muonstream"
    echo "INFO          Data11_PeriodM10_Muons              | 2011 7TeV Period M10, Muonstream"
    echo "INFO          Data11_PeriodM_Muons                | 2011 7TeV Period M, Muonstream"
    echo "INFO          Data11_PeriodX_Muons_DAOD_ZMUMU     | X = B, D, E, F, G, H, I, J, K, L, M"
    echo "INFO                                              | DAOD_ZMUMU selected muons from 2011"
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


if [ "$DATASETTOUSE" == "ZTauTauOff" ]; then               
    INPUTDATA="mc12_8TeV.147818.Pythia8_AU2CTEQ6L1_Ztautau.merge.AOD.e1176_s1479_s1470_r3553_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_147818_Pythia8_AU2CTEQ6L1_ZTauTau_e1176_s1479_s1470_r3553_r3549_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "ZPrime500Off" ]; then               
    INPUTDATA="mc12_8TeV.170202.Pythia8_AU2CTEQ6L1_Zprime500tautau.merge.AOD.e1176_s1479_s1470_r3553_r3549/"
    OUTPUTDATA="user.limbach.mc12_8TeV_170202_Pythia8_AU2CTEQ6L1_Zprime500tautau_e1176_s1479_s1470_r3553_r3549_$UNIQUEIDTOUSE.D3PD.root"



elif [ "$DATASETTOUSE" == "ZTauTau" ]; then               
    INPUTDATA="mc11_7TeV.106052.PythiaZtautau.merge.AOD.e825_s1349_s1300_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_106052_PythiaZTauTau_e825_s1349_s1300_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
 ### Attention! This has not the same reco tags as the other samples ###
elif [ "$DATASETTOUSE" == "ZPrime250TauTau" ]; then 
    INPUTDATA="mc11_7TeV.107381.Pythia_Zprime_tautau_SSM250.merge.AOD.e825_s1349_s1300_r3060_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_107381_Pythia_Zprime_tautau_SSM250_e825_s1349_s1300_r3060_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZPrime750TauTau" ]; then
    INPUTDATA="mc11_7TeV.107383.Pythia_Zprime_tautau_SSM750.merge.AOD.e825_s1349_s1300_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_107383_Pythia_Zprime_tautau_SSM750_e825_s1349_s1300_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
    
    
elif [ "$DATASETTOUSE" == "ZMuMuJet" ]; then
    INPUTDATA="mc11_7TeV.113285.PythiaZmumuJet_Ptcut5.merge.AOD.e1039_s1372_s1370_r3043_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_113285_Pythia_ZmumuJet_Ptcut5_e1039_s1372_s1370_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZMuMu" ]; then
    INPUTDATA="mc11_7TeV.106047.PythiaZmumu_no_filter.merge.AOD.e815_s1310_s1300_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_106047_PythiaZmumu_no_filter_e815_s1310_s1300_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZMuMuNp1pt20Jimmy" ]; then
    INPUTDATA="mc11_7TeV.107661.AlpgenJimmyZmumuNp1_pt20.merge.AOD.e835_s1299_s1300_r3157_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_107661_AlpgenJimmyZmumuNp1_pt20_e835_s1299_s1300_r3157_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZMuMuNp1pt20Pythia" ]; then
    INPUTDATA="mc11_7TeV.117661.AlpgenPythiaZmumuNp1_pt20.merge.AOD.e1051_s1372_s1370_r3043_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_117661_AlpgenPythiaZmumuNp1_pt20_e1051_s1372_s1370_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZmumugammaSherpa" ]; then
    INPUTDATA="mc11_7TeV.126012.Sherpa_Zmumugamma.merge.AOD.e931_s1310_s1300_r3043_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_126012_Sherpa_Zmumugamma_e931_s1310_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
    
elif [ "$DATASETTOUSE" == "WmunuNp1pt20Pythia" ]; then
    INPUTDATA="mc11_7TeV.117691.AlpgenPythiaWmunuNp1_pt20.merge.AOD.e1051_s1372_s1370_r3043_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_117691__e1051_s1372_s1370_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "WmunuNp1pt20Jimmy" ]; then
    INPUTDATA="mc11_7TeV.107691.AlpgenJimmyWmunuNp1_pt20.merge.AOD.e825_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_107691_AlpgenJimmyWmunuNp1_pt20_e825_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
    
    
elif [ "$DATASETTOUSE" == "ZJets0" ]; then
    INPUTDATA="mc11_7TeV.107660.AlpgenJimmyZmumuNp0_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.limbach.mc11_7TeV_107660_AlpgenJimmyZmumuNp0_pt20_e835_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZJets1" ]; then
    INPUTDATA="mc11_7TeV.107661.AlpgenJimmyZmumuNp1_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.limbach.mc11_7TeV_107661_AlpgenJimmyZmumuNp1_pt20_e835_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZJets2" ]; then
    INPUTDATA="mc11_7TeV.107662.AlpgenJimmyZmumuNp2_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.limbach.mc11_7TeV_107662_AlpgenJimmyZmumuNp2_pt20_e835_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZJets3" ]; then
    INPUTDATA="mc11_7TeV.107663.AlpgenJimmyZmumuNp3_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.limbach.mc11_7TeV_107663_AlpgenJimmyZmumuNp3_pt20_e835_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZJets4" ]; then
    INPUTDATA="mc11_7TeV.107664.AlpgenJimmyZmumuNp4_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.limbach.mc11_7TeV_107664_AlpgenJimmyZmumuNp4_pt20_e835_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "ZJets5" ]; then
    INPUTDATA="mc11_7TeV.107665.AlpgenJimmyZmumuNp5_pt20.merge.AOD.e835_s1299_s1300_r3043_r2993/"
    OUTPUTDATA="user.limbach.mc11_7TeV_107665_AlpgenJimmyZmumuNp5_pt20_e835_s1299_s1300_r3043_r2993_$UNIQUEIDTOUSE.D3PD.root"
    
    
    
    
    
    
    
 ### Attention! This has not the same reco tags as the other samples ###
elif [ "$DATASETTOUSE" == "WTauNu" ]; then 
    INPUTDATA="mc11_7TeV.107054.PythiaWtaunu_incl.merge.AOD.e825_s1349_s1300_r3069_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_107054_PythiaWtaunu_incl_e825_s1349_s1300_r3069_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J0" ]; then
    INPUTDATA="mc11_7TeV.105009.J0_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105009_J0_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J1" ]; then
    INPUTDATA="mc11_7TeV.105010.J1_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105010_J1_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J2" ]; then
    INPUTDATA="mc11_7TeV.105011.J2_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105011_J2_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J3" ]; then
    INPUTDATA="mc11_7TeV.105012.J3_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105012_J3_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J4" ]; then
    INPUTDATA="mc11_7TeV.105013.J4_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105013_J4_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J5" ]; then
    INPUTDATA="mc11_7TeV.105014.J5_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063/"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105014_J5_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "J6" ]; then
    INPUTDATA="mc11_7TeV.105015.J6_pythia_jetjet.merge.AOD.e815_s1273_s1274_r3082_r3063"
    OUTPUTDATA="user.cLimbach.mc11_7TeV_105015_J6_pythia_jetjet_e815_s1273_s1274_r3082_r3063_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "Data191933" ]; then
    INPUTDATA="data11_7TeV.00191933.physics_JetTauEtmiss.merge.AOD.r3047_r3053_p673/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_00191933_physics_JetTauEtmiss_r3047_r3053_p673_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "Data191933Muons" ]; then
    INPUTDATA="data11_7TeV.00191933.physics_Muons.merge.AOD.f415_m1025"
    OUTPUTDATA="user.cLimbach.data11_7TeV_00191933_physics_Muons_f415_m1025_$UNIQUEIDTOUSE.D3PD.root"
    
    
#    data11_7TeV.00191190.physics_Muons.merge.AOD.f413_m1019
# elif [ "$DATASETTOUSE" == "Data11_PeriodM5_Muons" ]; then
#     INPUTDATA="data11_7TeV.periodM5.physics_Muons.PhysCont.AOD.t0pro09_v01/"
#     OUTPUTDATA="user.cLimbach.data11_7TeV_periodM5_physics_Muons_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"

elif [ "$DATASETTOUSE" == "Data191190Muons" ]; then
    INPUTDATA="data11_7TeV.00191190.physics_Muons.merge.AOD.f413_m1019"
    OUTPUTDATA="user.cLimbach.data11_7TeV_00191190_physics_Muons_AOD_f413_m1019_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "Data11_PeriodM6_Muons" ]; then
    INPUTDATA="data11_7TeV.periodM6.physics_Muons.PhysCont.AOD.t0pro09_v01/"
    OUTPUTDATA="user.limbach.data11_7TeV_periodM6_physics_Muons_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"
    
#     there is no m7 on ami...

elif [ "$DATASETTOUSE" == "Data11_PeriodM8_Muons" ]; then
    INPUTDATA="data11_7TeV.periodM8.physics_Muons.PhysCont.AOD.t0pro09_v01/"
    OUTPUTDATA="user.limbach.data11_7TeV_periodM8_physics_Muons_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "Data11_PeriodM9_Muons" ]; then
    INPUTDATA="data11_7TeV.periodM9.physics_Muons.PhysCont.AOD.t0pro09_v01/"
    OUTPUTDATA="user.limbach.data11_7TeV_periodM9_physics_Muons_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"
    
    
elif [ "$DATASETTOUSE" == "Data11_PeriodM10_Muons" ]; then
    INPUTDATA="data11_7TeV.periodM10.physics_Muons.PhysCont.AOD.t0pro09_v01/"
    OUTPUTDATA="user.limbach.data11_7TeV_periodM10_physics_Muons_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"
    
elif [ "$DATASETTOUSE" == "Data11_PeriodM_Muons" ]; then
    INPUTDATA="data11_7TeV.periodM.physics_Muons.PhysCont.AOD.t0pro09_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodM_physics_Muons_t0pro09_v01_$UNIQUEIDTOUSE.D3PD.root"

### DAOD_ZMUMU



#14757
elif [ "$DATASETTOUSE" == "Data11_PeriodB_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodB.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodB_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  

# 94777
elif [ "$DATASETTOUSE" == "Data11_PeriodD_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodD.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodD_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 32189
elif [ "$DATASETTOUSE" == "Data11_PeriodE_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodE.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodE_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 79903
elif [ "$DATASETTOUSE" == "Data11_PeriodF_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodF.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodF_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 294244
elif [ "$DATASETTOUSE" == "Data11_PeriodG_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodG.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodG_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 146719
elif [ "$DATASETTOUSE" == "Data11_PeriodH_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodH.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodH_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 211659
elif [ "$DATASETTOUSE" == "Data11_PeriodI_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodI.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodI_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 123329
elif [ "$DATASETTOUSE" == "Data11_PeriodJ_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodJ.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodJ_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 348728 events
elif [ "$DATASETTOUSE" == "Data11_PeriodK_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodK.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodK_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 799540 events
elif [ "$DATASETTOUSE" == "Data11_PeriodL_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodL.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodL_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
# 588452 events
elif [ "$DATASETTOUSE" == "Data11_PeriodM_Muons_DAOD_ZMUMU" ]; then
    INPUTDATA="data11_7TeV.periodM.physics_Muons.PhysCont.DAOD_ZMUMU.pro10_v01/"
    OUTPUTDATA="user.cLimbach.data11_7TeV_periodM_physics_Muons_DAOD_ZMUMU_pro10_v01_$UNIQUEIDTOUSE.D3PD.root"
  
    
    
    
    
    
    
    
    
    
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
    GBPERJOB=" --nGBPerJob=10"
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

