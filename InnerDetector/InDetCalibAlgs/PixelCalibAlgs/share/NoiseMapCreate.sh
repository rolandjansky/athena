#!/bin/zsh

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

maintaglong="PixNoiseMapLong-002-00"
maintagshor="PixNoiseMapShort-002-00"
upd4taglong="PixNoiseMapLong-BLKP-UPD4-000-02"
upd4tagshor="PixNoiseMapShort-BLKP-UPD4-000-03"
stream="calibration_PixelNoise"
workdir=`pwd`


if [[ ! -r  NoiseMap_run${1}_${stream}.root ]]; then
  echo Missing root file NoiseMap_run${1}_${stream}.root, exiting!
  exit -1
fi

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

EOF

chmod u+x $workdir/NoiseMapDBWriter_run${1}_${stream}.sh

$workdir/NoiseMapDBWriter_run${1}_${stream}.sh

grep "PixMapDBWriter       INFO [T]" $workdir/NoiseMapDBWriter_run${1}_${stream}.log
grep "special pixels." $workdir/NoiseMapDBWriter_run${1}_${stream}.log | sort -g -k 6 

echo 
echo Cleaning up
echo 

rm $workdir/NoiseMap_sqlite_validation_run${1}_${stream}.py
rm $workdir/NoiseMapDBWriter_run${1}_${stream}.sh
rm $workdir/NoiseMapDBWriter_run${1}_${stream}.log

echo It is now possible to upload the new noise map with the command
echo /afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py noisemap.db COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_PIXEL_W password

