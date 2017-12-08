# echo "setup xAODTrigRingerCnv xAODTrigRingerCnv-r655703 in /afs/cern.ch/user/j/jodafons/workspace_ringer_bundle/Event/xAOD"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.0/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtxAODTrigRingerCnvtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtxAODTrigRingerCnvtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=xAODTrigRingerCnv -version=xAODTrigRingerCnv-r655703 -path=/afs/cern.ch/user/j/jodafons/workspace_ringer_bundle/Event/xAOD  -quiet -without_version_directory -no_cleanup $* >${cmtxAODTrigRingerCnvtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=xAODTrigRingerCnv -version=xAODTrigRingerCnv-r655703 -path=/afs/cern.ch/user/j/jodafons/workspace_ringer_bundle/Event/xAOD  -quiet -without_version_directory -no_cleanup $* >${cmtxAODTrigRingerCnvtempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtxAODTrigRingerCnvtempfile}
  unset cmtxAODTrigRingerCnvtempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtxAODTrigRingerCnvtempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtxAODTrigRingerCnvtempfile}
unset cmtxAODTrigRingerCnvtempfile
return $cmtsetupstatus

