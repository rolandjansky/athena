#!/bin/zsh

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

maintaglong="PixNoiseMapLong-001-02"
maintagshor="PixNoiseMapShort-001-02"
upd4taglong="PixNoiseMapLong-BLKP-UPD4-000-02"
upd4tagshor="PixNoiseMapShort-BLKP-UPD4-000-03"
stream="calibration_PixelNoise"
tempfile="temp.root"
sqlfile="noisemap.db"



if [[ $# -lt 2 ]]; then
  echo 1>&2 "Usage: $0 <run number 1> <run number 2> [<run number 3>...]"
  exit 127
fi;


workdir=`pwd`


declare -a filelist
runlist=""
nmerged=0

for run in $@ ; do
  fname=NoiseMap_run${run}_$stream.root
  if [ -r $fname ] ; then 
      echo "Merging noise map for run $run"
      nmerged=$((nmerged+1)) 
      filelist[${nmerged}]=$fname
      runlist="${runlist}_run${run}"
  else 
    if [[ $run != $0 ]] ; then
	echo "File $fname not found: skipping run $run"  
    fi
  fi
done

if [[ $nmerged -lt 2 ]] ; then
    echo No merging needed
    exit -1
fi 

mergedfile="NoiseMapMerged${runlist}_${stream}.root"
hadd -f $tempfile ${filelist[@]}

if [[ $? -ne 0 ]] ; then
  echo Error in merging $filelist into $mergedfile
  exit -2
fi;

updateNoiseMaps.exe $tempfile $mergedfile
rm $tempfile

echo;
echo "Running validation"
echo;

if [[ -f $sqlfile ]] then
  echo "Removing old database file"
  rm $sqlfile
fi

athenafile="$workdir/NoiseMapMerged_sqlite_validation${runlist}_${stream}.py"
outputfile="$workdir/NoiseMapDBValid${runlist}_${stream}.log"
summaryfile="$workdir/Noisemapmerged_validation${runlist}.log"  
cat > $athenafile << EOF
NoiseMapFilename = "$mergedfile"
ReferenceDB = "default" #"oracle", "sqlite"
ReferenceRun = int("${1}")
ReferenceTag = '${maintagshor}'
ReferenceLongTag = '${maintaglong}'

include( "PixelCalibAlgs/NoiseMapValidation.py" )
EOF

athena.py $athenafile >! $outputfile
grep "PixMapDBWriter       INFO [NC]" $outputfile >! $summaryfile
grep "pixels differing on module" $outputfile >> $summaryfile


scriptfile="$workdir/NoiseMapDBWriter${runlist}_${stream}.sh"
cat > $scriptfile << EOF
#!/bin/zsh

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source \$AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $workdir

if [[ -f ${sqlfile} ]] then
 rm ${sqlfile}
fi

cat > $workdir/NoiseMapDBWriter${runlist}_${stream}.py << EOG
NoiseMapFilename = "NoiseMap_run${1}_${stream}.root"
OutputRun = int("${1}")
OutputLB = 0
doValidate = False

OutputTag = '${maintagshor}'
OutputLongTag = '${maintaglong}'

include( "PixelCalibAlgs/NoiseMapDBWriter.py" )

PixMapDBWriter.CalculateOccupancy = False
EOG

athena $workdir/NoiseMapDBWriter${runlist}_${stream}.py >! $workdir/NoiseMapDBWriter${runlist}_${stream}.log
rm $workdir/NoiseMapDBWriter${runlist}_${stream}.py
EOF

chmod u+x $scriptfile

./$scriptfile

grep "PixMapDBWriter       INFO [T]" $workdir/NoiseMapDBWriter_run${runlist}_${stream}.log >> $summaryfile
grep "special pixels." $workdir/NoiseMapDBWriter_run${runlist}_${stream}.log | sort -g -k 6 -r >> $summaryfile

echo "============================================"
echo "Summary of noise map creation for run ${1}"
echo "============================================"

head -n 40 $summaryfile

export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
echo "Look at summary output in $summaryfile"
echo It is now possible to upload the new noise map with the command
echo /afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py ${sqlfile} COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_PIXEL_W password
