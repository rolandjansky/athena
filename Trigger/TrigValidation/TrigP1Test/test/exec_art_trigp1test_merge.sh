#!/bin/bash

echo  $(date "+%FT%H:%M %Z")"     Execute TrigP1Test merging processing"

# Merge log and monitoring files from children
for f in athenaHLT:*; do
  cat $f >> ${JOB_athenaHLT_LOG}
done

if [ -f expert-monitoring.root ]; then
  mv expert-monitoring.root expert-monitoring_mother.root
  hadd expert-monitoring.root athenaHLT_workers/*/*.root expert-monitoring_mother.root
fi

