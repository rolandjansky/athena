# echo "Setting AthExHelloWorld AthExHelloWorld-01-02-00 in /afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive/Control/AthenaExamples"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r20p20080222; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh

tempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then tempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=AthExHelloWorld -version=AthExHelloWorld-01-02-00 -path=/afs/cern.ch/atlas/project/RTT/data/brinick/TestFrameWork2/testsuites/linuxinteractive/Control/AthenaExamples  -no_cleanup $* >${tempfile}; . ${tempfile}
/bin/rm -f ${tempfile}

