#!/usr/bin/env bash
## Compare the root files produced by athenaHLT when there is one stop/start

## Extract run number from typical oh_cp file name (r0000000000_part_...)
function runno {
    filename=`basename $1`
    n=`echo $filename | cut -d'_' -f1`
    echo ${n: -1}                  # only works for runs <= 9
}

## Prepend script name to all our printouts
shopt -s expand_aliases
alias log='echo [`basename $0`] '

if [ $# -ne 2 ]; then
   log "Syntax: `basename $0` file1 file2"
   exit 1
fi

base1="run_"`runno $1`
base2="run_"`runno $2`
rootcomp.py --zip --noRoot --refBaseDir=$base1 --fileBaseDir=$base2 $1 $2 2> rootcomp_stopstart_stderr.log

