#!/bin/zsh

#==========
# Setup
#==========

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
#PACKAGE_SETUP_PATH="/afs/cern.ch/work/s/silviam/yosuke20/ATHENA/$AtlasProject-$AtlasVersion"
PACKAGE_SETUP_PATH="/afs/cern.ch/work/a/atlpixdq/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"
echo ${PACKAGE_SETUP_PATH}

#maintaglong="PixNoiseMapLong-DATA-RUN2-000-00"
#maintagshor="PixNoiseMapShort-DATA-RUN2-000-00"
#upd4taglong="PixNoiseMapLong-BLKP-UPD4-000-02"
#upd4tagshor="PixNoiseMapShort-BLKP-UPD4-000-03"

maintaglong="PixNoiseMapLong-RUN2-DATA-UPD4-02"
maintagshor="PixNoiseMapShort-RUN2-DATA-UPD4-02"
upd4taglong="PixNoiseMapLong-BLKPA-2016-10"
upd4tagshor="PixNoiseMapShort-BLKPA-2016-10"

#project="data15_cos"
project=${2}
stream="calibration_PixelNoise" # default
#stream="physics_IDCosmic"
#stream="express_express" # use express_express for the moment

STAGE_SVCCLASS_TMP=$STAGE_SVCCLASS
#export STAGE_SVCCLASS="atlcal"
export STAGE_SVCCLASS="t0atlas"
#PathOnCastor=/castor/cern.ch/grid/atlas/DAQ/2012/00$1/${stream}
#PathOnCastor=/castor/cern.ch/grid/atlas/DAQ/2015/00$1/${stream}/

#==========
# Usage
#==========

if [[ $# -lt 1 || $# -gt 3 ]]; then
  echo 1>&2 "Usage: $0 <run number> <project tag> (retry)"
  exit 127
fi;

LBMin=0
LBMax=-1


workdir=`pwd`

n=0;

declare -a filelist
alias eos=/afs/cern.ch/project/eos/installation/atlas/bin/eos.select
#PathOnEos=`eos find -d /eos/atlas/atlasgroupdisk/det-ibl/data12_8TeV/RAW/other/ | grep "00${1}.${stream}" `
PathOnEos=`eos ls /eos/atlas/atlastier0/rucio/${project}/${stream}/00${1}/ | grep "RAW$"`
#PathOnCastor=/castor/cern.ch/grid/atlas/DAQ/2014/00$1/${stream}/
PathOnCastor=`eos ls root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/${project}/${stream}/00$1/ | grep "RAW$"`
if [[ $PathOnEos != "" ]] then
  prefix="root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/${project}/${stream}/00${1}/$PathOnEos/"
  echo $prefix
  filelist=( `eos ls $prefix | grep $1` )
else
  # case for cosmic runs as well
  #prefix="rfio:$PathOnCastor"
  #filelist=( `nsls $PathOnCastor | grep $1` )
  #prefix="root://castoratlas/$PathOnCastor" # kazuki
  prefix="root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/${project}/${stream}/00$1/$PathOnCastor/" # kazuki
  echo $prefix
  filelist=( `eos ls $prefix | grep $1` ) # kazuki
fi

nFiles=${#filelist[@]}

if (( nFiles <=0 )) then
  echo Unable to find files for run $1
  exit -1
fi

filesPerSubjob=0
if [[ $nFiles -le 50 ]]; then
  filesPerSubjob=5
elif [[ $nFiles -le 200 ]]; then
  filesPerSubjob=$((nFiles/10))
elif [[ $nFiles -le  600 ]]; then
  filesPerSubjob=$((nFiles/15))
elif [[ $nFiles -le 1000 ]]; then
  filesPerSubjob=$((nFiles/20))
elif [[ $nFiles -le 1500 ]]; then
  filesPerSubjob=$((nFiles/25))
elif [[ $nFiles -le 4000 ]]; then
  filesPerSubjob=$((nFiles/40))
else
  filesPerSubjob=$((nFiles/60))
fi;

nSubjobs=$((nFiles/filesPerSubjob))
if [[ $((nFiles%filesPerSubjob)) != 0 ]]; then
  nSubjobs=$((nSubjobs+1))
fi;

echo "Dataset contains $nFiles files. Splitting into $nSubjobs subjobs."

for FileName in ${filelist[@]}; do
  i="$prefix/$FileName";
  part=$((n/filesPerSubjob))
  pathname=.NoiseMapBuilder_tmp_run${1}_${stream}_part${part}
  if [ ! -e $workdir/$pathname ]; then
    rm -rf $workdir/$pathname;
    mkdir -p $workdir/$pathname;
  fi;
  echo $i >> $workdir/$pathname/inputfilelist
  if [ ! -e $workdir/$pathname/NoiseMapBuilder_run${1}_${stream}_part${part}.sh ]; then
    cat >> $workdir/$pathname/NoiseMapBuilder_run${1}_${stream}_part${part}.sh << EOF
#!/bin/zsh
export STAGE_SVCCLASS="t0atlas"
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source \$AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $workdir/$pathname
cat >> NoiseMapBuilder_run${1}_${stream}_part${part}.py << EOG
include( "PixelCalibAlgs/NoiseMapBuilder.py" )
#yosuke160415 topSequence.NoiseMapBuilder.LBMin = $LBMin
#yosuke160415 topSequence.NoiseMapBuilder.LBMax = $LBMax
#topSequence.NoiseMapBuilder.CalculateNoiseMaps = True # kazuki
EOG
athena NoiseMapBuilder_run${1}_${stream}_part${part}.py
touch finished
EOF

    chmod u+x $workdir/$pathname/NoiseMapBuilder_run${1}_${stream}_part${part}.sh
  fi;

  n=$((n+1))

done;

if [ ! -e $workdir/.NoiseMapBuilder_tmp_run${1}_${stream} ]; then mkdir -p $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}; fi;

if [ ! -e $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh ]; then
  cat >> $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh << EOF
#!/bin/zsh
export STAGE_SVCCLASS="t0atlas"
cd $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part\$((\$LSB_JOBINDEX-1))
./NoiseMapBuilder_run${1}_${stream}_part\$((\$LSB_JOBINDEX-1)).sh
touch finished
EOF

  chmod u+x $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh
fi;

#==========
# submit jobs to lxbatch
#==========

if [[ $# -eq 2 ]]; then
  echo "Submitting"

  bsub -q atlasb1 -J "NoiseMapBuilder_run${1}_${stream}[1-$(($part+1))]" $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh
  #bsub -q 1nh -J "NoiseMapBuilder_run${1}_${stream}[1-$(($part+1))]" $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh

elif [[ $3 == "retry" ]]; then
  echo "'retry' option is found. will skip bsub."
  echo "*** Caution: Don't use 'retry' option unless batch jobs finished!"

fi;
echo;
echo "Waiting for jobs to finish"

joblist=()

if [[ $3 == "retry" ]]; then
  for (( i = 0; i <= $part; i++ )); do
    if [ -e  $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part$i/finished ]; then
      mv $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part$i/NoiseMap.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_$i.root
      echo "Subjob $i finished"
      rm $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part$i/finished
    fi;
  done;
else
  while [ $#joblist -ne $(($part+1)) ]; do
    sleep 10
    for (( i = 0; i <= $part; i++ )); do
      if [ -e  $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part$i/finished ]; then
        mv $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part$i/NoiseMap.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_$i.root
        echo "Subjob $i finished"
        joblist=($joblist $i)
        rm $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part$i/finished
      fi;
    done;
  done;
fi

#==========
# Merge and update histograms
#==========

echo;
echo "Merging output"

cd $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}
hadd NoiseMap_merged.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_*
#echo "save merged file for debug"
#cp NoiseMap_merged.root NoiseMap_merged_spare.root
#hadd NoiseMap.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_* # kazuki # skip updateNoiseMaps.exe
updateNoiseMaps.exe NoiseMap_merged.root NoiseMap.root
mv NoiseMap.root $workdir/NoiseMap_run${1}_${stream}.root
rm -rf $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}*
cd $workdir

echo;
echo "Running validation"
echo;

if [[ -f noisemap.db ]] then
  echo "Removing old database file"
  rm noisemap.db
fi

echo "NoiseMapFileName = NoiseMap_run${1}_${stream}.root"

cat > $workdir/NoiseMap_sqlite_validation_run${1}_${stream}.py << EOF
NoiseMapFilename = "NoiseMap_run${1}_${stream}.root"
ReferenceDB = "default" #"oracle", "sqlite"
ReferenceRun = int("${1}")
ReferenceTag = '${maintagshor}'
ReferenceLongTag = '${maintaglong}'

include( "PixelCalibAlgs/NoiseMapValidation.py" )
EOF

echo;
echo "athena.py $workdir/NoiseMap_sqlite_validation_run${1}_${stream}.py >! $workdir/NoiseMapDBValid_run${1}_${stream}.log"
echo;

athena.py $workdir/NoiseMap_sqlite_validation_run${1}_${stream}.py >! $workdir/NoiseMapDBValid_run${1}_${stream}.log
grep "PixMapDBWriter       INFO [NC]" $workdir/NoiseMapDBValid_run${1}_${stream}.log >! NoiseMap_validation_run${1}.log
echo "========== modules with the largest difference ==========" >> NoiseMap_validation_run${1}.log
grep "pixels differing on module" $workdir/NoiseMapDBValid_run${1}_${stream}.log >> NoiseMap_validation_run${1}.log
echo "========== modules with the largest number of noisy pixels ==========" >> NoiseMap_validation_run${1}.log
grep "noisy pixels on module" $workdir/NoiseMapDBValid_run${1}_${stream}.log >> NoiseMap_validation_run${1}.log


cat > $workdir/NoiseMapDBWriter_run${1}_${stream}.sh << EOF
#!/bin/zsh
export STAGE_SVCCLASS="t0atlas"
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source \$AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $workdir

if [[ -f noisemap.db ]] then
 rm noisemap.db
fi

cat > $workdir/NoiseMapDBWriter_run${1}_${stream}.py << EOG
NoiseMapFilename = "NoiseMap_run${1}_${stream}.root"
OutputRun = int("${1}")
OutputLB = 0
doValidate = False

OutputTag = '${maintagshor}'
OutputLongTag = '${maintaglong}'

include( "PixelCalibAlgs/NoiseMapDBWriter.py" )

PixMapDBWriter.CalculateOccupancy = False
EOG

athena $workdir/NoiseMapDBWriter_run${1}_${stream}.py >! $workdir/NoiseMapDBWriter_run${1}_${stream}.log
rm $workdir/NoiseMapDBWriter_run${1}_${stream}.py
#
#cat > $workdir/NoiseMapDBWriter_run${1}_${stream}.py < < EOG
#NoiseMapFilename = "NoiseMap_run${1}_${stream}.root"
#OutputRun = int("${1}")
#OutputLB = 0
#doValidate = False
#
#OutputTag = '${upd4tagshor}'
#OutputLongTag = '${upd4taglong}'

#include( "PixelCalibAlgs/NoiseMapDBWriter.py" )
#EOG

#athena $workdir/NoiseMapDBWriter_run${1}_${stream}.py
#rm $workdir/NoiseMapDBWriter_run${1}_${stream}.py
EOF

chmod u+x $workdir/NoiseMapDBWriter_run${1}_${stream}.sh

echo;
echo "$workdir/NoiseMapDBWriter_run${1}_${stream}.sh"
echo;
$workdir/NoiseMapDBWriter_run${1}_${stream}.sh

grep "PixMapDBWriter       INFO [T]" $workdir/NoiseMapDBWriter_run${1}_${stream}.log >> NoiseMap_validation_run${1}.log
grep "special pixels." $workdir/NoiseMapDBWriter_run${1}_${stream}.log | sort -g -k 6 -r >> NoiseMap_validation_run${1}.log

echo "============================================"
echo "Summary of noise map creation for run ${1}"
echo "============================================"

#head -n 40 NoiseMap_validation_run${1}.log
cat NoiseMap_validation_run${1}.log

export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
