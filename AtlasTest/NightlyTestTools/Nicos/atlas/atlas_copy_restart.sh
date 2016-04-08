#!/bin/sh

dat=`date`
echo "Starting copy after test: $dat" 

quant=1200

df=10
NICOS_PROJECT_HOME="/build/atnight/localbuilds/nightlies/rel"
NICOS_COPY_HOME="/afs/cern.ch/atlas/software/dist/nightlies/rel"
NICOS_PROJECT_RELNAME="atlrel_4"
cd $NICOS_PROJECT_HOME/$NICOS_PROJECT_RELNAME
objects="--exclude=*.o"
#export CVSIGNORE="cmt cmz *.make *.cmtref *.py InstallArea jobOptions"
rsync -rltvz ${objects} . $NICOS_COPY_HOME/$NICOS_PROJECT_RELNAME
/afs/cern.ch/user/a/alibrari/scripts/copy_release.sh -R -v -f $NICOS_PROJECT_HOME -t /afs/cern.ch/atlas/software/dist/nightlies/rel -r $NICOS_PROJECT_RELNAME -b "copy"
rm -f $file_st_1


