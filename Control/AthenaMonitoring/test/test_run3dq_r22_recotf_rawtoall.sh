#!/bin/bash
# art-description: new DQ in Reco_tf, RAWtoALL, data q431
# art-type: grid
# art-memory: 6144
# art-include: master/Athena
# art-output: myHIST.root
# art-output: log*

Reco_tf.py --athenaopts='--threads=1' --AMI=q431 --preExec="DQMonFlags.doNewMonitoring=True;DQMonFlags.doHLTMon=False" --steering doRAWtoALL > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"
rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/myHIST.root)
hist_diff.sh myHIST.root $REFFILE -x TIME_execute -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
