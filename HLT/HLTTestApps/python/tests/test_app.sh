#!/bin/bash 
# author Andre Anjos <andre.dos.anjos@cern.ch>
# author Ricardo Abreu <ricardo.abreu@cern.ch>

# Runs, in sequence, a few athenaMT.py tests

# temporary solution
DATADIR="/afs/cern.ch/work/r/ricab/datafiles/"
DATAFILE="--file $DATADIR/2012-05-04VALAllPT_physicsV4-1.only3events.data"
export PYTHONPATH="$DATADIR:$PYTHONPATH" # so that the robhit module is found

#DEBUG=-l 'DEBUG'
#DEBUG=-l 'INFO,ERROR'

i=1
function runtest() {
  echo "==============================================================="
  echo "$i) Options are [$*]"
  echo "==============================================================="
  echo 
  athenaHLT.py $*
  ret=$?
  echo
  echo "=========================================="
  echo "$i) The status output of this test is $ret"
  echo "=========================================="
  echo
  if [ $ret -ne 0 ]; then
    exit $ret
  else
    i=$((i+1))
  fi
}


# This test tries to perform a basic run of athenaHLT
runtest $DEBUG $DATAFILE -n5 TrigExMTHelloWorld/MTHelloWorldOptions.py
                         
# This test tries to run with --tcmalloc
# TODO reactivate once tcmalloc is available again (with TCMALLOCDIR env var
# automatically defined after an asetup)
#runtest $DEBUG $DATAFILE --tcmalloc TrigExMTHelloWorld/MTHelloWorldOptions.py

# This test tries to run with --leak-check-execute
runtest $DEBUG $DATAFILE --leak-check-execute \
                         TrigExMTHelloWorld/MTHelloWorldOptions.py

# This test tries to run with --tcmalloc and --leak-check-execute
# simultaneously, which mustn't be possible. A warning should be printed 
# informing the user that libc malloc is chosen instead
# TODO reactivate once tcmalloc is available again (with TCMALLOCDIR env var
# automatically defined after an asetup)
#runtest $DEBUG $DATAFILE --tcmalloc --leak-check-execute\
#                         TrigExMTHelloWorld/MTHelloWorldOptions.py

# try to apply the print_event_header plugin; force accept all events
runtest $DEBUG $DATAFILE --postcommand 'HltEventLoopMgr.ForceHltAccept=True'\
                         -Z 'HLTTestApps.plugins.print_event_header'\
                         TrigExMTHelloWorld/MTHelloWorldOptions.py

# try to apply the fill_empty plugin; force accept all events
runtest $DEBUG $DATAFILE --postcommand 'HltEventLoopMgr.ForceHltAccept=True'\
                         -Z 'HLTTestApps.plugins.fill_empty'\
                         TrigExMTHelloWorld/MTHelloWorldOptions.py

# try to apply the remove_nonlisted plugin; force accept all events
runtest $DEBUG $DATAFILE --postcommand 'HltEventLoopMgr.ForceHltAccept=True'\
                         -Z 'HLTTestApps.plugins.remove_nonlisted'\
                         TrigExMTHelloWorld/MTHelloWorldOptions.py

# try to apply the remove_nonlisted and truncate plugins; force accept all events
runtest $DEBUG $DATAFILE --postcommand 'HltEventLoopMgr.ForceHltAccept=True'\
                         -Z 'HLTTestApps.plugins.remove_nonlisted' \
                         -Z 'HLTTestApps.plugins.truncate'\
                         TrigExMTHelloWorld/MTHelloWorldOptions.py
                         
runtest $DEBUG $DATAFILE -n5 -M -W TrigExMTHelloWorld/MTHelloWorldOptions.py
                         
# try to configure from the TriggerDB with COOL provided DB connection params
# TODO use a run number indexed in cool to a workable config (only possible 
# when such a run exists)
#runtest $DEBUG $DATAFILE --use-database 

# try to configure from the TriggerDB with user provided DB connection params
#runtest $DEBUG $DATAFILE --use-database --db-type Coral \
#                         --db-server TRIGGERDBREPR --db-smkey 542 \
#                         --db-hltpskey 345 --db-extra #'{"lvl1key":"82","user":'\
#'"ignoreme","password":"ignoreme","schema":"ignoreme","retrialPeriod":"25",'\
#'"maxRetrials":"5","some_unrecognized_param":"gnfdliadfbg"}'
