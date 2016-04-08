#!/bin/bash

# generate tarballs for the main submit script
if  [ "niko" = $USER ]; then
	NAME="nkoehler"
else NAME=$USER
fi

pathena -c "FNAME='$1'" MuonPerformanceAlgs/MuonTP_topOptions.py \
      --dbRelease=LATEST \
      --outDS='user.'$NAME'.dummy.1' \
      --outTarBall=otb.tar\
      --outRunConfig=runconf.datatp \
      --mergeOutput \
      --nGBPerJob=MAX \
      --noSubmit
