# echo "setup xAODTrigRingerCnv xAODTrigRingerCnv-r655703 in /afs/cern.ch/user/j/jodafons/workspace_ringer_bundle/Event/xAOD"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc48-opt/20.1.0/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtxAODTrigRingerCnvtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtxAODTrigRingerCnvtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=xAODTrigRingerCnv -version=xAODTrigRingerCnv-r655703 -path=/afs/cern.ch/user/j/jodafons/workspace_ringer_bundle/Event/xAOD  -quiet -without_version_directory -no_cleanup $* >${cmtxAODTrigRingerCnvtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=xAODTrigRingerCnv -version=xAODTrigRingerCnv-r655703 -path=/afs/cern.ch/user/j/jodafons/workspace_ringer_bundle/Event/xAOD  -quiet -without_version_directory -no_cleanup $* >${cmtxAODTrigRingerCnvtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtxAODTrigRingerCnvtempfile}
  unset cmtxAODTrigRingerCnvtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtxAODTrigRingerCnvtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtxAODTrigRingerCnvtempfile}
unset cmtxAODTrigRingerCnvtempfile
exit $cmtsetupstatus

