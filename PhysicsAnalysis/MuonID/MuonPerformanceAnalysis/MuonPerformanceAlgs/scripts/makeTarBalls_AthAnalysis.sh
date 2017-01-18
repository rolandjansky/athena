#!/bin/bash

# generate tarballs for the main submit script
if  [ "niko" = $USER ]; then
	NAME="nkoehler"
else NAME=$USER
fi

pathena --trf "athena -c \"FNAME='%IN'\" MuonPerformanceAlgs/CombinedRun_topOptions.py" \
      --noSubmit \
      --inDS $1 \
      --extOutFile='muontp.root,muonscale.root' \
      --outDS='group.perf-muons.'$NAME'.dummy.1' \
      --outTarBall=otb.tar\
      --outRunConfig=runconf.datatp \
      --official \
      --voms=atlas:/atlas/perf-muons/Role=production

