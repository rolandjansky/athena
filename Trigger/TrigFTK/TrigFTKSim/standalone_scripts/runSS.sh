# example commands to run superstrip-shifted version of FTK simulation

ROADWARRIOR=2
n=100
bankdir0=/share/ftkdata/antonk/bases/hugeBank
bankdir5=/share/ftkdata/antonk/bases/hugeBankSS
bankdir=${bankdir0}
HW_NDIFF11=3
EXTRA_OPTS=""
MAXPATTERNS=7000000
SS_OFFSET=0.0
SSCONFIG=50x64x144
m=4
patternPoints="100000 500000 1000000 1500000 2000000 3000000 5000000 7500000 10000000"

# simple fully-customized run
function run_simple () {
    set -x
    ./ftk.py -n${n} -m${m} -s30 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR} -q pbs ${EXTRA_OPTS} -d raw_11L_${SSCONFIG}_${sample} -t ${bankdir} -o SS_OFFSET=${SS_OFFSET},MAXPATTERNS=${MAXPATTERNS} -a ${action}
    set +x
}
# single job to run ss-shifted architecture (slower)
function run_ss_all () {
    set -x
    ./ftk.py -n${n} -m${m} -s30 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} ${EXTRA_OPTS} -q pbs -a [-d=raw_11L_${SSCONFIG}_${sample}_group0:-t=${bankdir0}:SS_OFFSET=0.0],fullchain,STOP,anaone,STOP,plot_1,STOP,[-d=raw_11L_${SSCONFIG}_${sample}_group1:-t=${bankdir5}:SS_OFFSET=0.5],fullchain,STOP,anaone,STOP,plot_1,STOP,[-d=raw_11L_${SSCONFIG}_${sample}_group0:SHIFTROADS=raw_11L_${SSCONFIG}_${sample}_group1:FILTERED_ROADS:-t=${bankdir0}:SS_OFFSET=0.0],hitfilter,STOP,trackfitter,STOP,trackmerger,STOP,anaone,STOP,plot_1
    set +x
}
# SS-shifted run functions (faster, but must be manually submitted
function run_group () {
    set -x
    ./ftk.py -n${n} -m${m} -s50 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group0:-t=${bankdir0}:SS_OFFSET=0.0],fullchain,STOP,anaone,STOP,plot_1
    ./ftk.py -n${n} -m${m} -s50 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group1:-t=${bankdir5}:SS_OFFSET=0.5],roadfinder,STOP,roadmerger
    set +x
}
function run_filter () {
    set -x
    ./ftk.py -n${n} -m${m} -s30 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group0:-t=${bankdir0}:SS_OFFSET=0.0:SHIFTROADS=raw_11L_${SSCONFIG}_${sample}_group1:FILTERED_ROADS],hitfilter,STOP,trackfitter,STOP,trackmerger,STOP,anaone,STOP,plot_1
    set +x
}
# automated efficiency running (only default architecture)
function run_simple_eff () {
    set -x
    ./ftk.py -n${n} -m${m} -s30 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR} -q pbs ${EXTRA_OPTS} -d raw_11L_${SSCONFIG}_${sample} -t ${bankdir} -o SS_OFFSET=${SS_OFFSET} -a effchain6
    set +x
}

# manual efficiency in ss-shifted architecture
function run_eff_group () {
    samplebp=${sample}
    MAXPATTERNSbp=${MAXPATTERNS}
    for p in $patternPoints; do
	MAXPATTERNS=${p}
	bankdir=${bankdir0}
	sample=${samplebp}_group0_np${p}
	SS_OFFSET=0.0
	action=fullchain6
	run_simple
	bankdir=${bankdir5}
	sample=${samplebp}_group1_np${p}
	SS_OFFSET=0.5
	action=roadfinder6,STOP,roadmerger6
	run_simple
    done
    sample=${samplebp}
    MAXPATTERNS=${MAXPATTERNSbp}
}
function run_eff_filter () {
    samplebp=${sample}
    MAXPATTERNSbp=${MAXPATTERNS}
    for p in $patternPoints; do
	sample=${samplebp}
	MAXPATTERNS=${p}
	bankdir=${bankdir0}
	SS_OFFSET=0.0
	set -x
	./ftk.py -n${n} -m${m} -s30 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group0_np${p}:-t=${bankdir0}:SS_OFFSET=0.0:SHIFTROADS=raw_11L_${SSCONFIG}_${sample}_group1_np${p}:FILTERED_ROADS],hitfilter6,STOP,trackfitter6
	set +x
    done
    sample=${samplebp}
    MAXPATTERNS=${MAXPATTERNSbp}
}

############################################################################
# actual submissions:

function aug20_turnon () {
    outdir=/share/ftkdata/antonk/aug20/effcurve
    n=10000
    EXTRA_OPTS="-o HF_MAX_MATCHES=50"
    sample=effmuons
    run_eff_group
    sample=effpions
    run_eff_group
}
function aug20_turnonTODO () {
    outdir=/share/ftkdata/antonk/aug20/effcurve
    n=10000
    EXTRA_OPTS="-o HF_MAX_MATCHES=50"
    sample=effmuons
    run_eff_filter
    sample=effpions
    run_eff_filter
}

#############################################
# TMP: tracking only
function run_group_tracking () {
    set -x
    ./ftk.py -n${n} -m${m} -s50 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group0:-t=${bankdir0}:SS_OFFSET=0.0],trackfitter,STOP,trackmerger,STOP,anaone,STOP,plot_1
    set +x
}
function run_filter_tracking () {
    set -x
    ./ftk.py -n${n} -m${m} -s30 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group0:-t=${bankdir0}:SS_OFFSET=0.0:SHIFTROADS=raw_11L_${SSCONFIG}_${sample}_group1:FILTERED_ROADS],trackfitter,STOP,trackmerger,STOP,anaone,STOP,plot_1
    set +x
}
#############################################
# TMP

function aug20_whzero2 () {
    # runs whuu and whbb for Antonio using SS-shifted scheme
    outdir=/share/ftkdata/antonk/aug20/whzero2
    n=30000
    EXTRA_OPTS="-o HF_MAX_MATCHES=50,HW_NDIFF=${HW_NDIFF11}"
    sample=whbbzero
    run_group
    sample=whuuzero
    run_group
}
function aug20_whzero2TODO () {
    # runs whuu and whbb for Antonio using SS-shifted scheme
    outdir=/share/ftkdata/antonk/aug20/whzero2
    n=30000
    EXTRA_OPTS="-o HF_MAX_MATCHES=50,HW_NDIFF=${HW_NDIFF11}"
    sample=whbbzero
    run_filter
    sample=whuuzero
    run_filter
    # + do a (symlinked) version where Joe's case is added in!
}

function aug20_wh34 () {
    for num in {0..1}; do #39
	outdir=/share/ftkdata/antonk/aug20/wh34
	n=25
	EXTRA_OPTS="-o HF_MAX_MATCHES=50,HW_NDIFF=${HW_NDIFF11}"	
	#sample=WH34
	sample=WH34I${num}
	run_group
    done
}
function aug20_wh34TODO () {
    for num in {0..1}; do 
	outdir=/share/ftkdata/antonk/aug20/wh34
	n=25
	EXTRA_OPTS="-o HF_MAX_MATCHES=50,HW_NDIFF=${HW_NDIFF11}"
	#sample=WH34
	sample=WH34I${num}
	run_filter
    done
}
function wh34TODO__part2 () {
    # symlinked 5,10,20,30 maxmatches
    for mp in {5,10,20,30}; do
	EXTRA_OPTS="-o HF_MAX_MATCHES=${mp}"
	sample="WH34_mp${mp}"
	run_filter
    done;
    #symlinked Joe
    EXTRA_OPTS="-o HF_MAX_MATCHES=50,ONE_PER_ROAD=1"
    sample="WH34_joe"
    run_filter
    # and, joe's thing without filt
    EXTRA_OPTS="-o HF_MAX_MATCHES=50,ONE_PER_ROAD=1"
    sample="WH34"
    ./ftk.py -n${n} -m${m} -s50 -r ${outdir} -o BARREL_ONLY,ROADWARRIOR=${ROADWARRIOR},MAXPATTERNS=${MAXPATTERNS} -q pbs ${EXTRA_OPTS} -a [-d=raw_11L_${SSCONFIG}_${sample}_group0:-t=${bankdir0}:SS_OFFSET=0.0],trackfitter,STOP,trackmerger,anaone,STOP,plot_1
}

#aug20_whzero2
#aug20_whzero2TODO

#aug20_wh34
#aug20_wh34TODO
