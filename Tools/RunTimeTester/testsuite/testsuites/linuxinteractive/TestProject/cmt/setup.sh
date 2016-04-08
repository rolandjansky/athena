# echo "Setting TestProject TestProject-00-00-02 in /afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20080222; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=TestProject -version=TestProject-00-00-02 -path=/afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

