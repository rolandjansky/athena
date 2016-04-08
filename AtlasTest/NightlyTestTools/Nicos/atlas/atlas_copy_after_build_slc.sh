#!/bin/sh
                                                                             
dat=`date`
echo "Starting copy after build: $dat"
                                                                             
quant=1200


df=10
NICOS_PROJECT_HOME="/build/atnight/localbuilds/nightlies/rel"
file_st=$NICOS_PROJECT_HOME/stamps/nicos_build_opt_complete

LIMIT=30
a=1

   while [ "$a" -le $LIMIT ]
   do
   echo -n "wait (copy_after_build) $a "
   let "a+=1"
if [ -f $file_st ]; then
df=`/afs/cern.ch/atlas/software/dist/nightlies/scripts/mtime_diff_prec $file_st`
fi
if [ "$df" -ge 5 ]; then
sleep $quant
fi
   done

dat=`date`
echo "Starting copy after build (delayed): $dat"
                                                                             
rm -rf /afs/cern.ch/atlas/software/dist/nightlies/slc/$NICOS_PROJECT_RELNAME/*
/afs/cern.ch/user/a/alibrari/scripts/copy_release.sh -R -v -f $NICOS_PROJECT_HOME -t /afs/cern.ch/atlas/software/dist/nightlies/slc -r $NICOS_PROJECT_RELNAME

/afs/cern.ch/atlas/scripts/fixcvsroot -f -v /afs/cern.ch/atlas/software/dist/nightlies/slc/$NICOS_PROJECT_RELNAME

cd /afs/cern.ch/atlas/software/dist/nightlies/slc/$NICOS_PROJECT_RELNAME/AtlasRelease/*/cmt
export CMTPATH=/afs/cern.ch/atlas/software/dist/nightlies/slc/$NICOS_PROJECT_RELNAME
cmt bro cmt config

