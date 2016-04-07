#!/bin/sh

# prints local files on EOS. Youll be prompted for your VOMS password even if you set it up already; no check 

source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
localSetupRucioClients
voms-proxy-init -voms atlas
rucio list-file-replicas $1 | cut -d ":" -f 5 | cut -d "/" -f 2- | sed 's/^/\//'
