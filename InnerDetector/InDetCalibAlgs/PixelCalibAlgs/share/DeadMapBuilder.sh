#!/bin/zsh

#CMT_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/cmthome"
#CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,32"
CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
#PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion/cmt"
PACKAGE_SETUP_PATH="/afs/cern.ch/work/a/atlpixdq/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

#maintaglong="PixMapLong-RUN2-DATA-RUN1-UPD1-00"
#maintagshor="PixMapShort-RUN2-DATA-RUN1-UPD1-00"
#maintaglong="PixMapLong-DATA-RUN2-000-00"
#maintagshor="PixMapShort-DATA-RUN2-000-00"
#upd4taglong="PixMapLong-BLKP-UPD4-000-02"
#upd4tagshor="PixMapShort-BLKP-UPD4-000-03"

# used in BLK
maintaglong="PixMapLong-RUN2-DATA-RUN1-UPD1-00"
maintagshor="PixMapShort-RUN2-DATA-RUN1-UPD1-00"
# used in ES1
maintaglong="PixMapLong-RUN2-ES1C-UPD1-000-06"
maintagshor="PixMapShort-RUN2-ES1C-UPD1-000-06"

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
#ReferenceDB = "sqlite"
ReferenceRun = int("${1}")
#ReferenceTag = 'PixMapShort-000-00'
#ReferenceLongTag = 'PixMapLong-000-00'
ReferenceTag = '${maintagshor}'
ReferenceLongTag = '${maintaglong}'

include( "PixelCalibAlgs/PixMapValidation.py" )
EOF

athena.py $workdir/DeadMap_sqlite_validation_run${1}.py

cat > $workdir/DeadMapDBWriter_run${1}.sh << EOF
#!/bin/zsh

#cd $CMT_SETUP_PATH
#source setup.sh -tag=$CMT_SETUP_TAG
#cd $PACKAGE_SETUP_PATH
#source setup.sh
export STAGE_SVCCLASS="t0atlas"
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source \$AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $workdir

cat > $workdir/DeadMapDBWriter_run${1}.py << EOG
PixMapFilename = 'DeadMap_DeadMapWriter_run${1}.root'
PixelStatus = 2049
ListSpecialPixels = False
OutputRun = int("${1}")
OutputLB = 1
doValidate = False

#OutputTag = 'PixMapShort-000-00'
#OutputLongTag = 'PixMapLong-000-00'
OutputTag = '${maintagshor}'
OutputLongTag = '${maintaglong}'

#include( "PixelCalibAlgs/PixMapDBWriter.py" )
include( "PixelCalibAlgs/DeadMapDBWriter.py" )

PixMapDBWriter.CalculateOccupancy = False
EOG

athena $workdir/DeadMapDBWriter_run${1}.py
rm $workdir/DeadMapDBWriter_run${1}.py

EOF

chmod u+x $workdir/DeadMapDBWriter_run${1}.sh

echo;
echo "$workdir/DeadMapDBWriter_run${1}.sh"
echo;
$workdir/DeadMapDBWriter_run${1}.sh

export STAGE_SVCCLASS=$STAGE_SVCCLASS_TMP

echo "Job finished"
