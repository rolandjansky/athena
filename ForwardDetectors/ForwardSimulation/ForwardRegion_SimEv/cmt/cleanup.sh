# echo "cleanup ForwardRegion_SimEv ForwardRegion_SimEv-00-00-05 in /afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtForwardRegion_SimEvtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtForwardRegion_SimEvtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=ForwardRegion_SimEv -version=ForwardRegion_SimEv-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation  $* >${cmtForwardRegion_SimEvtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=ForwardRegion_SimEv -version=ForwardRegion_SimEv-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation  $* >${cmtForwardRegion_SimEvtempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtForwardRegion_SimEvtempfile}
  unset cmtForwardRegion_SimEvtempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtForwardRegion_SimEvtempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtForwardRegion_SimEvtempfile}
unset cmtForwardRegion_SimEvtempfile
return $cmtcleanupstatus

