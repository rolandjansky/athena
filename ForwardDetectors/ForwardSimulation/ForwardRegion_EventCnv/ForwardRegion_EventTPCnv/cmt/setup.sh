# echo "setup ForwardRegion_EventTPCnv ForwardRegion_EventTPCnv-00-00-07 in /afs/cern.ch/user/g/gemmeren/workarea/dev/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtForwardRegion_EventTPCnvtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtForwardRegion_EventTPCnvtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-07 -path=/afs/cern.ch/user/g/gemmeren/workarea/dev/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=ForwardRegion_EventTPCnv -version=ForwardRegion_EventTPCnv-00-00-07 -path=/afs/cern.ch/user/g/gemmeren/workarea/dev/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -quiet -without_version_directory -no_cleanup $* >${cmtForwardRegion_EventTPCnvtempfile}"
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

