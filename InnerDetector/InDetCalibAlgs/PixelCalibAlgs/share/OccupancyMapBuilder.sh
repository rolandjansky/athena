#!/bin/zsh

###
CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here,gcc49"
PACKAGE_SETUP_PATH="/afs/cern.ch/work/a/atlpixdq/Pixel_Calibration/ATHENA/${AtlasProject}-${AtlasVersion}"

#project="data15_cos"
project=${2}
stream=${3}

if [[ "${3}" == "retry" ]] || [[ -z "${3}" ]]; then
  stream="express_express" # default
fi

#stream="physics_MinBias" # default
#stream="physics_IDCosmic"
#echo "stream = ${stream}" # test

STAGE_SVCCLASS_TMP=$STAGE_SVCCLASS
#export STAGE_SVCCLASS="atlcal"
export STAGE_SVCCLASS="t0atlas"

#for ProjectTag in `nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/`; do
#  if [[ `nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/ 2>/dev/null` != "" ]]; then
#    DatasetName=`nsls /castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/|grep RAW$`
#    PathOnCastor=/castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/$DatasetName/
#  fi;
#done;
#eos ls root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/
alias eos=/afs/cern.ch/project/eos/installation/atlas/bin/eos.select
for ProjectTag in `eos ls root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/`;
do
  if [[ `eos ls root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/ 2>/dev/null` != "" ]]; then
    DatasetName=`eos ls root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/|grep RAW$`
    PathOnCastor=/castor/cern.ch/grid/atlas/tzero/prod1/perm/$ProjectTag/${stream}/00$1/$DatasetName/
  fi;
done;

echo $PathOnCastor # test

#PathOnCastor=/castor/cern.ch/grid/atlas/tzero/prod1/perm/data14_cos/${stream}/00$1/

if [[ $# -lt 1 || $# -gt 4 ]]; then
  #echo 1>&2 "Usage: $0 <runNumber> <ProjectTag> [<Stream>] [continue]"
  echo 1>&2 "Usage: $0 <runNumber> <ProjectTag> [<Stream>] [retry]"
  exit 127
fi;

LBMin=0
LBMax=-1

workdir=`pwd`

n=0;

declare -a filelist
#PathOnEos=`eos find -d /eos/atlas/atlasgroupdisk/det-ibl/data12_8TeV/RAW/other/ | grep "00${1}.${stream}" `
PathOnEos=`eos ls /eos/atlas/atlastier0/rucio/${project}/${stream}/00${1}/ | grep "RAW$"`
if [[ $PathOnEos != "" ]] then
  #filelist=( `eos ls $PathOnEos | grep $1` )
  #prefix="root://eosatlas/$PathOnEos"
  prefix="root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/${project}/${stream}/00${1}/$PathOnEos/"
  echo $prefix
  filelist=( `eos ls $prefix | grep $1` )
else
  #prefix="rfio:$PathOnCastor"
  #filelist=( `nsls $PathOnCastor | grep $1` )
  prefix="root://castoratlas/$PathOnCastor"
  filelist=( `eos ls ${prefix} | grep $1 | grep RAW` )
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
#yosuke 160415 topSequence.NoiseMapBuilder.LBMin = $LBMin
#yosuke 160415 topSequence.NoiseMapBuilder.LBMax = $LBMax
#topSequence.NoiseMapBuilder.CalculateNoiseMaps = True # kazuki
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

if [[ $# -lt 4 ]] && [[ "${3}" != "retry" ]] ; then
  echo "Submitting"

  bsub -q atlasb1 -J "NoiseMapBuilder_run${1}_${stream}[1-$(($part+1))]" $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh
  #bsub -q 1nh -J "NoiseMapBuilder_run${1}_${stream}[1-$(($part+1))]" $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMapBuilder_run${1}_${stream}.sh

elif [[ "$3" == "retry" ]] || [[ "$4" == "retry" ]]; then
  echo "'retry' option is found. will skip bsub."
  echo "*** Caution: Don't use 'retry' option unless batch jobs finished!"

fi;
echo;
echo "Waiting for jobs to finish"

joblist=()

if [[ "$3" == "retry" ]] || [[ "$4" == "retry" ]]; then
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

echo;
echo "Merging output"

cd $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}
hadd -k $workdir/OccupancyMap.root $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}/NoiseMap_*
mv $workdir/OccupancyMap.root $workdir/OccupancyMap_run${1}_${stream}.root
rm -rf $workdir/.NoiseMapBuilder_tmp_run${1}_${stream}*
cd $workdir


export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
