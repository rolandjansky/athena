#!/bin/bash

# General setup for cron jobs:
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'
asetup="/afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh"
asetup_cfg="/afs/cern.ch/user/a/atlidbs/cron/asetup.cron"
source "$asetup" --input "$asetup_cfg"
# We need the AtlasProject to get the platform:
project="$(eval "echo \$${AtlasProject}_PLATFORM")"
project="${project:-$CMTCONFIG}"
csetup="/afs/cern.ch/user/a/atlidbs/cron/build/${project}/setup.sh"
# The cmake setup.sh gives us the built version of the IBDS package:
source "$csetup"
# This variable is used in jobs submitted by JobRunner:
export CMDSETUP="source '$asetup' --input '$asetup_cfg' && source '$csetup'"
unset asetup asetup_cfg project csetup

# Environment:
export TASKDB='auth_file:/afs/cern.ch/user/a/atlidbs/private/beamspotdbinfo_w.dat'

# Debugging output (in case of network problems):
echo '------------------------------'
echo "Host name = ${HOSTNAME}"
echo '------------------------------'
echo '/usr/bin/klist:'
/usr/bin/klist
echo '------------------------------'
echo '/usr/bin/kvno host/lxadm10.cern.ch'
/usr/bin/kvno host/lxadm10.cern.ch
echo '------------------------------'
echo '/usr/bin/tokens'
/usr/bin/tokens
echo '------------------------------'
echo '/usr/bin/id'
/usr/bin/id
echo '------------------------------'
echo 'Executables:'
which beamspotman.py
which beamspotnt.py
which taskman.py
echo '------------------------------'
