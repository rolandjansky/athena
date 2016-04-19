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
runtest $DEBUG $DATAFILE --tcmalloc TrigExMTHelloWorld/MTHelloWorldOptions.py

# This test tries to run with --leak-check-execute
runtest $DEBUG $DATAFILE --leak-check-execute \
                         TrigExMTHelloWorld/MTHelloWorldOptions.py

# This test tries to run with --tcmalloc and --leak-check-execute
# simultaneously, which mustn't be possible. A warning should be printed 
# informing the user that libc malloc is chosen instead
runtest $DEBUG $DATAFILE --tcmalloc --leak-check-execute\
                         TrigExMTHelloWorld/MTHelloWorldOptions.py

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
