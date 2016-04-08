#!/bin/sh
                                                                             
dat=`date`
echo "Starting copy after build: $dat"
                                                                             
quant=1200


#df=10
#NICOS_PROJECT_HOME="/build/atnight/localbuilds/nightlies/rel"
#file_st=$NICOS_PROJECT_HOME/stamps/nicos_build_opt_complete
#
#LIMIT=35
#a=1
#
#   while [ "$a" -le $LIMIT ]
#   do
#   echo -n "wait (copy_after_build) $a "
#   let "a+=1"
#if [ -f $file_st ]; then
#df=`/afs/cern.ch/atlas/software/dist/nightlies/scripts/mtime_diff_prec $file_st`
#fi
#if [ "$df" -ge 5 ]; then
#sleep $quant
#fi
#   done

#dat=`date`
#echo "Starting copy after build (delayed): $dat"
lifetime=7
export NICOS_HOUR=24
relnum=`${NICOS_HOME}/nicos_relnum_calcul.pl ${lifetime}`; stat=$?
if [ "$stat" -ne 0 ]; then
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "atlas_copy_after_build_lcl: nicos_relnum_calcul exited with non zero value"
echo " and returned: $relnum "
echo "lifetime parameter is set to 0"
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
lifetime=0;
relnum=0;
fi

PROJECT_RELNAME=${NICOS_PROJECT_RELNAME_GEN}_${relnum}
if [ "${relnum}" = "" ]; then
PROJECT_RELNAME=${NICOS_PROJECT_RELNAME_GEN}
fi

echo "copy is performed for platform i686-slc3-gcc323-dbg to release ${PROJECT_RELNAME}"

/afs/cern.ch/user/a/alibrari/scripts/copy_release.sh -R -v -f $NICOS_PROJECT_HOME -f_r atlrel_0 -p i686-slc3-gcc323-dbg -t /afs/cern.ch/atlas/software/dist/nightlies/rel -r ${PROJECT_RELNAME}

