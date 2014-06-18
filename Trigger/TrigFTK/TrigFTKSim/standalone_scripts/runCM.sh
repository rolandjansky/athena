# example commands to run constantinos version of FTK simulation (8L SCT first, followed by 4L PIX+SCTtrk)

run_queue="pbs"
run_region=""    # single space runs over all 8 regions
EXTRA=''

# general settings
# these are propagated to grid scripts, too!
RW8=0     # disable, since TP assumes no RW
RW11=0
HW8=1
HW11=2
HW_NDIFF11=6
HW_NDIFF8=3
nsctmiss=1
npixmiss=1
nscttrkmiss=1
nsctmisspair=0
restrictsctplayer=1
restrictsctpmod=1
chi2_8L=4
chi2dofcut=4
requireb=0

# some bank defaults that work on Chicago tier3
# these are NOT propagated to the grid, since there the banks
# are specified on ftk.py command line
base8=/share/ftkdata/antonk/jan15base/
ss8L=50x16x144
nsubs8=16
maxpat8=4000000

base4=/share/ftkdata/antonk/jan15base/
ss4L=20x156I225x25
nsubs4=1
maxpat4=25000000

function run4common () {
    echo "[-t=${base4}:-m=${nsubs4}:-p=1:SCTTRK_MODE=1:ROADWARRIOR=0:REQUIRE_FIRST=${requireb}:MAX_MISSING_PLANES=${npixmiss}:MAX_MISSING_SCT_PAIRS=${nsctmisspair}:SCTTRK_MAX_MISSING_PLANES=${nscttrkmiss}:HW_NDIFF=${HW_NDIFF11}:HITWARRIOR=${HW11}:ROADWARRIOR=${RW11}:CHI2_DOF=${chi2dofcut}]"
}
function run4string () {
    ss=$1      # 4L ss configuration (eg, 20x156I225x25)
    maxpat=$2  # 4L # of patterns to load (eg, 23000000)
    r=$3       # which region to run. emptry string ('') runs all regions
    dep=$4     # dependency group id (eg, "[5]") used if running simultaneously with 8L
    # run_4 sequence (all regions)
    echo "[-d=raw_4L_${ss}_${dataset}_np${maxpat}:MAXPATTERNS=${maxpat}],${dep},roadfinder${r},STOP,roadmerger${r},STOP,makelinks,STOP,[-d=raw_11L_${ss}_${dataset}_np${maxpat}],trackfitter${r},STOP,trackmerger${r},STOP,ana${r}"
}
function run8common () {
    echo "[-t=${base8}:-m=${nsubs8}:HITWARRIOR=${HW8}:HW_NDIFF=${HW_NDIFF8}:SCTTRK_MODE=0:ROADWARRIOR=${RW8}:REQUIRE_FIRST=0:MAX_MISSING_PLANES=${nsctmiss}:MAX_MISSING_SCT_PAIRS=${nsctmisspair}:RESTRICT_SCT_PAIR_LAYER=${restrictsctplayer}:RESTRICT_SCT_PAIR_MODULE=${restrictsctpmod}:CHI2_DOF=${chi2_8L}]"
}
function run8string () {
    ss=$1      # 8L ss configuration (eg, 50x16x144)
    maxpat=$2  # 8L # of patterns to load (eg, 4000000)
    r=$3       # which region to run. emptry string ('') runs all regions
    echo "[-d=raw_8L_${ss}_${dataset}:MAXPATTERNS=${maxpat}],roadfinder${r},STOP,roadmerger${r},STOP,trackfitter${r},STOP,trackmerger${r},STOP,ana${r},STOP,[-3],makelists${r}"
}

function run_all () {
    # 8L + 4L + 4/11L (CM case)
    part8common=`run8common`
    part8run=`run8string ${ss8L} ${maxpat8} "${run_region}"`
    part4common=`run4common`
    part4run=`run4string ${ss4L} ${maxpat4} "${run_region}" "[5]"`
    ./ftk.py -s5 -r ${outdir} -n${n} -o SS_OFFSET=0.0,BARREL_ONLY=0 -q ${run_queue} ${EXTRA} -a ${part8common},${part8run},STOP,${part4common},${part4run}
}

function run_8 () {
    part8common=`run8common`
    part8run=`run8string ${ss8L} ${maxpat8} "${run_region}"`
    ./ftk.py -s5 -r ${outdir} -n${n} -o SS_OFFSET=0.0,BARREL_ONLY=0 -q ${run_queue} ${EXTRA} -a ${part8common},${part8run}
}

function run_4 () {
    part4common=`run4common`
    part4run=`run4string ${ss4L} ${maxpat4} "${run_region}"`
    ./ftk.py -s5 -r ${outdir} -n${n} -o SS_OFFSET=0.0,BARREL_ONLY=0 -q ${run_queue} ${EXTRA} -a ${part4common},${part4run}
}

# Example of how to create many 4L effcurve points
# Assumed 8L has been run already!
function run_eff4 () {
    # 8L + 4L + 4/11L (CM case):
    s2a=`run4string 25x156I225x36 15000000 0`
    s2b=`run4string 25x156I225x36 10000000 0`
    s2c=`run4string 25x156I225x36 7500000 0`
    s3a=`run4string 25x104I144x36 20000000 0`
    s3b=`run4string 25x104I144x36 15000000 0`
    s3c=`run4string 25x104I144x36 10000000 0`
    
    s2a=`run4string 20x156I225x25 23000000 0`
    s2b=`run4string 20x156I225x25 15000000 0`
    s2c=`run4string 20x156I225x25 10000000 0`
    s3a=`run4string 20x104I144x25 30000000 0`
    s3b=`run4string 20x104I144x25 25000000 0`
    s3c=`run4string 20x104I144x25 15000000 0`

    s2a=`run4string 15x156I225x20 33000000 0`
    s2b=`run4string 15x156I225x20 25000000 0`
    s2c=`run4string 15x156I225x20 20000000 0`
    s3a=`run4string 15x104I144x20 40000000 0`
    s3b=`run4string 15x104I144x20 30000000 0`
    s3c=`run4string 15x104I144x20 20000000 0`

    sMel=`run4string 20x156I225x25 23000000 0`

    sMel0=`run4string 20x156I225x25 100000 0`
    sMel1=`run4string 20x156I225x25 500000 0`
    sMel2=`run4string 20x156I225x25 1000000 0`
    sMel3=`run4string 20x156I225x25 1500000 0`
    sMel4=`run4string 20x156I225x25 2000000 0`
    sMel5=`run4string 20x156I225x25 3000000 0`
    sMel6=`run4string 20x156I225x25 5000000 0`
    sMel7=`run4string 20x156I225x25 7500000 0`
    sMel8=`run4string 20x156I225x25 10000000 0`
    sMel9=`run4string 20x156I225x25 15000000 0`
    sMel10=`run4string 20x156I225x25 20000000 0`

    part4common=`run4common`
    
    ./ftk.py -s5 -r ${outdir} -n${n} -o SS_OFFSET=0.0,CHI2_DOF=${chi2dofcut},BARREL_ONLY=0 -q ${run_queue} ${EXTRA} -a ${part4common},${sMel0},STOP,STOP,${sMel1},STOP,STOP,${sMel2},STOP,STOP,${sMel3},STOP,STOP,${sMel4},STOP,STOP,${sMel5},STOP,STOP,${sMel6},STOP,STOP,${sMel7},STOP,STOP,${sMel8},STOP,STOP,${sMel9},STOP,STOP,${sMel10}
}

# Example of how to create many 4L effcurve points
# Assumed 8L has been run already!
function run_eff () {
    # 8L + 4L + 4/11L (CM case):
    s2a=`run4string 25x156I225x36 15000000 "${run_region}" "[5]"`
    s2b=`run4string 25x156I225x36 10000000 "${run_region}" "[5]"`
    s2c=`run4string 25x156I225x36 7500000 "${run_region}" "[5]"`
    s3a=`run4string 25x104I144x36 20000000 "${run_region}" "[5]"`
    s3b=`run4string 25x104I144x36 15000000 "${run_region}" "[5]"`
    s3c=`run4string 25x104I144x36 10000000 "${run_region}" "[5]"`
    
    s2a=`run4string 20x156I225x25 23000000 "${run_region}" "[5]"`
    s2b=`run4string 20x156I225x25 15000000 "${run_region}" "[5]"`
    s2c=`run4string 20x156I225x25 10000000 "${run_region}" "[5]"`
    s3a=`run4string 20x104I144x25 30000000 "${run_region}" "[5]"`
    s3b=`run4string 20x104I144x25 25000000 "${run_region}" "[5]"`
    s3c=`run4string 20x104I144x25 15000000 "${run_region}" "[5]"`

    s2a=`run4string 15x156I225x20 33000000 "${run_region}" "[5]"`
    s2b=`run4string 15x156I225x20 25000000 "${run_region}" "[5]"`
    s2c=`run4string 15x156I225x20 20000000 "${run_region}" "[5]"`
    s3a=`run4string 15x104I144x20 40000000 "${run_region}" "[5]"`
    s3b=`run4string 15x104I144x20 30000000 "${run_region}" "[5]"`
    s3c=`run4string 15x104I144x20 20000000 "${run_region}" "[5]"`

    sMel=`run4string 20x156I225x25 23000000 "${run_region}" "[5]"`

    sMel0=`run4string 20x156I225x25 100000 "${run_region}" "[5]"`
    sMel1=`run4string 20x156I225x25 500000 "${run_region}" "[5]"`
    sMel2=`run4string 20x156I225x25 1000000 "${run_region}" "[5]"`
    sMel3=`run4string 20x156I225x25 1500000 "${run_region}" "[5]"`
    sMel4=`run4string 20x156I225x25 2000000 "${run_region}" "[5]"`
    sMel5=`run4string 20x156I225x25 3000000 "${run_region}" "[5]"`
    sMel6=`run4string 20x156I225x25 5000000 "${run_region}" "[5]"`
    sMel7=`run4string 20x156I225x25 7500000 "${run_region}" "[5]"`
    sMel8=`run4string 20x156I225x25 10000000 "${run_region}" "[5]"`
    sMel9=`run4string 20x156I225x25 15000000 "${run_region}" "[5]"`
    sMel10=`run4string 20x156I225x25 20000000 "${run_region}" "[5]"`
    sMel11=`run4string 20x156I225x25 23000000 "${run_region}" "[5]"`

    part8common=`run8common`
    part8run=`run8string ${ss8L} ${maxpat8} "${run_region}"`
    part4common=`run4common`
    
    ./ftk.py -s5 -r ${outdir} -n${n} -o SS_OFFSET=0.0,CHI2_DOF=${chi2dofcut},BARREL_ONLY=0 -q ${run_queue} ${EXTRA} -a ${part8common},${part8run},STOP,${part4common},${sMel0},STOP,${sMel1},STOP,${sMel2},STOP,${sMel3},STOP,${sMel4},STOP,${sMel5},STOP,${sMel6},STOP,${sMel7},STOP,${sMel8},STOP,${sMel9},STOP,${sMel10},STOP,${sMel11}
}

# When running locally, use the examples below:
local_enabled=true
if [ "${local_enabled}" == "true" ]; then

    dataset=wh3e34ZEROREC
    dataset=effmuons0         # from Joe: some files are centra-eta only, some large-eta only!
    dataset=effmuons0small    # 7k events, uniform in eta!
    dataset=v14indep
    
    outdir=/share/ftkdata1/melachri/jan12_wh
    maxpat8=4000000
    reg=''
    n=51
    for i in {0..19}; do 
	dataset=wh3e34bb${i}
    #run_all
	dataset=wh3e34uu${i}
    #run_all
    done
    
    outdir=/share/ftkdata1/antonk/jan22_extrawide_ctheta
    
    run_region=""
    dataset=v14indepCOPY
    n=100000
    #run_all
    dataset=wh3e34ZEROREC
    n=100
    #run_all
    
    outdir=/share/ftkdata1/antonk/apr01_new
    dataset=wh3e34ZEROREC
    n=100
    run_region=""
    run_all
fi;
