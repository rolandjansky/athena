#!/bin/sh
                                                                             
dat=`date`
echo "atlas_merge_build: start at $dat"
                                                                             
echo "atlas_merge_build: performed for platform ${CMTCONFIG} from release ${NICOS_PROJECT_RELNAME} to release ${PROJECT_RELNAME}"

/afs/cern.ch/user/a/alibrari/scripts/copy_release.sh -R -v -f ${NICOS_PROJECT_HOME} -f_r ${NICOS_PROJECT_RELNAME} -p ${CMTCONFIG} -t ${NICOS_COPY_HOME} -r ${NICOS_PROJECT_RELNAME_COPY}

echo "atlas_merge_build: finish at $dat"
