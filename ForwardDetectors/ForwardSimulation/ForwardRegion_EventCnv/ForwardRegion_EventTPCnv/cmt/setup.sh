# echo "setup ForwardRegion_EventTPCnv ForwardRegion_EventTPCnv-00-00-05 in /home/chytka/testarea/18.9.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv"

if test "${CMTROOT}" = ""; then
  CMTROOT=/athena/18.9.0/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtForwardRegion_EventTPCnvtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtForwardRegion_EventTPCnvtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-05 -path=/home/chytka/testarea/18.9.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-05 -path=/home/chytka/testarea/18.9.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtForwardRegion_EventTPCnvtempfile}
  unset cmtForwardRegion_EventTPCnvtempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtForwardRegion_EventTPCnvtempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtForwardRegion_EventTPCnvtempfile}
unset cmtForwardRegion_EventTPCnvtempfile
return $cmtsetupstatus

