# echo "setup GeomACTS  in /home/pagessin/dev/geomodel/20.20"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/cern.ch/sw/contrib/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtGeomACTStempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtGeomACTStempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=GeomACTS -version= -path=/home/pagessin/dev/geomodel/20.20  -no_cleanup $* >${cmtGeomACTStempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=GeomACTS -version= -path=/home/pagessin/dev/geomodel/20.20  -no_cleanup $* >${cmtGeomACTStempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtGeomACTStempfile}
  unset cmtGeomACTStempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtGeomACTStempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtGeomACTStempfile}
unset cmtGeomACTStempfile
return $cmtsetupstatus

