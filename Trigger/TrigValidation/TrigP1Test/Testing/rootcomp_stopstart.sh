#!/usr/bin/env bash
## Compare the root files produced by athenaHLT when there is one stop/start

## Prepend script name to all our printouts
shopt -s expand_aliases
alias log='echo [`basename $0`] '

log "command: $0 $*"

if [ $# -ne 2 ]; then
   log "Syntax: `basename $0` file1 file2"
   exit 1
fi

log "comparing files $1 $2..."

rootcomp.py --zip --noRoot --refBaseDir=run_0 --fileBaseDir=run_1 $1 $2 2> rootcomp_stopstart_stderr.log

status=$? 
log $status

if [ $status -eq 0 ]; then
    exit 0
else
    exit 1
fi
