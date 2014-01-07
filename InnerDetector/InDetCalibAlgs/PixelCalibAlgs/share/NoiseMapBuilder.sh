#!/bin/zsh

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

maintaglong="PixNoiseMapLong-002-00"
maintagshor="PixNoiseMapShort-002-00"
upd4taglong="PixNoiseMapLong-BLKP-UPD4-000-02"
upd4tagshor="PixNoiseMapShort-BLKP-UPD4-000-03"
stream="calibration_PixelNoise"

STAGE_SVCCLASS_TMP=$STAGE_SVCCLASS
export STAGE_SVCCLASS="atlcal"
PathOnCastor=/castor/cern.ch/grid/atlas/DAQ/2012/00$1/${stream}

if [[ $# -lt 1 || $# -gt 2 ]]; then
  echo 1>&2 "Usage: $0 <run number> [continue]"
  exit 127
fi;

LBMin=0
LBMax=-1


workdir=`pwd`

n=0;

declare -a filelist
alias eos=/afs/cern.ch/project/eos/installation/atlas/bin/eos.select
PathOnEos=`eos find -d /eos/atlas/atlasgroupdisk/det-ibl/data12_8TeV/RAW/other/ | grep "00${1}.${stream}" `
if [[ $PathOnEos != "" ]] then
  filelist=( `eos ls $PathOnEos | grep $1` )
  prefix="root://eosatlas/$PathOnEos"
else
  prefix="rfio:$PathOnCastor"
  filelist=( `nsls $PathOnCastor | grep $1` )
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
  if [ ! -e $workdir/$pathname ]; then mkdir $workdir/$pathname; fi;
  echo $i >> $workdir/$pathname/inputfilelist
  if [ ! -e $workdir/$pathname/NoiseMapBuilder_run${1}_${stream}_part${part}.sh ]; then
    cat >> $workdir/$pathname/NoiseMapBuilder_run${1}_${stream}_part${part}.sh << EOF
#!/bin/zsh

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source \$AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $workdir/$pathname
cat >> NoiseMapBuilder_run${1}_${stream}_part${part}.py << EOG
include( "PixelCalibAlgs/NoiseMapBuilder.py" )
topSequence.NoiseMapBuilder.LBMin = $LBMin
topSequence.NoiseMapBuilder.LBMax = $LBMax
EOG
athena NoiseMapBuilder_run${1}_${stream}_part${part}.py 
touch finished
EOF

    chmod u+x $workdir/$pathname/NoiseMapBuilder_run${1}_${stream}_part${part}.sh
  fi;
 
  n=$((n+1)) 

done;

if [ ! -e $workdir/.NoiseMapBuilder_tmp_run${1}_${stream} ]; then mkdir $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}; fi;

if [ ! -e $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh ]; then
  cat >> $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh << EOF
#!/bin/zsh

cd $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}_part\$((\$LSB_JOBINDEX-1))
./NoiseMapBuilder_run${1}_${stream}_part\$((\$LSB_JOBINDEX-1)).sh
touch finished
EOF

  chmod u+x $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh
fi;

if [[ $# -eq 1 ]]; then
  echo "Submitting"

  bsub -q atlasb1 -J "NoiseMapBuilder_run${1}_${stream}[1-$(($part+1))]" $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh

fi;
echo;
echo "Waiting for jobs to finish" 

joblist=()

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

echo;
echo "Merging output"

cd $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}
hadd NoiseMap_merged.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_*
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

cat > $workdir/NoiseMap_sqlite_validation_run${1}_${stream}.py << EOF
NoiseMapFilename = "NoiseMap_run${1}_${stream}.root"
ReferenceDB = "default" #"oracle", "sqlite"
ReferenceRun = int("${1}")
ReferenceTag = '${maintagshor}'
ReferenceLongTag = '${maintaglong}'

include( "PixelCalibAlgs/NoiseMapValidation.py" )
EOF

athena.py $workdir/NoiseMap_sqlite_validation_run${1}_${stream}.py >! $workdir/NoiseMapDBValid_run${1}_${stream}.log
grep "PixMapDBWriter       INFO [NC]" $workdir/NoiseMapDBValid_run${1}_${stream}.log >! NoiseMap_validation_run${1}.log
grep "pixels differing on module" $workdir/NoiseMapDBValid_run${1}_${stream}.log >> NoiseMap_validation_run${1}.log



cat > $workdir/NoiseMapDBWriter_run${1}_${stream}.sh << EOF
#!/bin/zsh

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

$workdir/NoiseMapDBWriter_run${1}_${stream}.sh

grep "PixMapDBWriter       INFO [T]" $workdir/NoiseMapDBWriter_run${1}_${stream}.log >> NoiseMap_validation_run${1}.log
grep "special pixels." $workdir/NoiseMapDBWriter_run${1}_${stream}.log | sort -g -k 6 -r >> NoiseMap_validation_run${1}.log

echo "============================================"
echo "Summary of noise map creation for run ${1}"
echo "============================================"

head -n 40 NoiseMap_validation_run${1}.log

export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
