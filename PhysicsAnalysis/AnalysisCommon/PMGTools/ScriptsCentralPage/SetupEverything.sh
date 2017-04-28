#!/bin/bash
 
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh --quiet

localSetupPyAMI

PROXY=/tmp/x509up_u`id -u`
voms-proxy-init -valid 720:00 -out $PROXY

export MYPROXY_SERVER=myproxy.cern.ch
myproxy-init -d -n -c 900 --voms atlas

chmod 400 $PROXY
