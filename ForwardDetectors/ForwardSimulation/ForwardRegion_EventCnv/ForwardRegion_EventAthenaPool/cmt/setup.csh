# echo "setup ForwardRegion_EventAthenaPool ForwardRegion_EventAthenaPool-00-00-05 in /afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc46-opt/17.7.0/CMT/v1r26
endif
source ${CMTROOT}/mgr/setup.csh
set cmtForwardRegion_EventAthenaPooltempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtForwardRegion_EventAthenaPooltempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=ForwardRegion_EventAthenaPool -version=ForwardRegion_EventAthenaPool-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -no_cleanup $* >${cmtForwardRegion_EventAthenaPooltempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=ForwardRegion_EventAthenaPool -version=ForwardRegion_EventAthenaPool-00-00-05 -path=/afs/cern.ch/user/t/tkeck/workspace/17.7.0/ForwardDetectors/ForwardSimulation/ForwardRegion_EventCnv  -no_cleanup $* >${cmtForwardRegion_EventAthenaPooltempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtForwardRegion_EventAthenaPooltempfile}
  unset cmtForwardRegion_EventAthenaPooltempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtForwardRegion_EventAthenaPooltempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtForwardRegion_EventAthenaPooltempfile}
unset cmtForwardRegion_EventAthenaPooltempfile
exit $cmtsetupstatus

