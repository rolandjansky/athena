#!/bin/zsh

CMT_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/cmthome"
CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,32"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion/cmt"

workdir=`pwd`

if [[ $# -lt 2 ]]; then
  echo 1>&2 "Usage: $0 <IOV_RMin> <filename>"
  exit 127
fi;

updateDeadMaps.exe ${2} DeadMap_DeadMapWriter_run${1}.root

echo;
echo "Running validation"
echo;

cat > $workdir/DeadMap_sqlite_validation_run${1}.py << EOF
PixMapFilename = 'DeadMap_DeadMapWriter_run${1}.root'
PixelStatus = 2049
ListSpecialPixels = False
ReferenceDB = "default" #"oracle", "sqlite"
ReferenceRun = int("${1}")
ReferenceTag = 'PixMapShort-000-00'
ReferenceLongTag = 'PixMapLong-000-00'

include( "PixelCalibAlgs/PixMapValidation.py" )
EOF

athena.py $workdir/DeadMap_sqlite_validation_run${1}.py

cat > $workdir/DeadMapDBWriter_run${1}.sh << EOF
#!/bin/zsh

cd $CMT_SETUP_PATH
source setup.sh -tag=$CMT_SETUP_TAG
cd $PACKAGE_SETUP_PATH
source setup.sh
cd $workdir

cat > $workdir/DeadMapDBWriter_run${1}.py << EOG
PixMapFilename = 'DeadMap_DeadMapWriter_run${1}.root'
PixelStatus = 2049
ListSpecialPixels = False
OutputRun = int("${1}")
OutputLB = 0
doValidate = False

OutputTag = 'PixMapShort-000-00'
OutputLongTag = 'PixMapLong-000-00'

include( "PixelCalibAlgs/PixMapDBWriter.py" )

PixMapDBWriter.CalculateOccupancy = False
EOG

athena $workdir/DeadMapDBWriter_run${1}.py
rm $workdir/DeadMapDBWriter_run${1}.py

cat > $workdir/DeadMapDBWriter_run${1}.py << EOG
PixMapFilename = 'DeadMap_DeadMapWriter_run${1}.root'
PixelStatus = 2049
ListSpecialPixels = False
OutputRun = int("${1}")
OutputLB = 0
doValidate = False

OutputTag = 'PixMapShort-BLKP-UPD4-000-00'
OutputLongTag = 'PixMapLong-BLKP-UPD4-000-00'

include( "PixelCalibAlgs/PixMapDBWriter.py" )
EOG

athena $workdir/DeadMapDBWriter_run${1}.py
rm $workdir/DeadMapDBWriter_run${1}.py
EOF

chmod u+x $workdir/DeadMapDBWriter_run${1}.sh

export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
