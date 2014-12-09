# THIS SCRIPT CAN BE USED TO SUBMIT VARIOUS FTK GRID JOBS
# Suggestion: make a separate script and source grid.sh from there,
# then you can use all the functions defined here.

# try to determine username automatically
username=`if [ -f $PATHENA_GRID_SETUP_SH ]; then . $PATHENA_GRID_SETUP_SH &>/dev/null; fi; voms-proxy-info -all | grep --color=never nickname | awk '{print $5}'`
official="user"
if [ -n "$username" ]; then
    echo "Setting: username=${username}"
else
    echo 'WARNING: cannot retrieve nickname attribute - forgot to run voms-proxy-init?'
    exit -1
fi;

site=ANALY_MWT2_SL6
#global_extra="-o IGNORE_SIGSEGV,strict_error_check,run_all_inputs"
global_extra="-o strict_error_check,run_all_inputs"
#uncomment the line below to force local disk staging, even on direct-access sites
global_extra="${global_extra} -o COPY_WRAPS_FROM_DCACHE,COPY_BANKS_FROM_DCACHE"
#uncomment the line below to print, but not actually submit, ftk.py commands
#global_extra="${global_extra} -o skip_tarball,skip_submit"

# standard input samples (feel free to add more)
MUON_INPUT="--inDS user09.AlbertoAnnovi.FtkV14_data_indepmuons_20091028_182458"
V16MUON_INPUT="--inDS user.JordanWebster.training.20110803_fulldetector_16.0.3_nominal_100k_indep_muons"
V18_IBL_MUON_INPUT="--inDS user.cmelachr.ibl100kIndepMuons_v2_20120628bz2"
MC11MUON_INPUT="--inDS user.cmelachr.FTKnoIBL.muIndepLauren.set3MWT2.20130219/"
MUON_DEGRADED="--inDS user.kapliy.FTKDeadChannelMuons.COMBINED.12_13_2010.v1"
PION_INPUT=" --inDS user10.AntonioBoveia.FTKtrainingmuons.set1MWT2.TrainingPions.20100715"
WH3_INPUT="--inDS user10.AntonKapliy.v14wh3e34ZEROREC.250evts.v1"
WH_INPUT="${WH3_INPUT}"  # compatibility
WH1_INPUT="--inDS user09.MonicaDunford.v14WHbb.set02_RECO1E34.rset10.20091114"
WH40PE_INPUT="--inDS user10.MelShochet.v14WHbb.set05_RECOPU41.rset1.IDonly.20100904.101102134730"
HTAUTAU_INPUT="--inDS mc10_14TeV.105334.HerwigVBFH120tautaulh.recon.TXT_FTKIP.e662_s1107_d459_r3729/"
WHLNUBB_46PU_INPUT="--inDS group.trig-daq.mc11_14TeV.105851.WH120uu_pythia.digit.FTK.e1808_s1321_d603_ftkoldwrapv2/"
#lost: HTAUTAU_INPUT_70PU="--inDS user.tompkins.mc11_14TeV.1093522.WH120lnubb_pythia.FTKoutIDonly.e940_s1321_d602.rel17051/"
#lost: TTBAR_INPUT="--inDS user.gvolpi.mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459/"
WH120LNUBB_INPUT="--inDS user.gvolpi.mc10_14TeV.109352.WH120lnubb_pythia.digit.FTK.e662_s1107_d459/"

TTBAR_46PU_INPUT="--inDS group.trig-daq.mc10_14TeV.105568.ttbar_Pythia.digit.RDO.e662_s1107_d459_FTKTXT_20130327/"
HTAUTAU_70PU_INPUT="--inDS group.trig-daq.mc11_14TeV.105334.HerwigVBFH120tautaulh.digit.RDO.e940_s1321_d602_ftkwrap/"
WH120LNUBB_70PU_INPUT="--inDS group.trig-daq.mc11_14TeV.1093522.WH120lnubb_pythia.FTKoutIDonly.e940_s1321_d602.rel1705/"

# standard base datasets (sectors, constants, slices, lookups)
baseDS8=user10.AntonKapliy.FtkV14_base8Lv4_20100218 #m=0,1,16,32 (for training or running)
baseDS8_4M=user.kapliy.FtkV14_base8L_2010_4M_v2 #m=4 (for running)
baseDS8_64M=user10.AntonKapliy.FtkV14_base8Lv4_64M_20100218 # m=64 (for running)   ## SOME LOST ON MWT2
baseDS4=user10.AntonKapliy.FtkV14_base4L11Lv2_20100311 #m=1,4,16; includes 11L con (for running)
baseDS4_unsplit=user10.AntonKapliy.FtkV14_base4L_unsplit.20100311.v1 #m=0 (for training)
baseDS11=user09.AlbertoAnnovi.FtkV14_base16Mv2_20091211_123726 #m=16   (for running)
baseDS11_unsplit=user09.AlbertoAnnovi.FtkV14_base_20091204_130347 #m=0 (for training)
# standard pattern datasets compatible with Technical Proposal configuration
bankDS11_50x64x144=user10.AntonKapliy.v14.raw_11L_50x64x144_4M_pgen.2010_04_09.v1 #benchmark 11L
bankDS11_50x64x36=user10.AntonKapliy.v14.raw_11L_50x64x36n200M_16M_pgen.2010_04_09.v1 #TP 11L TSP
bankDS8_50x16x144=user10.AntonKapliy.v14.raw_8L_50x16x144_16M_wpgen.2010_03_11.v1 #TP split
bankDS8_50x16x144_4M=user.kapliy.raw_8L_50x16x144_v14bankRESUBBED4000000_4NSubs.2010_09_29_16_41_35/  # 4M version of the above (up to 64M patts)
bankDS8_50x10x144=user.kapliy.raw_8L_50x10x144_v14pconst_64NSubsPerRegion.2010_06_03_10_10_12  #ss=10 8L bank (m=64)
bankDS4_20x156I225x25=user10.AntonKapliy.v14.raw_4L_20x156I225x25_1M_pgenonly.2010_03_11.v1 #TP split
bankDS4_20x156I225x25_PC=user10.ConstantinosMelachrinos.raw_4L_20x156I225x25_v14pconst_4NSubsPerRegion.2010_05_14_14_36_39 # TP using PC
bankDS4_15x150I150x20_PC=user10.ConstantinosMelachrinos.raw_4L_15x150I150x20_v14pconst_4NSubsPerRegion.2010_05_18_11_16_13 # PC only diff SS
bankDS4_11x90I150x18_PC=user10.ConstantinosMelachrinos.raw_4L_11x90I150x18_v14pconst_16NSubsPerRegion.2010_05_22_19_15_04
# updated 4L datasets (4L part is trained using truth tracks. For patterns, we also apply event filter on pixel layers only
baseDS4_no4Lfilt=user10.AntonKapliy.FtkV14_base4L11L_no4Lfilt_2010_05_12.v1 #m=1; includes 11L con - new sec lookup + 4L sectors
vbankDS4_20x156I300x25_no4Lfilt=user10.AntonKapliy.v14.raw_4L_20x156I300x25_1M_pgenonly_no4Lfilt.2010_05_12.v1
# bank for option A TSP simulation
bankDS7_24x20x36_48x40x36=user.gvolpi.raw_7L_24x20x36_merge30Mx509reg0barrel_16NSubs_DB48x40x36.2010_12_06_23_18_23/
bankDS7_12x10x36_24x20x36=user.gvolpi.raw_7L_12x10x36_merge10Mx1887reg0barrel_128NSubs_DB24x20x36.2010_12_12_03_47_38/


# helper function to give file ranges (-o fmin=BLA,fmax=BLA)
# DEPRECATED
function frange() {
    fmin=$1
    fmax=$2
    echo "-o fmin=${fmin},fmax=${fmax}"
}
# checks that a variable with a given name is defined
function check() { 
    for name in $@; do
	if [ ! -n "${!name}" ]; then
	    echo "ERROR: missing variable ${name}"
	    exit -1
	fi;
    done;
}
# skeleton: base job submission line
function skeleton() {
    check mode username site runstring m
    if [ -n "$inputs" ]; then extra="${extra} --inputs ${inputs}"; fi;
    if [ -n "$loops" ]; then extra="${extra} --loops ${loops}"; fi;
    if [ -n "$regions" ]; then extra="${extra} --regions ${regions}"; fi;
    if [ -n "$subregions" ]; then extra="${extra} --subregions ${subregions}"; fi;
    ./ftk.py -q grid --official ${official} --mode ${mode} --username ${username} --site ${site} -d ${runstring} -m${m} ${extra} ${global_extra}
#    ./ftk.py -q grid -- --mode ${mode} --username ${username} --site ${site} -d ${runstring} -m${m} ${extra} ${global_extra}
}
# skeleton: bank-related submission line
function skeleton_bankprod() {
    check n baseDS
    extra="-o runNum=1 -n${n} -o baseDS=${baseDS} -t ${baseDS} ${extra}"
    skeleton
}
# skeleton: FTK simulation submission line
function skeleton_ftksim() {
    check baseDS bankDS MAXPATTERNS INPUT
    extra="-t ${bankDS} -o baseDS=${baseDS} -o MAXPATTERNS=${MAXPATTERNS} ${INPUT} ${extra}"
    skeleton
}

# makes a TSP patterns-from-const bank
function makeBank () {
    mode=pconst
    skeleton_bankprod
}
# makes an 8L patterns-from-const bank bootstrapped from 11L
function makeBank8L () {
    check baseDS8
    echo ${runstring} | grep -q "11L" || check "YOU_HAVE_TO_SPECIFY_11L_RUNSTRING_FOR_8L_PCCONST_PRODUCTION"
    mode=pconst
    extra="-o PC_MODE=1,PC_NPLANES=8 -o baseDS8=${baseDS8} ${extra}"
    makeBank
}
# makes a 4L patterns-from-const bank bootstrapped from 11L
# WARNING: ss specified in runstring must be a matching 11L file.
#          actual 4L ss is given in PC_SSMAP variable!
function makeBank4L () {
    check baseDS8 baseDS4_unsplit
    echo ${runstring} | grep -q "11L" || check "YOU_HAVE_TO_SPECIFY_11L_RUNSTRING_FOR_4L_PCONST_PRODUCTION"
    mode=pconst
    extra="-o PC_MODE=2,PC_NPLANES=4 -o baseDS8=${baseDS8},baseDS4=${baseDS4_unsplit} ${extra}"
    makeBank
}
# merges patterns-from-const banks on the grid
function mergeBank () {
    check ftkDS
    mode=merge_pconst
    extra="-o ftkDS=${ftkDS} ${extra}"
    skeleton_bankprod
}
# merges patterns-from-const banks on the grid using root files
function mergeBankRoot () {
    check ftkDS
    mode=merge_pconst_root
    extra="-o ftkDS=${ftkDS} -o factor=${factor} ${extra}"
    extra="-o runNum=1 -n${n} ${extra}"
    skeleton
}
# reduces the size of a patterns-from-const bank on the grid
function reduceBank () {
   check bankDS MAXPATTERNS
   mode="reduce"
   extra="-o MAXPATTERNS=${MAXPATTERNS} -t ${bankDS} ${extra}"
   skeleton
}
# 
function makeDCBank () {
   check bankDS MAXPATTERNS
   mode="makedc"
   extra="-t ${bankDS} ${extra}"
   skeleton
}
function convertDBBank() {
    makeDCBank
}

# decreases the number of subregions used in a bank
function resubBank () {
   check bankDS MAXPATTERNS factor
   mode="resub"
   extra="-o MAXPATTERNS=${MAXPATTERNS},factor=${factor} -t ${bankDS} ${extra}"
   skeleton
}

# runs FTK with a TSP bank. If any argument if given, runs in effchain\ mode
function runTSP () {
    mode=ftk
    if [ -n "$1" ]; then mode=ftk_eff; fi
    skeleton_ftksim
}
# merges subregion results of a TSP run
function mergeTSP () {
    check ftkDS
    mode="merge_ftk"
    if [ -n "$1" ]; then mode="merge_ftk_eff"; fi
    extra="-o ftkDS=${ftkDS} ${extra}"
    skeleton_ftksim
}
# analysis of a TSP run
function analysisTSP () {
    check ftkDS
    mode="ana_ftk"
    extra="-o ftkDS=${ftkDS} ${extra}"
    skeleton_ftksim
}
# run ftk_explorer on the GRID
function exploreFTK () {
    check ftkDS
    mode="exp_ftk"
    extra="-o ftkDS=${ftkDS} ${extra}"
    skeleton_ftksim    
}
# runs effchain
function effchainTSP () {
    runTSP 1
}
# builds effcurve based on output of effchain
function effcurveTSP () {
    mergeTSP 1
}
# runs FTK with split arch banks. If any argument if given, runs in effchain mode
function runSPLIT_reg () {
    check MAXPATTERNS8 MAXPATTERNS4 runstring4 baseDS8 bankDS8 baseDS4 bankDS4
    mode=split_reg
    if [ -n "$1" ]; then mode=split_reg_eff; fi
    MAXPATTERNS=${MAXPATTERNS4}
    bankDS=${bankDS8}   # not actually used!
    baseDS=${baseDS8}   # not actually used!
    # note that there is no MAXPATTERNS4 flag in ftk.py - 4L maxpat HAS to be passed as ${MAXPATTERNS}
    extra="-o MAXPATTERNS8=${MAXPATTERNS8} -o runstring4=${runstring4} -o baseDS8=${baseDS8},bankdir8=${bankDS8} -o baseDS4=${baseDS4},bankdir4=${bankDS4} ${extra}"
    skeleton_ftksim
}
# merges subregions or runs effcurve for a SPLIT run
function mergeSPLIT_reg () {
    check ftkDS
    mode="merge_split_reg"
    if [ -n "$1" ]; then mode="merge_ftk_eff"; fi
    m=1   # since everything is already merged, split arch merging is effectively in one subregion
    # in SPLIT arch, all roads are already merged:
    extra="-o ftkDS=${ftkDS} -o DO_NOT_MERGE_ROADS,SKIP_MERGE_REGIONS,MERGED_ROADS=1 ${extra}"
    skeleton_ftksim
}
# runs effchain
function effchainSPLIT_reg () {
    runSPLIT_reg 1
}
# builds effcurve based on output of effchain
function effcurveSPLIT_reg () {
    mergeSPLIT_reg 1
}

# runs FTK with split arch banks in 8L subregions (maximally parallel, but gives wrong road dataflow)
function runSPLIT () {
    # TODO - work in progress!
    ./ftk.py -s5 -m16 -d raw_8L_50x16x144_wh3e34ZEROREC -r /share/ftkdata1/antonk/testingNEW -q pbs -n2 -o SS_OFFSET=0.0,CHI2_DOF=4,BARREL_ONLY=0,sp_base8=${base8},sp_ss8L=${ss8L},sp_nsubs8=${nsubs8},sp_maxpat8=${maxpat8},sp_base4=${base4},sp_ss4L=${ss4L},sp_nsubs4=${nsubs4},sp_maxpat4=${maxpat4} -a newsplit
    check MAXPATTERNS8 MAXPATTERNS4 runstring4 baseDS8 bankDS8 baseDS4 bankDS4
    mode=split
    if [ -n "$1" ]; then mode=split_eff; fi
    MAXPATTERNS=${MAXPATTERNS4}
    bankDS=${bankDS8}   # not actually used!
    m=1                 # this -m is only used to compute # of jobs to submit.
                        # actual m for 8L and 4L should be included in the runstring
    # note that there is no MAXPATTERNS4 flag in ftk.py - 4L maxpat HAS to be passed as ${MAXPATTERNS}
    extra="-o sp_maxpat8=${MAXPATTERNS8},sp_maxpat4=${MAXPATTERNS4} -o runstring4=${runstring4} -o baseDS8=${baseDS8},bankdir8=${bankDS8} -o baseDS4=${baseDS4},bankdir4=${bankDS4} ${extra}"
    skeleton_ftksim
}


################################ BANKS ########################################
#### 8L 50x14 bank production (region 0 only):
unset extra loops regions subregions inputs
runstring=raw_11L_50x14x144_v14pconst   # if making PC_MODE 8L/4L banks, use 11L here
m=32                                    # number of subregions
baseDS=${baseDS11_unsplit}              # dataset with sectors/constants
n=50000000                              # number of iterations per per-region loop
loops="0..100"                          # number of per-region submission loops
regions="0..7"                          # which regions to generate
extra=""
#makeBank
#makeBank8L
#extra="-o PC_SS4L=20x156I225x25"       # only for 4L split production
#makeBank4L

#### 8L 50x14 - merging subruns to create final patterns
ftkDS=
runstring=raw_8L_50x14x144_v14pconst    # if making PC_MODE=1 8L banks, use 8L here
unset loops
extra="-o DQ2_FORCE_DCAP"               # add this if mode=pconst output is on the same SITE
#mergeBank

#### Reducing bank size
unset extra loops regions subregions inputs
bankDS=${bankDS8_50x16x144}
m=16
MAXPATTERNS=4000000
runstring=raw_8L_50x16x144_v14bankREDUCED${MAXPATTERNS}
regions="0..7"
subregions="0..`expr ${m} - 1`"
extra=""
#reduceBank

#### Converting TSP patt.bz2 bank into sqlite patt.db format
unset extra loops regions subregions inputs
bankDS=${bankDS8_50x16x144}
m=16
runstring=raw_8L_50x16x144_v14bankDB
regions="0..7"
subregions="0..`expr ${m} - 1`"
extra="-o amlabel=100x32x144"
#convertDBBank

#### Decreasing the number of subregions used in a bank
unset extra loops regions subregions inputs
bankDS=${bankDS8_50x16x144}
m=16                     # nsubregions in final bank
MAXPATTERNS=4000000      # per subregion in the original (many-nsubs) patterns
factor=4                 # nsubregions_orig / nsubregions_final
runstring=raw_8L_50x16x144_v14bankREDUCED${MAXPATTERNS}
regions="0..7"
subregions="0..`expr ${m} - 1`"
extra=""
#resubBank

############################### TSP run ######################################
#### Regular (mode1) run
unset extra loops regions subregions inputs
INPUT="-n 5 --nFilesPerJob 5 --nJobs 1 ${WH3_INPUT}" # input dataset, file range to process, and nevents:
runstring=raw_8L_50x16x144_wh3e34TSP
m=16                                    # number of subregions
baseDS=${baseDS8}                       # dataset with sectors/constants
bankDS=${bankDS8_50x16x144}
MAXPATTERNS=4000000                     # per subregion!
extra=""
#runTSP
#### Merging subregions for regular (mode1) run
ftkDS=
extra="-o DO_NOT_MERGE_ROADS"           # uncomment to only merge tracks
#mergeTSP

############################ Effchain TSP run ################################
#### effchain run
unset extra loops regions subregions inputs
INPUT="-n 25000 --nFilesPerJob 1 --nJobs 1 ${MUON_INPUT}" # input dataset, file range to process, and nevents:
runstring=raw_8L_50x16x144_effcurveTSP
m=16                                        # number of subregions
baseDS=${baseDS8}                           # dataset with sectors/constants
bankDS=${bankDS8_50x16x144}
MAXPATTERNS=10000001                        # upper bound on patternPoints to try - per subregion!
extra=""
#effchainTSP
#### effcurve run
ftkDS=
extra=""
#effcurveTSP

############################### SPLIT run (per-region) ######################################
#### split arch run
unset extra loops regions subregions inputs
INPUT="-n 5 --nFilesPerJob 1 --nJobs 1 ${WH3_INPUT}"    # input dataset, file range to process, and nevents:
# specify 8L part:
m=16
runstring=raw_8L_50x16x144_wh3e34SPLIT_16M      # notice that first tag has to give nsubs, e.g: _16M
baseDS8=${baseDS8}
bankDS8=${bankDS8_50x16x144}
MAXPATTERNS8=4000000                       # per subregion!
# specify 4L part:
runstring4=raw_4L_20x156I225x25_wh3e34SPLIT_1M  # notice that first tag has to give nsubs, e.g: _16M
baseDS4=${baseDS4}
bankDS4=${bankDS4_20x156I225x25}
MAXPATTERNS4=25000000                      # per subregion!
extra=""
#runSPLIT_reg
#### Merging subregions for 8L tracks in the split arch run
unset loops regions subregions
ftkDS=
runstring=raw_8L_50x16x144_wh3e34SPLIT_16M
MAXPATTERNS=${MAXPATTERNS8}
extra=""
#mergeSPLIT_reg
#### Merging subregions for 11L(aka 4L) tracks in the split arch run
unset loops regions subregions
ftkDS=
runstring=raw_11L_20x156I225x25_wh3e34SPLIT_1M
MAXPATTERNS=${MAXPATTERNS4}
extra=""
#mergeSPLIT_reg

############################### Effchain SPLIT run ######################################
#### split arch run
unset extra loops regions subregions inputs
INPUT="-n 25000 --nFilesPerJob 1 --nJobs 1 ${MUON_INPUT}" # input dataset, file range to process, and nevents:
# specify 8L part
runstring=raw_8L_50x16x144_effcurveSPLIT_16M       # notice that first tag has to give nsubs, e.g: _16M
baseDS8=${baseDS8}
bankDS8=${bankDS8_50x16x144}
MAXPATTERNS8=4000000                        # this is NOT looped over in effchainSPLIT - per subregion!
# specify 4L part:
runstring4=raw_4L_20x156I225x25_effcurveSPLIT_1M   # notice that first tag has to give nsubs, e.g: _16M
baseDS4=${baseDS4}
bankDS4=${bankDS4_20x156I225x25}
MAXPATTERNS4=25000001                       # upper bound on 4L patternPoints to try - per subregion!
extra=""
#effchainSPLIT_reg
#### Building effcurve for 4L pattern points
unset loops regions subregions
ftkDS=
runstring=raw_11L_20x156I225x25_effcurveSPLIT_1M
MAXPATTERNS=${MAXPATTERNS4}
extra=""
#effcurveSPLIT_reg

################### Optiona TSP and DC simulation
unset extra loops regions subregions inputs
#INPUT="-n 5 --nFilesPerJob 5 --nJobs 1 ${WH3_INPUT}" # input dataset, file range to process, and nevents:
INPUT="-n 3000 --nFilesPerJob 50 ${WH40PE_INPUT}" # input dataset, file range to process, and nevents:
runstring=raw_7L_24x20x36_TEST # the SS string in the middle must describe the TSP
regions="0..0"
m=16                                    # number of subregions
baseDS=${baseDS11}                       # dataset with sectors/constants
bankDS=$bankDS7_24x20x36_48x40x36}
MAXPATTERNS=6250000                     # per subregion!
# USETSP_SIM=0 means use TSP as AM, legacy mode for the old simulation
# USETSP_SIM=1 means to run on the AM groupin N TSP patterns,
# USETSP_SIM=2 means AM w/ DC,
# USETSP_SIM=3 AM w/ TSP
extra="-o RFmode=roadfindertsp,amlabel=48x40x36,USETSP_SIM=2" 
# PS if USETSP_SIM=0 amlabel must describe the TSP and not the AM
#runTSP


#### merging ####
ftkDS=
extra="-o DO_NOT_MERGE_ROADS"           # uncomment to only merge tracks
#mergeTSP


#### analysis ###
ftkDS=
extra="-o ANA_IGNORE_ROADS=0,absd0cut=.2,absetacut=.1"
#analysisTSP

ftkDS=
extra=""
#exploreFTK
