# echo "setup YODA YODA-r689608 in /afs/cern.ch/user/e/ewelina/testarea/19.2.X.Y-VAL/External"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.2.4/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtYODAtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtYODAtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=YODA -version=YODA-r689608 -path=/afs/cern.ch/user/e/ewelina/testarea/19.2.X.Y-VAL/External  -quiet -without_version_directory -no_cleanup $* >${cmtYODAtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=YODA -version=YODA-r689608 -path=/afs/cern.ch/user/e/ewelina/testarea/19.2.X.Y-VAL/External  -quiet -without_version_directory -no_cleanup $* >${cmtYODAtempfile}"
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

