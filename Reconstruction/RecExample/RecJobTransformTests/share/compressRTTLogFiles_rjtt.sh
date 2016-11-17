#!/bin/bash

for i in $@
do
 echo compressing $i
 if [[ -e $i ]]
 then
  tail -n 300 $i > $i.tail300_log
  bzip2 $i
 fi
done
