#!/usr/bin/env bash

# General setup for cron jobs

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
alias asetup='source $AtlasSetup/scripts/asetup.sh'
source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh --input  /afs/cern.ch/user/a/atlidbs/cron/asetup.cron
export CMDSETUP='source /afs/cern.ch/atlas/software/dist/AtlasSetup/scripts/asetup.sh --input  /afs/cern.ch/user/a/atlidbs/cron/asetup.cron'

export TASKDB='auth_file:/afs/cern.ch/user/a/atlidbs/private/beamspotdbinfo_w.dat'

# Some debugging output in case of network problems
echo "------------------------------"
echo "Host name = ${HOSTNAME}"
echo "------------------------------"
echo "/usr/kerberos/bin/klist:"
#/usr/kerberos/bin/klist
/usr/bin/klist
echo "------------------------------"
echo "/usr/kerberos/bin/kvno host/lxadm10.cern.ch"
/usr/bin/kvno host/lxadm10.cern.ch
echo "------------------------------"
echo "/usr/sue/bin/tokens"
/usr/bin/tokens
echo "------------------------------"
echo "/usr/bin/id"
/usr/bin/id
echo "------------------------------"

