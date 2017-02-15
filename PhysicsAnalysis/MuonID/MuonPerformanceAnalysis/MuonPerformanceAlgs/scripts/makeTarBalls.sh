#!/bin/bash

# generate tarballs for the main submit script
if  [ "niko" = $USER ]; then
	NAME="nkoehler"
else NAME=$USER
fi

pathena --trf "Reco_tf.py --inputAODFile=%IN --outputNTUP_MCPTPFile=%OUT.NTUP_MCPTP.root --outputNTUP_MCPScaleFile=%OUT.NTUP_MCScale.root" \
      --noSubmit \
      --inDS $1 \
      --outDS='group.perf-muons.'$NAME'.dummy.1' \
      --outTarBall=otb.tar\
      --outRunConfig=runconf.datatp \
      --official \
      --voms=atlas:/atlas/perf-muons/Role=production

