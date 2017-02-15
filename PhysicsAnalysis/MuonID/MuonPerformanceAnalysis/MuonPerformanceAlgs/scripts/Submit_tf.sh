
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
# SUFFIX="TP"
# DATE=`date +%Y%m%d`
# SUFFIX+=$DATE
SUFFIX="_v030"                  # change version name

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

  dsorig=${dsorig/.merge.AOD/.NTUP_MCPTP}
  dsorig=${dsorig/.PhysCont.AOD/.NTUP_MCPTP}
  dsorig=${dsorig/.recon.AOD/recon.NTUP_MCPTP}
  dsorig=${dsorig/user.dzhang./}
  dsorig=${dsorig/_EXT0/}
  dsorig=${dsorig/group.perf-muons:group.perf-muons./}
  dsorig=${dsorig/data15_comm:data15_comm./}
  dsorig=${dsorig/data15_13TeV:data15_13TeV./data15_13TeV.}
  dsorig=${dsorig/mc15_13TeV:mc15_13TeV./mc15_13TeV.}
  dsorig=${dsorig/_hdamp172p5_nonallhad/}
  dsorig=${dsorig/EvtGen_P2012/}
  dsorig=${dsorig/group.det-muon:group.det-muon./}
  dsorig=${dsorig/group.det-muon./}
  dsorig=${dsorig/_AZNLOCTEQ6L1/}
  dsorig=${dsorig//s????_/}
  dsorig=${dsorig%\/}

  # Output name
  #outname="user.$my_prun_nickname.$dsorig.$SUFFIX"
  outname="group.perf-muons.${dsorig}${SUFFIX}"

  # Submit using pathena.
  pathena --trf "Reco_tf.py --inputAODFile=%IN --outputNTUP_MCPTPFile=%OUT.NTUP_MCPTP.root --outputNTUP_MCPScaleFile=%OUT.NTUP_MCScale.root" \
      --inDS $ds \
      --outDS $outname \
      --inTarBall=otb.tar \
      --inRunConfig=runconf.datatp \
      --destSE=MPPMU_PERF-MUONS \
      --official \
      --mergeOutput \
      --voms=atlas:/atlas/perf-muons/Role=production
      
  echo "output DS is " $outname/
done < "$list"
      #--nFilesPerJob=1 \
      #--noSubmit \


   #   --excludedSite=ANALY_IN2P3-CC_SL6,ANALY_IN2P3-CC \
