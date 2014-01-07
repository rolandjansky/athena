#!/bin/zsh

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

stream="express_express"

STAGE_SVCCLASS_TMP=$STAGE_SVCCLASS
export STAGE_SVCCLASS="atlcal"

for ProjectTag in `nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/`; do
  if [[ `nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/ 2>/dev/null` != "" ]]; then
    DatasetName=`nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/|grep RAW$`
    PathOnCastor=/castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/$DatasetName/
  fi;
done; 


if [[ $# -lt 1 || $# -gt 3 ]]; then
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
hadd $workdir/OccupancyMap_run${1}_${stream}.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_*
rm -rf $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}*
cd $workdir


export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
