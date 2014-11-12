# echo "setup xAODTrigRingerAthenaPool xAODTrigRingerAthenaPool-00-00-01 in /tmp/jodafons/testRinger/test2/Event/xAOD"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.3/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtxAODTrigRingerAthenaPooltempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtxAODTrigRingerAthenaPooltempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=xAODTrigRingerAthenaPool -version=xAODTrigRingerAthenaPool-00-00-01 -path=/tmp/jodafons/testRinger/test2/Event/xAOD  -no_cleanup $* >${cmtxAODTrigRingerAthenaPooltempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=xAODTrigRingerAthenaPool -version=xAODTrigRingerAthenaPool-00-00-01 -path=/tmp/jodafons/testRinger/test2/Event/xAOD  -no_cleanup $* >${cmtxAODTrigRingerAthenaPooltempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtxAODTrigRingerAthenaPooltempfile}
  unset cmtxAODTrigRingerAthenaPooltempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtxAODTrigRingerAthenaPooltempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtxAODTrigRingerAthenaPooltempfile}
unset cmtxAODTrigRingerAthenaPooltempfile
exit $cmtsetupstatus

