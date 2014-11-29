# echo "cleanup xAODTrigRinger xAODTrigRinger-00-00-05 in /afs/cern.ch/user/j/jodafons/workspace_tbed/trunk/workspace_ringer_19.1.3.7.svn/Event/xAOD"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtxAODTrigRingertempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtxAODTrigRingertempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=xAODTrigRinger -version=xAODTrigRinger-00-00-05 -path=/afs/cern.ch/user/j/jodafons/workspace_tbed/trunk/workspace_ringer_19.1.3.7.svn/Event/xAOD  -quiet -without_version_directory $* >${cmtxAODTrigRingertempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=xAODTrigRinger -version=xAODTrigRinger-00-00-05 -path=/afs/cern.ch/user/j/jodafons/workspace_tbed/trunk/workspace_ringer_19.1.3.7.svn/Event/xAOD  -quiet -without_version_directory $* >${cmtxAODTrigRingertempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtxAODTrigRingertempfile}
  unset cmtxAODTrigRingertempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtxAODTrigRingertempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtxAODTrigRingertempfile}
unset cmtxAODTrigRingertempfile
exit $cmtcleanupstatus

