#!/bin/bash

# the script should be called via: "inp=3 reg=1 sub=2 loop=0 inp=0 ./prun_job.sh $@"
# all arguments are directly passed to ftk.py

####  The following processing modes are supported: ####
#  ftk           - Original 11L or TSP (one job per subregion)
#  ftk_eff       - Effchain for TSP arch (several different values of MAXPATTERNS)
#  split         - Split arch - one job per 8L subregion (but it still needs all 4L subregions!)
#  split_eff     - Effchain for the above (changing #patts at 4L stage)
#  711           - Regular mode with 7L fit plut 11L extrapolation and fit
#  711_eff       - Effchain in regular mode with 7L fit plut 11L extrapolation and fit
#  merge_X       - Merge FTK output from run mode X (e.g., merge_split_eff)
#  pconst        - Patterns from constants production
#  merge_pconst  - Merge patterns from constants production
########### ADDITIONAL MODES (mostly deprecated) #############
#  ftk_reg       - Original 11L or TSP (one job per region + TrigFTKAna)
#  split_reg     - Split arch - one job per region (also does TrigFTKAna) [has correct road flow]
#  split_reg_eff - Effchain for the above (changing #patts at 4L stage)
#  tsp           - not implemented yet (true TSP simulation from Pisa)

#########################################
# GENERAL SETTINGS (kapliy@gmail.com)
#########################################
# choose queue to use: "silent" (default) or "normal" (for debugging)
queue='normal'
echo "Working Directory " 
pwd
nLayersFinalFit=12
ROOTDIR=`pwd`
SCRIPTDIR=${ROOTDIR}/TrigFTKSim/standalone_scripts
OUTPUTDIR=${ROOTDIR}/output
_PATT='patt.bz2'
_GCON='gcon.bz2'
_DAT='dat.bz2'

# prepare the environment
export inp=${inp}
export reg=${reg}
export sub=${sub}
export loop=${loop}
chmod u+wrx prun_compile.sh
chmod g+rx prun_compile.sh
chmod a+rx prun_compile.sh
source prun_functions.sh
source prun_compile.sh 1

# override some dcache settings
export DCACHE_DEBUG=0
#unset DC_LOCAL_CACHE_BUFFER

# Before doing anything else, make a list of all pilot-staged files
cd ${ROOTDIR}
STAGED_PATHS="$ROOTDIR/ROOTDIR_STAGEDPATHS"
STAGED_FILES="$ROOTDIR/ROOTDIR_STAGEDFILES"
STAGED_INPATH="$ROOTDIR/ROOTDIR_STAGEDINPATH"
STAGED=${STAGED_PATHS}
prepareStaged

# parse command line arguments
# note that prun should have set these variables for us already:
# mode, reg, sub, loop
if [ ! -n "${mode}" ]; then 
    echo "ERROR: please set mode, reg, and sub variables. Exiting...."
    exit -1;
fi;
extra_args=$@
# trickery below is needed to properly preserve quotes
# extra_args breaks the field-separating quotes, so we instead construct an array extraArgs:
i=0
for arg in "$@"; do
    extraArgs[i]="$arg"   # this will be passed to ftk.py as "${extraArgs[@]}"
    let i+=1
done;
nsubs=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-m") break; if(i<NF) print $(i+1); }'`
nsubsMinus1=`expr $nsubs - 1`
echo "FTK run mode: ${mode}"
echo "FTK will run on region ${reg}, subregion ${sub}, loop ${loop}"
echo "FTK command line options: ${extra_args}"

# extract some parameters from ftk.py options string
runstring=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-d") break; if(i<NF) print $(i+1); }'`
if [ "$runstring" == "" ]; then runstring=raw_11L_50x64x144_mydata; fi;
dataset=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-d") break; if(i<NF) print $(i+1); }' | awk 'BEGIN{FS="_";}; {print $4}'`
if [ "$dataset" == "" ]; then dataset=mydata; fi;
echo "Default dataset: ${dataset}"
numLayers=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-d") break; if(i<NF) print $(i+1); }' | awk 'BEGIN{FS="_";}; {print $2}'`
if [ "$numLayers" == "" ]; then numLayers=11L; fi;
echo "Default numLayers: ${numLayers}"
## fix the mismatch between the used PMAP and the bank files where
## the subversion is not used, i.e.: 8Lb --> 8L
bankNumLayers=`echo $numLayers | sed -e "s/L.*/L/"`
## numeric value of the number of layers
MYnumLayers=`echo $bankNumLayers | sed -e "s,L,,"`
echo "bankNumLayers $bankNumLayers"
SSname=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-d") break; if(i<NF) print $(i+1); }' | awk 'BEGIN{FS="_";}; {print $3}'`
if [ "$SSname" == "" ]; then SSname=50x64x36n200M; fi;
echo "Default SSname: ${SSname}"
dSTRING=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-d") break; if(i<NF) print $(i+1); }'`
echo "Default dSTRING: ${dSTRING}"
maxpatterns=`extractOption MAXPATTERNS`
if [ "${maxpatterns}" == "" ]; then maxpatterns=100000000; fi;
echo "maxpatterns = ${maxpatterns}"
pc_mode=`extractOption PC_MODE`
if [ "${pc_mode}" == "" ]; then pc_mode=0; fi; # pc_mode means 8L patt-from-const bootstrapping from 11L

ftkDS=`extractOption ftkDS`
inputDS=`extractOption inputDS`
if [ "$inputDS" == "" ]; then 
    echo "Input dataset (-o inputDS) was not given on command line";
else
    minusIarg=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-I") break; if(i<NF) print $(i+1); }'`;
fi;
echo "inputDS: ${inputDS}"
fmin=`extractOption fmin`
fmax=`extractOption fmax`
echo "Will run over files [${fmin}..${fmax}] from dataset ${inputDS}"
COPY_FROM_DCACHE=`extractOption COPY_FROM_DCACHE`
echo "COPY_FROM_DCACHE=${COPY_FROM_DCACHE}"
if [ "${COPY_FROM_DCACHE}" == "1" ]; then
    echo "Warning: input wrapper files will be copied from dcache to local disk!"
fi;

# bankdir (or pattern dir if baseDS is used)
bankdir=`echo $extra_args | awk '{for(i=0;i<NF;i++) if($i=="-t") break; if(i<NF) print $(i+1); }'`
if [ "$bankdir" == "" ]; then 
    echo "WARNING: bank directory (-t) was not given on command line; reverting to prun_job.sh defaults"
    bankdir=/pnfs/uchicago.edu/data/tier3/user/user.AntonKapliy/ftkbanksV14 # V14 banks
fi;
baseDS=`extractOption baseDS`
setBankDirs

_PATTDB=`extractOption _PATTDB patt.root`
# extract the effective action for roadfinder and trackfitter
RFmode=`extractOption RFmode roadfinder`
if [ $RFmode = "roadfindertsp" ]; then
    echo RFmode=$RFmode;
    if [ "${mode}" != "pconst" ]; then
	_PATT=${_PATTDB}
	echo Changing _PATT to _PATT=${_PATTDB};
    fi
fi
TFmode=`extractOption TFmode trackfitter`
ANAmode=`extractOption ANAmode ana`
NTowers=`extractOption N 8`
TRACKFITTER_MODE=`extractOption TRACKFITTER_MODE 0`

UNSPLIT_SECTORS=`extractOption UNSPLIT_SECTORS`
PATMERGEROOT=`extractOption PATMERGEROOT`
MAKECACHE=`extractOption MAKECACHE`
CACHEDBANK=`extractOption CACHEDBANK`
ROOT_BASED_PATMERGE=`extractOption ROOT_BASED_PATMERGE`
SECTORSASPATTERNS=`extractOption SECTORSASPATTERNS`

cd ${SCRIPTDIR}
# cy adding remove statements on the grid, so it downloads the banks, and does not use the ones used for local tests
rm -rf patt/
rm -rf gcon/
# Run in regular (TSP) mode
# one job per subregion
if [ "${mode}" == "ftk" ]; then
    set -x
    runTSP ${reg} ${sub} ${maxpatterns}
    set +x

# Merge results of a regular per-subregion run
elif [ "${mode}" == "merge_ftk" ]; then
    set -x
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    mergeSubregions ${ftkDS} 1 0
    set +x

# Merge results of a regular per-subregion run
elif [ "${mode}" == "ana_ftk" ]; then
    set -x
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    runAnalysis ${ftkDS} 1 0
    set +x

elif [ ${mode} == "exp_ftk" ]; then
    set -x 
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    runExplorer ${ftkDS} 1 0
    set +x

# Run in regular (TSP) mode with several values of MAXPATTERNS (to build efficiency curve)
# Note that effcurve is always run in region 0 only. "reg" is then used to encode patternPoint index.
# one job per subregion
elif [ "${mode}" == "ftk_eff" ]; then
    set -x
    patternPoints=`python -c "from FTKCore import patternPoints; print ' '.join([str(j) for j in patternPoints if j<=${maxpatterns}])"`
    # choose a pattern point encoded in ${reg} variable, which is set by athena
    patternPoint=`python -c "from FTKCore import patternPoints; print patternPoints[${loop}]"`
    # change the region number below if you want to run effcurve on a different region!
    runTSP ${reg} ${sub} "${patternPoint}"
    set +x

# Merge results of effcurve run and build efficiency curves
elif [ "${mode}" == "merge_ftk_eff" ]; then
    set -x
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    realRegion=$reg
    mergeSubregions ${ftkDS} 0 $realRegion
    set +x

# Merge results of effcurve run and build efficiency curves
elif [ "${mode}" == "merge_ftk_neweff" ]; then
    set -x
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    realRegion=$reg
    effcurveTowers ${ftkDS}
    set +x

# Run in regular (TSP) mode, but with 1 jobs per region, and with TrigFTKAna in the end
# Don't use this for 11L 3E34 TSP processing since it will incredibly slow (Nsubs times slower than mode 1)
# This may be useful for quick 7L/8L runs. But in general, ignore this option
elif [ "${mode}" == "ftk_reg" ]; then
    set -x
    runTSP ${reg} "`seq 0 ${nsubsMinus1}`" ${maxpatterns}
    set +x

# Merge results of a per-region TSP/split run
elif [ "${mode}" == "merge_ftk_reg" -o "${mode}" == "merge_split_reg" ]; then
    set -x
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    mergeSubregions ${ftkDS} 1
    set +x

# Run in split arch mode with 1 job per subregion [new parallelized split run]
elif [ "${mode}" == "split" ]; then
    set -x
    # note that ${maxpatterns} refer to 4L stage; for 8L, use -o MAXPATTERNS8=4000000
    runSPLIT ${reg} ${maxpatterns}
    set +x

# Run in split arch mode with 1 job per region, and with TrigFTKAna in the end
elif [ "${mode}" == "split_reg" ]; then
    set -x
    # note that ${maxpatterns} refer to 4L stage; for 8L, use -o MAXPATTERNS8=4000000
    runSPLIT_reg ${reg} ${maxpatterns}
    set +x

# Run effcurve in split arch mode with 1 job per region, and with TrigFTKAna in the end
# This runs several different values of 4L MAXPATTERNS.
# Note that effcurve always runs on region 0 ONLY
elif [ "${mode}" == "split_reg_eff" ]; then
    set -x
    # choose a pattern point encoded in ${reg} variable, which is set by athena
    patternPoint=`python -c "from FTKCore import patternPoints; print patternPoints[${loop}]"`
    # note that maxpatterns refers to 4L stage; for 8L, use -o MAXPATTERNS8=4000000
    runSPLIT_reg ${reg} ${patternPoint}
    set +x
    
# Run in ss-shifted mode [V13 only for now!] - deprecated!
elif [ "${mode}" == "disabled_ss_shifted" ]; then
    set -x
    bankdir0=${bankdir}
    bankdirSS="`dirname ${bankdir}`/`basename ${bankdir}`SS/"
    #bankdir0=/pnfs/uchicago.edu/data/tier3/user/user.AntonKapliy/hugeBank/    # v13 bank
    #bankdirSS=/pnfs/uchicago.edu/data/tier3/user/user.AntonKapliy/hugeBankSS/ # v13 bank
    SSCONFIG=50x64x144
    ./ftk.py "${extraArgs[@]}" -r ${OUTPUTDIR} -q ${queue} -d raw_${numLayers}_${SSCONFIG}_${dataset}_group0 -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -a [-d=raw_${numLayers}_${SSCONFIG}_${dataset}_group0:-t=${bankdir0}:SS_OFFSET=0.0],roadfinder${reg}_${sub},STOP,trackfitter${reg}_${sub},STOP,[-d=raw_${numLayers}_${SSCONFIG}_${dataset}_group1:-t=${bankdirSS}:SS_OFFSET=0.5],roadfinder${reg}_${sub},STOP,[-d=raw_${numLayers}_${SSCONFIG}_${dataset}_group0:SHIFTROADS=raw_${numLayers}_${SSCONFIG}_${dataset}_group1:FILTERED_ROADS:-t=${bankdir0}:SS_OFFSET=0.0],hitfilter${reg}_${sub},STOP,trackfitter${reg}_${sub}
    checkReturn
    ./ftk.py "${extraArgs[@]}" -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -d raw_${numLayers}_${SSCONFIG}_${dataset}_group0 -a list,log
    ./ftk.py "${extraArgs[@]}" -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -d raw_${numLayers}_${SSCONFIG}_${dataset}_group1 -a list,log
    set +x

# Run in regular mode with 7L fit plus 11L extrapolation and fit (711 mode)
elif [ "${mode}" == "711" ]; then
    set -x
    runTSP ${reg} ${sub} ${maxpatterns}
    set +x

# merge jobs produced by 711 mode
elif [ "${mode}" == "merge_711" ]; then
    set -x
    ftkDS=`extractOption ftkDS`
    checkFTKDS
    mergeSubregions ${ftkDS} 1
    set +x

# Run effcurve in regular mode with 7L fit plut 11L extrapolation and fit (8711 mode)
elif [ "${mode}" == "eff_711" ]; then
    set -x
    if isGridDS ${baseDS}; then
	retrieveBankFiles ${baseDS} {gcon/corrgen_raw_${numLayers}_${nsubs}M_reg${reg}_sub${sub}.${_GCON},patt/sectors_raw_${numLayers}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}}
    fi
    if isGridDS ${bankdir}; then
	retrieveBankFiles ${bankdir} patt/patterns_raw_${numLayers}_${SSname}_${nsubs}M_reg${reg}_sub${sub}.${_PATT}
    fi
    #get 11L files syncronized with 7L sectors
    if isGridDS ${baseDS}; then
	retrieveBankFiles ${baseDS} {gcon/corrgen_raw_${nLayersFinalFit}L_${nsubs}M_reg${reg}_sub${sub}.${_GCON},patt/sectors_raw_${nLayersFinalFit}L_${nsubs}M_reg${reg}_sub${sub}.${_PATT}}
    fi
    set +x
    if isGridDS ${inputDS}; then
	getSomeWrapperFiles ${inputDS}
    fi;
    set -x
    
    ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -a effchain711${reg}_${sub}
    checkReturn
#   ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -a roadfinder711${reg}_${sub},trackfitter711${reg}_${sub}
    ./ftk.py "${extraArgs[@]}" ${NEWBANKDIR[@]} -r ${OUTPUTDIR} -q ${queue} -o _PATT=${_PATT},_GCON=${_GCON},MERGED_ROADS=0 -a list,log
    set +x

# Reduce the size of a pattern file up to MAXPATTERNS patterns
elif [ "${mode}" == "reduce" ]; then
    set -x
    runREDUCE
    set +x

elif [ "${mode}" == "makedc" ]; then
    set -x
    runMakeDCBank
    set +x

# reduce number of subregions by a factor -o factor (loading -o MAXPATTERNS patts per input file)
elif [ "${mode}" == "resub" ]; then
    set -x
    runRESUB
    set +x

# Run in patterns from constants mode (produces patterns via TrigFTKLib)
elif [ "${mode}" == "pconst" ]; then
    set -x
    runPCONST
    set +x
	    
# Merging patterns from constants
elif [ "${mode}" == "merge_pconst" ]; then
    set -x
    echo "Running PCONST_MERGE at ${site}"
    runPCONST_MERGE ${site}
    set +x

elif [ "${mode}" == "merge_pconst_root" ]; then
    set -x
    echo "Running PCONST_MERGE_ROOT at ${site}"
    runPCONST_MERGE_ROOT ${site}
    set +x

else
    echo "Unsupported run mode: ${mode}"
fi

# pack up the final results to be registered in dq2 catalog
cd ${ROOTDIR}
if [ -d output/ ]; then 
    echo 'Job finished successfully. Bzipping the output...'
    tar cfj out_files.tbz2 output/
else
    echo "ERROR: job failed - output/ folder is missing"
fi;

# If an archive is absent after the job finishes, Panda will mark the job as failed
# touch out_files.tbz2

echo
echo "Directory listing (at end of prun_job.sh):"
echo
find . -maxdepth 2 -exec ls -ld \{\} \; ;

echo "Final space usage:"
du -shc *
