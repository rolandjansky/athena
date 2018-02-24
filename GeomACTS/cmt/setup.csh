# echo "setup GeomACTS  in /home/pagessin/dev/geomodel/20.20"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/cern.ch/sw/contrib/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtGeomACTStempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtGeomACTStempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=GeomACTS -version= -path=/home/pagessin/dev/geomodel/20.20  -no_cleanup $* >${cmtGeomACTStempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=GeomACTS -version= -path=/home/pagessin/dev/geomodel/20.20  -no_cleanup $* >${cmtGeomACTStempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtGeomACTStempfile}
  unset cmtGeomACTStempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtGeomACTStempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtGeomACTStempfile}
unset cmtGeomACTStempfile
exit $cmtsetupstatus

