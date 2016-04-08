# echo "cleanup ForwardRegion_EventAthenaPool ForwardRegion_EventAthenaPool-00-00-05 in /afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc46-opt/17.7.0/CMT/v1r26; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtForwardRegion_EventAthenaPooltempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtForwardRegion_EventAthenaPooltempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=ForwardRegion_EventAthenaPool -version=ForwardRegion_EventAthenaPool-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  $* >${cmtForwardRegion_EventAthenaPooltempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=ForwardRegion_EventAthenaPool -version=ForwardRegion_EventAthenaPool-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  $* >${cmtForwardRegion_EventAthenaPooltempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtForwardRegion_EventAthenaPooltempfile}
  unset cmtForwardRegion_EventAthenaPooltempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtForwardRegion_EventAthenaPooltempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtForwardRegion_EventAthenaPooltempfile}
unset cmtForwardRegion_EventAthenaPooltempfile
return $cmtcleanupstatus

