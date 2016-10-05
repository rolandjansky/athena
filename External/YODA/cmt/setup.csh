# echo "setup YODA YODA-01-06-03 in /afs/cern.ch/user/e/ewelina/testarea/20.7.X-VAL/External"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtYODAtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtYODAtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=YODA -version=YODA-01-06-03 -path=/afs/cern.ch/user/e/ewelina/testarea/20.7.X-VAL/External  -quiet -without_version_directory -no_cleanup $* >${cmtYODAtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=YODA -version=YODA-01-06-03 -path=/afs/cern.ch/user/e/ewelina/testarea/20.7.X-VAL/External  -quiet -without_version_directory -no_cleanup $* >${cmtYODAtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtYODAtempfile}
  unset cmtYODAtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtYODAtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtYODAtempfile}
unset cmtYODAtempfile
exit $cmtsetupstatus

