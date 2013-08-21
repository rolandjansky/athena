# echo "setup ForwardRegion_SimEv ForwardRegion_SimEv-00-00-05 in /afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtForwardRegion_SimEvtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtForwardRegion_SimEvtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=ForwardRegion_SimEv -version=ForwardRegion_SimEv-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation  -no_cleanup $* >${cmtForwardRegion_SimEvtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=ForwardRegion_SimEv -version=ForwardRegion_SimEv-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation  -no_cleanup $* >${cmtForwardRegion_SimEvtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtForwardRegion_SimEvtempfile}
  unset cmtForwardRegion_SimEvtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtForwardRegion_SimEvtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtForwardRegion_SimEvtempfile}
unset cmtForwardRegion_SimEvtempfile
exit $cmtsetupstatus

