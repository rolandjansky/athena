# echo "cleanup xAODTrigRinger xAODTrigRinger-00-00-05 in /afs/cern.ch/user/j/jodafons/workspace_tbed/trunk/workspace_ringer_19.1.3.7.svn/Event/xAOD"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtxAODTrigRingertempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtxAODTrigRingertempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=xAODTrigRinger -version=xAODTrigRinger-00-00-05 -path=/afs/cern.ch/user/j/jodafons/workspace_tbed/trunk/workspace_ringer_19.1.3.7.svn/Event/xAOD  -quiet -without_version_directory $* >${cmtxAODTrigRingertempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=xAODTrigRinger -version=xAODTrigRinger-00-00-05 -path=/afs/cern.ch/user/j/jodafons/workspace_tbed/trunk/workspace_ringer_19.1.3.7.svn/Event/xAOD  -quiet -without_version_directory $* >${cmtxAODTrigRingertempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtxAODTrigRingertempfile}
  unset cmtxAODTrigRingertempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtxAODTrigRingertempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtxAODTrigRingertempfile}
unset cmtxAODTrigRingertempfile
return $cmtcleanupstatus

