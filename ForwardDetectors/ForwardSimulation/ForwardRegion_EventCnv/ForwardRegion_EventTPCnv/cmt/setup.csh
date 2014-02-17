# echo "setup ForwardRegion_EventTPCnv ForwardRegion_EventTPCnv-00-00-05 in /home/chytka/testarea/18.9.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /athena/18.9.0/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtForwardRegion_EventTPCnvtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtForwardRegion_EventTPCnvtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-05 -path=/home/chytka/testarea/18.9.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-05 -path=/home/chytka/testarea/18.9.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}"
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

