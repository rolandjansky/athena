# echo "cleanup SCTExtension SCTExtension-r652903 in /afs/cern.ch/user/v/vcairo/WorkVMM/Material/testPackage_20/InnerDetector/InDetPerformance"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.0.0/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtSCTExtensiontempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtSCTExtensiontempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=SCTExtension -version=SCTExtension-r652903 -path=/afs/cern.ch/user/v/vcairo/WorkVMM/Material/testPackage_20/InnerDetector/InDetPerformance  -quiet -without_version_directory $* >${cmtSCTExtensiontempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=SCTExtension -version=SCTExtension-r652903 -path=/afs/cern.ch/user/v/vcairo/WorkVMM/Material/testPackage_20/InnerDetector/InDetPerformance  -quiet -without_version_directory $* >${cmtSCTExtensiontempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtSCTExtensiontempfile}
  unset cmtSCTExtensiontempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtSCTExtensiontempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtSCTExtensiontempfile}
unset cmtSCTExtensiontempfile
return $cmtcleanupstatus

