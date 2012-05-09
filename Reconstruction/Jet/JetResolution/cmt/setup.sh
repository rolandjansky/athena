# echo "setup JetResolution JetResolution-r482618 in /afs/cern.ch/user/d/delsart/scratch0/reviseTrackSelection/Reconstruction/Jet"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r24; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtJetResolutiontempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtJetResolutiontempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=JetResolution -version=JetResolution-r482618 -path=/afs/cern.ch/user/d/delsart/scratch0/reviseTrackSelection/Reconstruction/Jet  -quiet -without_version_directory -no_cleanup $* >${cmtJetResolutiontempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=JetResolution -version=JetResolution-r482618 -path=/afs/cern.ch/user/d/delsart/scratch0/reviseTrackSelection/Reconstruction/Jet  -quiet -without_version_directory -no_cleanup $* >${cmtJetResolutiontempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtJetResolutiontempfile}
  unset cmtJetResolutiontempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtJetResolutiontempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtJetResolutiontempfile}
unset cmtJetResolutiontempfile
return $cmtsetupstatus

