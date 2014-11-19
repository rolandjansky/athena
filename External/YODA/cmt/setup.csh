# echo "setup YODA YODA-01-03-00-branch in /afs/cern.ch/user/e/ewelina/testarea/MIG6/External"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtYODAtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtYODAtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=YODA -version=YODA-01-03-00-branch -path=/afs/cern.ch/user/e/ewelina/testarea/MIG6/External  -quiet -without_version_directory -no_cleanup $* >${cmtYODAtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=YODA -version=YODA-01-03-00-branch -path=/afs/cern.ch/user/e/ewelina/testarea/MIG6/External  -quiet -without_version_directory -no_cleanup $* >${cmtYODAtempfile}"
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

