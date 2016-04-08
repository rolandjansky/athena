#!/bin/bash

diff_files()
{
 if [ "$2" ]
 then
  v=`echo $AtlasVersion | tail -c2`
  packpath=`echo $1 | sed "s#/rel_$v/#/rel_$2/#g"`
  for file in *.pool.root
  do
   for ref in `grep -h "/$file<" $packpath/rttjobinfo.xml | sed "s/\(.\+\)>\(.\+\)<\(.\+\)/\2/g"`
   do
    if [[ $ref == /castor* ]]
    then
     ref="root://castoratlas/$ref"
    fi
    # echo $file $ref
    f=diff.$v.$2.txt
    diffPoolFiles.py $file $ref &> $f
   done
  done
 fi
} 

if [ "$1" ]
then
 v=`echo $AtlasVersion | tail -c2`
 for (( i=$v; i>=0; i-- ))
 do
   echo -n "comparing rel_$v to rel_$i : "
   diff_files $1 $i
   if [ -e diff.$v.$i.txt ]; then
    grep "## Comparison : " diff.$v.$i.txt | sed "s/## Comparison : //g"
   else
    echo "n/a"
   fi
 done
 for (( i=6; i>$v; i-- ))
 do
   echo -n "comparing rel_$v to rel_$i : "
   diff_files $1 $i
   if [ -e diff.$v.$i.txt ]; then
    grep "## Comparison : " diff.$v.$i.txt | sed "s/## Comparison : //g"
   else
    echo "n/a"
   fi
 done
fi

exit 0

