#!/usr/bin/env bash
## Run rootcomp.py on different run cycles of magField_on_off_on 

## Prepend script name to all our printouts
shopt -s expand_aliases
alias log='echo [`basename $0`] '

log "command: $0 $*"

if [ $# -ne 2 ]; then
   log "Syntax: `basename $0` reference"
   exit 1
fi

#get the different run cycle parameters 
#for run_0 (magnet on) 
#for run_2 (magnet on after being off)
run_0=$1
run_2=$2
rootcomp.py  --refBaseDir=run_0 --fileBaseDir=run_2  $run_0 $run_2 2> rootcomp_magField_on_off_on_stderr.log
status=$? 
log $status

if [ $status -eq 0 ]; then
    exit 0
else
    exit 1
fi
