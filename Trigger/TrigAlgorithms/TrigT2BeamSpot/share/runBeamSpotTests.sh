#!/bin/bash


echo 
echo "We will now be running several jobs for testing the beam spot code"
echo

homeDir=/afs/cern.ch/user/f/fizisist
baseDir=/u1/fizisist/beamspot
logDir=${baseDir}
jobOptions=${baseDir}/myTrigger_jobOptions.py
dataList="gashalo J1 J3 mixed shift"    # Data
trackList="SiTrack IdScan"              # TrackType
optsList="defaults.v8"                  # SiTrackOpt

#source ${homeDir}/work/14.0.0/cmt/setup.sh -tag=14.0.0,32

cd ${baseDir}  

for datatype in ${dataList}; do
  for trktype in ${trackList}; do
    for cutstype in ${optsList}; do
      outDir=${baseDir}/${datatype}/${trktype}/${cutstype}
      if [ ! -d ${outDir} ]; then
        mkdir ${outDir}
      fi
      cd ${outDir}
      echo "*** ${trktype} (SiTrack=${cutstype}) on ${datatype} events"
    #echo "athena.py -c EvtMax=$1;Data='${datatype}';TrackType='${trktype}';SiTrackOpt='${cutstype}'; ${jobOptions} >& ${logDir}/beamspot.${datatype}.$1evt.${trktype}.${cutstype}.log & "
      athena.py -c "EvtMax=$1;Data='${datatype}';TrackType='${trktype}';SiTrackOpt='${cutstype}';" ${jobOptions} >& ${logDir}/beamspot.b18.${datatype}.$1evt.${trktype}.${cutstype}.log & 
    done
  done
done  
    
echo

cd ${baseDir}

