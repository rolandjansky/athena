#!/bin/bash

events=100000

if [ "$1"a = "cleana" ]
then
  rm joboption_*
else
  for((run=1;run<=10;run++))
  do
    file=joboption_"$run".py

    cat part1.py > $file
    echo "theApp.EvtMax = $events" >> $file
    echo "svcMgr.EventSelector.RunNumber = $run" >> $file
    cat part2.py >>$file
 
    pathena $file --outDS user.soumya.Afterburner_p_pb.3.run_"$run"_events_"$events" --nEventsPerJob $events --athenaTag 17.6.0 --site ANALY_CERN_XROOTD
  done
fi
