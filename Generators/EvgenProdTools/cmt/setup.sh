# echo "setup EvgenProdTools EvgenProdTools-00-02-30 in /afs/cern.ch/user/e/ewelina/testarea/20.7.X.Y.Z-VAL/Generators"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.9/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtEvgenProdToolstempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtEvgenProdToolstempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=EvgenProdTools -version=EvgenProdTools-00-02-30 -path=/afs/cern.ch/user/e/ewelina/testarea/20.7.X.Y.Z-VAL/Generators  -quiet -without_version_directory -no_cleanup $* >${cmtEvgenProdToolstempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=EvgenProdTools -version=EvgenProdTools-00-02-30 -path=/afs/cern.ch/user/e/ewelina/testarea/20.7.X.Y.Z-VAL/Generators  -quiet -without_version_directory -no_cleanup $* >${cmtEvgenProdToolstempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtEvgenProdToolstempfile}
  unset cmtEvgenProdToolstempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtEvgenProdToolstempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtEvgenProdToolstempfile}
unset cmtEvgenProdToolstempfile
return $cmtsetupstatus

