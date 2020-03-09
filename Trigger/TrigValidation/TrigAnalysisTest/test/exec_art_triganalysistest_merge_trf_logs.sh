#!/bin/bash
#
# Merge transform logs for post-processing and prepare for RegTest comparison
#

trfNames=$1
echo $(date "+%FT%H:%M %Z")"     Execute TrigAnalysisTest trf log merging for steps: ${trfNames}"

echo "### ${JOB_LOG} ###" | tee athena.merged.log
cat ${JOB_LOG} >> athena.merged.log
for trf in ${trfNames}; do
  if [ -f log.${trf} ]; then
    echo "### log.${trf} ###" | tee -a athena.merged.log
    # cut the first 9 characters to remove timestamps from each message (needed for RegTest)
    cut -c 10- log.${trf} >> athena.merged.log
    # add trf name to RegTest exclude patterns to avoid matching to remaining timestamped lines
    if [ -z "${REGTESTEXP_EXCLUDE}" ]; then
      export REGTESTEXP_EXCLUDE="^${trf}.*"
    else
      export REGTESTEXP_EXCLUDE="${REGTESTEXP_EXCLUDE}|^${trf}.*"
    fi
  else
    echo "### WARNING: log.${trf} MISSING ###" | tee -a athena.merged.log
  fi
done
export JOB_LOG="athena.merged.log"
