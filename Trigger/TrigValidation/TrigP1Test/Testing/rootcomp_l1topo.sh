#!/usr/bin/env bash
## Run rootcomp.py restricted to L1Topo histograms

## Prepend script name to all our printouts
shopt -s expand_aliases
alias log='echo [`basename $0`] '

log "command: $0 $*"

if [ $# -ne 1 ]; then
   log "Syntax: `basename $0` reference"
   exit 1
fi

ref=$1

rootcomp.py --noRoot --zip --select 'L1TopoROBMonitor' $ref expert-monitoring.root 2> rootcomp_l1topo_stderr.log
status=$? 
log $status

if [ $status -eq 0 ]; then
    exit 0
else
    exit 1
fi
