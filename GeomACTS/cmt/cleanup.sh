# echo "cleanup GeomACTS  in /home/pagessin/dev/geomodel/20.20"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtGeomACTStempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtGeomACTStempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=GeomACTS -version= -path=/home/pagessin/dev/geomodel/20.20  $* >${cmtGeomACTStempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=GeomACTS -version= -path=/home/pagessin/dev/geomodel/20.20  $* >${cmtGeomACTStempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtGeomACTStempfile}
  unset cmtGeomACTStempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtGeomACTStempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtGeomACTStempfile}
unset cmtGeomACTStempfile
return $cmtcleanupstatus

