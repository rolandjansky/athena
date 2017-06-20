#!/bin/sh

FILE=$1
OUTPUT=collisions_run.config
FRAGMENT=idtrigger.fragment

[ -z $FILE ] && exit 1

hcg $FILE --dir HLT/TRIDT -o $OUTPUT -rc $FRAGMENT --algorithm "HLTid_Histogram_Not_Empty_with_Ref&GatherData" 

