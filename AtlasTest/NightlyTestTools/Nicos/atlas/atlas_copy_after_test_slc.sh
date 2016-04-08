#!/bin/sh

dat=`date`
echo "Starting copy after test: $dat" 

quant=1200

df=10
NICOS_PROJECT_HOME="/build/atnight/localbuilds/nightlies/rel"
file_st=$NICOS_PROJECT_HOME/stamps/nicos_test_opt_complete

LIMIT=20
a=1

   while [ "$a" -le $LIMIT ]
   do
   echo -n "wait (copy_after_test) $a "
   let "a+=1"
if [ -f $file_st ]; then
df=`/afs/cern.ch/atlas/software/dist/nightlies/scripts/mtime_diff_prec $file_st`
fi
if [ "$df" -ge 5 ]; then
sleep $quant
fi
   done

cp -a $NICOS_RELHOME/*Log* $NICOS_COPY_HOME/$NICOS_PROJECT_RELNAME/.
cp -a $NICOS_RELHOME/*qmtest* $NICOS_COPY_HOME/$NICOS_PROJECT_RELNAME/.
cp -a $NICOS_RELHOME/InstallArea/doc $NICOS_COPY_HOME/$NICOS_PROJECT_RELNAME/InstallArea/.