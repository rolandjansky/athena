
#!/bin/bash

#///////////////////////////////////////////////////////////////
# Tag-and-probe grid submit script.
# Use: First generate tarballs using a dummy xAOD file via
# source makeTarBalls.sh <dummy xAOD>
# then submit jobs via source Submit.sh <Dataset or txt file containing many datasets>
# Parameters:
# SUFFIX will be appended to the end of the output DS. Default: TP_<Date>_v1
#///////////////////////////////////////////////////////////////

# configure the name of the output dataset:
SUFFIX="TP_"
DATE=`date +%Y%m%d`
SUFFIX+=$DATE
SUFFIX+="_v1"                  # change version name

if  [ "niko" = $USER ]; then
        NAME="nkoehler"
else NAME=$USER
fi
my_prun_nickname=$NAME		# enter cern username

if [ -e $1 ]
then list=$1
else list="dummy.txt";echo $1 > $list
fi

while read sample
  do
  # ds=$sample
  # Remove whitespace.. either line should do
  #  (the first removes leading whitespace, the second removes trailing whitespace)
  # ds="${sample#"${sample%%[![:space:]]*}"}"
  ds="${sample%"${sample##*[![:space:]]}"}"

  if [[ "${ds}" == "" ]]
      then
      continue
  fi

  # Ignore lines starting with a hash
  if [[ ${ds:0:1} == "#" ]]
      then
      continue
  fi
  echo submitting $sample...
  # 
  # Here, we clean up the DS name a bit to stay within the 13X character limit
  # Feel free to extend! 
  #
  dsorig=$ds
  dsorig=${ds/.merge.AOD/}
  dsorig=${dsorig/user.dzhang./}
  dsorig=${dsorig/_EXT0/}
  dsorig=${dsorig/group.perf-muons:group.perf-muons./}
  dsorig=${dsorig/data15_comm:data15_comm./}
  dsorig=${dsorig/data15_13TeV:data15_13TeV./}
  dsorig=${dsorig/mc15_13TeV:mc15_13TeV./}
  dsorig=${dsorig/_hdamp172p5/}
  dsorig=${dsorig/EvtGen_P2012/}
  dsorig=${dsorig/group.det-muon:group.det-muon./}
  dsorig=${dsorig/group.det-muon./}
  dsorig=${dsorig//s????_/}
  dsorig=${dsorig%\/}
  outname="user.$my_prun_nickname.$dsorig.$SUFFIX"
  # Submit using pathena.
  pathena MuonPerformanceAlgs/MuonTP_topOptions.py \
      --inDS $ds \
      --outDS $outname \
      --express \
      --mergeOutput \
      --useShortLivedReplicas \
      --site=AUTO \
      --inTarBall=otb.tar\
      --inRunConfig=runconf.datatp
  echo "output DS is " $outname/
done < "$list"
      #--nFilesPerJob=1 \
      #--noSubmit \


   #   --excludedSite=ANALY_IN2P3-CC_SL6,ANALY_IN2P3-CC \
