# echo "cleanup EvgenProdTools EvgenProdTools-00-02-30 in /afs/cern.ch/user/e/ewelina/testarea/20.7.X.Y.Z-VAL/Generators"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.9/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtEvgenProdToolstempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtEvgenProdToolstempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=EvgenProdTools -version=EvgenProdTools-00-02-30 -path=/afs/cern.ch/user/e/ewelina/testarea/20.7.X.Y.Z-VAL/Generators  -quiet -without_version_directory $* >${cmtEvgenProdToolstempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=EvgenProdTools -version=EvgenProdTools-00-02-30 -path=/afs/cern.ch/user/e/ewelina/testarea/20.7.X.Y.Z-VAL/Generators  -quiet -without_version_directory $* >${cmtEvgenProdToolstempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtEvgenProdToolstempfile}
  unset cmtEvgenProdToolstempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtEvgenProdToolstempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtEvgenProdToolstempfile}
unset cmtEvgenProdToolstempfile
exit $cmtcleanupstatus

