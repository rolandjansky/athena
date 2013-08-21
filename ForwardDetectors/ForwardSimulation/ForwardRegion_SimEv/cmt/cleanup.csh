# echo "cleanup ForwardRegion_SimEv ForwardRegion_SimEv-00-00-05 in /afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtForwardRegion_SimEvtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtForwardRegion_SimEvtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=ForwardRegion_SimEv -version=ForwardRegion_SimEv-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation  $* >${cmtForwardRegion_SimEvtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=ForwardRegion_SimEv -version=ForwardRegion_SimEv-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation  $* >${cmtForwardRegion_SimEvtempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtForwardRegion_SimEvtempfile}
  unset cmtForwardRegion_SimEvtempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtForwardRegion_SimEvtempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtForwardRegion_SimEvtempfile}
unset cmtForwardRegion_SimEvtempfile
exit $cmtcleanupstatus

