# echo "setup ForwardRegion_EventTPCnv ForwardRegion_EventTPCnv-00-00-07 in /afs/cern.ch/user/g/gemmeren/workarea/dev/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtForwardRegion_EventTPCnvtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtForwardRegion_EventTPCnvtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-07 -path=/afs/cern.ch/user/g/gemmeren/workarea/dev/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-07 -path=/afs/cern.ch/user/g/gemmeren/workarea/dev/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtForwardRegion_EventTPCnvtempfile}
  unset cmtForwardRegion_EventTPCnvtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtForwardRegion_EventTPCnvtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtForwardRegion_EventTPCnvtempfile}
unset cmtForwardRegion_EventTPCnvtempfile
exit $cmtsetupstatus

