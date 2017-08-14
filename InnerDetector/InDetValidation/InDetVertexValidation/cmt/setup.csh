# echo "setup InDetVertexValidation InDetVertexValidation-00-00-04 in /afs/cern.ch/work/g/glee/svn/InnerDetector/InDetValidation"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.20.10/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtInDetVertexValidationtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtInDetVertexValidationtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=InDetVertexValidation -version=InDetVertexValidation-00-00-04 -path=/afs/cern.ch/work/g/glee/svn/InnerDetector/InDetValidation  -no_cleanup $* >${cmtInDetVertexValidationtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=InDetVertexValidation -version=InDetVertexValidation-00-00-04 -path=/afs/cern.ch/work/g/glee/svn/InnerDetector/InDetValidation  -no_cleanup $* >${cmtInDetVertexValidationtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtInDetVertexValidationtempfile}
  unset cmtInDetVertexValidationtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtInDetVertexValidationtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtInDetVertexValidationtempfile}
unset cmtInDetVertexValidationtempfile
exit $cmtsetupstatus

