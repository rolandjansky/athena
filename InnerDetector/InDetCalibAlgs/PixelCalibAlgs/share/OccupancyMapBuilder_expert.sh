#!/bin/zsh

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

STAGE_SVCCLASS_TMP=$STAGE_SVCCLASS
export STAGE_SVCCLASS="atlcal"

if [[ `expr match $2 "calibration"` == 0 ]]; then
  for ProjectTag in `nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/`; do
    if [[ `nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/$2/00$1/ 2>/dev/null` != "" ]]; then
      DatasetName=`nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/$2/00$1/|grep RAW$`
      PathOnCastor=/castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/$2/00$1/$DatasetName/
    fi;
  done; 
else
  PathOnCastor=/castor/cern.ch/grid/atlas/DAQ/2011/00$1/$2
fi;


if [[ $# -lt 2 || $# -gt 5 ]]; then
  echo 1>&2 "Usage: $0 <run number> <stream> [LBMin LBMax] [continue]"
  exit 127
fi;

LBMin=0
LBMax=-1

if [[ $# -ge 4 ]]; then
  LBMin=$3
  LBMax=$4
fi;


workdir=`pwd`

n=0;

nFiles=$((nFiles+`nsls $PathOnCastor|grep $1|wc -l`))

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

for FileName in `nsls $PathOnCastor|grep $1`; do
  i="rfio:$PathOnCastor/$FileName";

  part=$((n/filesPerSubjob))
  pathname=.NoiseMapBuilder_tmp_run${1}_${2}_part${part}
  if [ ! -e $workdir/$pathname ]; then mkdir $workdir/$pathname; fi;
  echo $i >> $workdir/$pathname/inputfilelist

  if [ ! -e $workdir/$pathname/NoiseMapBuilder_run${1}_${2}_part${part}.sh ]; then
    cat >> $workdir/$pathname/NoiseMapBuilder_run${1}_${2}_part${part}.sh << EOF
#!/bin/zsh

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source \$AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $workdir/$pathname
cat >> NoiseMapBuilder_run${1}_${2}_part${part}.py << EOG
include( "PixelCalibAlgs/NoiseMapBuilder.py" )
topSequence.NoiseMapBuilder.LBMin = $LBMin
topSequence.NoiseMapBuilder.LBMax = $LBMax
EOG
athena NoiseMapBuilder_run${1}_${2}_part${part}.py 
touch finished
EOF

    chmod u+x $workdir/$pathname/NoiseMapBuilder_run${1}_${2}_part${part}.sh
  fi;
 
  n=$((n+1)) 

done;

if [ ! -e $workdir/.NoiseMapBuilder_tmp_run${1}_${2} ]; then mkdir $workdir/.NoiseMapBuilder_tmp_run${1}_${2}; fi;

if [ ! -e $workdir/.NoiseMapBuilder_tmp_run${1}_${2}/NoiseMapBuilder_run${1}_${2}.sh ]; then
  cat >> $workdir/.NoiseMapBuilder_tmp_run${1}_${2}/NoiseMapBuilder_run${1}_${2}.sh << EOF
#!/bin/zsh

cd $workdir/.NoiseMapBuilder_tmp_run${1}_${2}_part\$((\$LSB_JOBINDEX-1))
./NoiseMapBuilder_run${1}_${2}_part\$((\$LSB_JOBINDEX-1)).sh
touch finished
EOF

  chmod u+x $workdir/.NoiseMapBuilder_tmp_run${1}_${2}/NoiseMapBuilder_run${1}_${2}.sh
fi;

if [[ $# -eq 2 || $# -eq 4 ]]; then
  echo "Submitting"

  bsub -q atlasb1 -J "NoiseMapBuilder_run${1}_${2}[1-$(($part+1))]" $workdir/.NoiseMapBuilder_tmp_run${1}_${2}/NoiseMapBuilder_run${1}_${2}.sh

  echo;
  echo "Waiting for jobs to finish" 
fi;

joblist=()

while [ $#joblist -ne $(($part+1)) ]; do
  sleep 10
  for (( i = 0; i <= $part; i++ )); do 
    if [ -e  $workdir/.NoiseMapBuilder_tmp_run${1}_${2}_part$i/finished ]; then
      mv $workdir/.NoiseMapBuilder_tmp_run${1}_${2}_part$i/NoiseMap.root $workdir/.NoiseMapBuilder_tmp_run${1}_${2}/NoiseMap_$i.root
      echo "Subjob $i finished"
      joblist=($joblist $i)
      rm $workdir/.NoiseMapBuilder_tmp_run${1}_${2}_part$i/finished
    fi;
  done;
done;

echo;
echo "Merging output"

cd $workdir/.NoiseMapBuilder_tmp_run${1}_${2}
hadd $workdir/OccupancyMap_run${1}_${2}.root $workdir/.NoiseMapBuilder_tmp_run${1}_${2}/NoiseMap_*
rm -rf $workdir/.NoiseMapBuilder_tmp_run${1}_${2}*
cd $workdir


export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
