# echo "Setting TestProject TestProject-00-00-02 in /afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r20p20080222
endif
source ${CMTROOT}/mgr/setup.csh

set tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set tempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=TestProject -version=TestProject-00-00-02 -path=/afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive  -no_cleanup $* >${tempfile}; source ${tempfile}
/bin/rm -f ${tempfile}

