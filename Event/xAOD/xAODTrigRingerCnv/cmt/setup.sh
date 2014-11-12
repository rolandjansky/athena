# echo "setup xAODTrigRingerCnv xAODTrigRingerCnv-00-00-01 in /tmp/jodafons/testRinger/test2/Event/xAOD"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtxAODTrigRingerCnvtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtxAODTrigRingerCnvtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=xAODTrigRingerCnv -version=xAODTrigRingerCnv-00-00-01 -path=/tmp/jodafons/testRinger/test2/Event/xAOD  -no_cleanup $* >${cmtxAODTrigRingerCnvtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=xAODTrigRingerCnv -version=xAODTrigRingerCnv-00-00-01 -path=/tmp/jodafons/testRinger/test2/Event/xAOD  -no_cleanup $* >${cmtxAODTrigRingerCnvtempfile}"
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

