#!/bin/sh

dat=`date`
echo "Starting copy after test: $dat" 

quant=1200

df=10
NICOS_PROJECT_HOME="/build/atnight/localbuilds/nightlies/rel"
file_st=$NICOS_PROJECT_HOME/stamps/nicos_test_opt_complete
file_st_1=$NICOS_PROJECT_HOME/stamps/nicos_copy_after_build_complete

LIMIT=40
a=1

   while [ "$a" -le $LIMIT ]
   do
   echo -n "wait (copy_after_test) $a "
   let "a+=1"
if [ -f $file_st ]; then
if [ -f $file_st_1 ]; then
df=`/afs/cern.ch/atlas/software/dist/nightlies/scripts/mtime_diff_prec $file_st`
fi
fi
if [ "$df" -ge 5 ]; then
sleep $quant
fi
   done

dat=`date`
echo "Starting copy after test: $dat"

#Copy only if there is a distinctive sign that copy after build complete
if [ -f $file_st_1 ]; then
objects="--exclude=*.o"
cd $NICOS_RELHOME
export CVSIGNORE="cmt cmz setup.* cleanup.* *.make *.cmtref *.py InstallArea jobOptions"
rsync -rCltvz . $NICOS_COPY_HOME/$NICOS_PROJECT_RELNAME
/afs/cern.ch/user/a/alibrari/scripts/copy_release.sh -R -v -f $NICOS_PROJECT_HOME -t /afs/cern.ch/atlas/software/dist/nightlies/rel -r $NICOS_PROJECT_RELNAME -b "copy cvsroot config"
rm -f $file_st_1
fi

