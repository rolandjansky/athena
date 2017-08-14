# echo "setup InDetVertexValidation InDetVertexValidation-00-00-04 in /afs/cern.ch/work/g/glee/svn/InnerDetector/InDetValidation"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.20.10/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtInDetVertexValidationtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtInDetVertexValidationtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=InDetVertexValidation -version=InDetVertexValidation-00-00-04 -path=/afs/cern.ch/work/g/glee/svn/InnerDetector/InDetValidation  -no_cleanup $* >${cmtInDetVertexValidationtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=InDetVertexValidation -version=InDetVertexValidation-00-00-04 -path=/afs/cern.ch/work/g/glee/svn/InnerDetector/InDetValidation  -no_cleanup $* >${cmtInDetVertexValidationtempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtInDetVertexValidationtempfile}
  unset cmtInDetVertexValidationtempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtInDetVertexValidationtempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtInDetVertexValidationtempfile}
unset cmtInDetVertexValidationtempfile
return $cmtsetupstatus

