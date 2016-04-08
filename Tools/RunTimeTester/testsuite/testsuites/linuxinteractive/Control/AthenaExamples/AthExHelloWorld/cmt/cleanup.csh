if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20080222
endif
source ${CMTROOT}/mgr/setup.csh
set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=AthExHelloWorld -version=AthExHelloWorld-01-02-00 -path=/afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive/Control/AthenaExamples $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

