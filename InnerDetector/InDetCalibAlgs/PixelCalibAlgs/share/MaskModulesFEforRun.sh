#!/bin/zsh

CMT_SETUP_TAG="$AtlasVersion,$AtlasProject,here"
PACKAGE_SETUP_PATH="/afs/cern.ch/user/i/idcalib/w0/Pixel_Calibration/ATHENA/$AtlasProject-$AtlasVersion"

STAGE_SVCCLASS_TMP=$STAGE_SVCCLASS
export STAGE_SVCCLASS="atlcal"

CURRENTDIR=`pwd`

maintag="PixMapOverlay-shift11-000-00"
upd4tag="PixMapOverlay-UPD4-000-03"
sqlfile="maskedmodules.db"
filename=PixMapOverlayWriter_temp.py


if [[ $# -lt 1 || $# -gt 4 ]] ; then
  echo Usage $0 run_number [ LB_start run_end LB_end ]
  exit -1
elif [[ $# == 1 ]]; then
  runstart=$1
  LBstart=0
  runend=$(( $1 + 1 ))
  LBend=0
  echo Disabling modules and FEs for run $1
elif [[ $# == 4 ]]; then
  runstart=$1
  LBstart=$2
  runend=$3
  LBend=$4
  echo Disabling modules and FE from run $1 lumiblock $2 \(included\) to run $3 lumiblock $4 \(excluded\)
else
  echo Reading $# arguments from the command line, only 1 or 4 expected
  echo Either disable modules and FEs for a whole run, 
  echo or need to specify the full run/lumi-block interval
  exit -2
fi 

if [[ -f $filename ]]; then rm $filename ; fi

cat >> $filename << EOF
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'
globalflags.print_JobProperties()

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.pixel_setOn()
DetFlags.Print()

from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-BLKPST-004-05')

conddb.iovdbsvc.dbConnection = "sqlite://;schema=${sqlfile};dbname=COMP200"

DetDescrVersion = "ATLAS-GEO-16-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

from PixelCalibAlgs.PixelCalibAlgsConf import PixMapOverlayWriter

job += PixMapOverlayWriter()
job.PixMapOverlayWriter.ModuleStatus = 1

from AthenaServices.AthenaServicesConf import AthenaOutputStream

job += AthenaOutputStream( "Stream1" )

from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

ServiceMgr += SpecialPixelMapSvc()
SpecialPixelMapSvc = ServiceMgr.SpecialPixelMapSvc

SpecialPixelMapSvc.RegisterCallback = False    
SpecialPixelMapSvc.OutputFolder = "/PIXEL/PixMapOverlay"

### configure OutputConditionsAlg

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg","PixMapOverlay.root")

OutputConditionsAlg.ObjectList=[ "CondAttrListCollection#" + SpecialPixelMapSvc.OutputFolder ]

OutputConditionsAlg.WriteIOV=True

OutputConditionsAlg.Run1=${runstart}
OutputConditionsAlg.LB1=${LBstart}
OutputConditionsAlg.Run2=${runend}
OutputConditionsAlg.LB2=${LBend}

OutputConditionsAlg.IOVTagList=["${upd4tag}"]


### configure IOVRegistrationSvc

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc

ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "ModuleSpecialPixelMap_Clob" ]
regSvc.OverrideTypes = [ "String4k" ]


### configure the event selector

from GaudiSvc.GaudiSvcConf import EventSelector

ServiceMgr += EventSelector()

EventSelector.RunNumber         = ${runstart}
EventSelector.EventsPerRun      = 5
EventSelector.FirstEvent        = 1
EventSelector.InitialTimeStamp  = 0
EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1


### configure the message service
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )

MessageSvc.OutputLevel      = 3
MessageSvc.debugLimit       = 100000
MessageSvc.infoLimit        = 100000
MessageSvc.errorLimit       = 1000
job.PixMapOverlayWriter.ModuleIDFragments = [ ]
job.PixMapOverlayWriter.Frontends = { }


EOF

echo Insert one line for each module or FE to disable
echo in the format:
echo MODULE module_name
echo MODULE module_name FE fe_number\[,fe_number,fe_number...\]
echo It is possible to disable group of modules \(but not FE\)
echo providing only the initial part of the name
echo for example 
echo MODULE L0_B04_S2
echo will disable a full stave.
echo Type END or Ctrl-D to end input.

while read type; do
  if expr match $type '\(MODULE\)' > /dev/null ; then
    module=${type##MODULE\ }
    if expr match $type '\(.*FE\)' > /dev/null ; then
      fe=${module##*FE\ }
      module=${module%%\ FE*}
      if [[ $disablefelist == "" ]]; then
	disablefelist=\"${module}\":\ \[${fe}\]
      else
	disablefelist+=\,\"${module}\":\ \[${fe}\]
      fi
    else
      echo job.PixMapOverlayWriter.ModuleIDFragments += [ \"${module}\" ]>> $filename
    fi
  elif  [[ $type == "END" ]]; then
    break
  else
    echo keyword $type not understood 
    echo \(only MODULE line or END are allowed\)
  fi
done
if [[ $disablefelist != "" ]]; then
  echo job.PixMapOverlayWriter.Frontends = \{ ${disablefelist} \} >> $filename
fi

echo ...setting up athena
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'
cd $PACKAGE_SETUP_PATH
asetup $CMT_SETUP_TAG
cd $CURRENTDIR

athena $filename | tee tmp.log
echo The following modules have been disabled:
grep "1 0 *$" tmp.log
if [[ $? != 0 ]] then
    echo NONE
fi
echo The following modules have some disabled FE:
grep "0 .* 1 *$" tmp.log
if [[ $? != 0 ]] then
    echo NONE
fi
grep "Registration failed" tmp.log
if [[ $? == 0 ]] then
  echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  echo There was an ERROR in updating ${sqlfile} !!!!!!
  echo Usually that is due to a too big size of the file
  echo or to a quota problem.
  echo Try to delete the file ${sqlfile} and restart the procedure.
  echo If that also fails, contact the experts.
  echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
else 
  rm tmp.log
  #AtlCoolCopy.exe  "sqlite://;schema=${sqlfile};dbname=COMP200" "sqlite://;schema=${sqlfile};dbname=COMP200" -create -folder /PIXEL/PixMapOverlay -tag ${maintag} -outtag ${upd4tag}
  echo It is now possible to upload the new module and FE mask with the command
  echo /afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py ${sqlfile} COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_PIXEL_W password
  echo Please, to clean up the directory, delete ${sqlfile} after the upload.
fi
rm $filename
