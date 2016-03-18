#!/bin/csh
#
#	Helper script to setup the test area entries in the various paths
#	(PATH, LD_LIBRARY_PATH, PYTHONPATH, DATAPATH, JOBOPTIONSEARCHPATH)
#	if the test area doesn't exist, or the InstallArea within the test
#	area doesn't yet exist. 
#
#	The test area is taken as the first entry of CMTPATH if it exists and is non-empty
#
#	Author:-
#	David Quarrie
#-
if ( ${?ATLAS_NO_SETUP_RUNTIME_HELPER} ) then
    if ( "${ATLAS_NO_SETUP_RUNTIME_HELPER}" != "" ) exit
endif

# Determine the current Python 2-digit version for later use within PYTHONPATH
set s_vers=`python -c "import sys; print '%s.%s' % sys.version_info[:2]"`


if ( $?CMTPATH != 0 ) then
    set s_dirs=`\echo $CMTPATH | \sed 's/:/ /g'`
    foreach s_dir ($s_dirs)
	    set s_test=`echo $LD_LIBRARY_PATH | \grep -E ${s_dir}`
	    if ( "${s_test}" == "" ) then
		    set s_lib=`echo $LD_LIBRARY_PATH | \grep -E "^/lib:"`
		    if ( "${s_lib}" == "" ) then
		        setenv LD_LIBRARY_PATH ${s_dir}/InstallArea/${CMTCONFIG}/lib:${LD_LIBRARY_PATH}
			else
			    set s_lib=`echo $LD_LIBRARY_PATH | \sed 's#^/lib:##'`
		        setenv LD_LIBRARY_PATH /lib:${s_dir}/InstallArea/${CMTCONFIG}/lib:${s_lib}
			endif
			unset s_lib
		endif
	    set s_test=`echo $PATH | grep ${s_dir}`
	    if ( "${s_test}"  == "" ) then
		    setenv PATH ${s_dir}/InstallArea/share/bin:${s_dir}/InstallArea/${CMTCONFIG}/bin:${PATH}
		endif
	    set s_test=`echo $PYTHONPATH | grep ${s_dir}`
	    if ( "${s_test}" == "" ) then
		    setenv PYTHONPATH ${s_dir}/InstallArea/python:${s_dir}/InstallArea/${CMTCONFIG}/lib/python${s_vers}:${PYTHONPATH}
		endif
	    set s_test=`echo $JOBOPTSEARCHPATH | grep ${s_dir}`
	    if ( "${s_test}" == "" ) then
		    setenv JOBOPTSEARCHPATH ${s_dir}/InstallArea/jobOptions:${JOBOPTSEARCHPATH}
		endif
	    set s_test=`echo $DATAPATH | grep ${s_dir}`
	    if ( "${s_test}" == "" ) then
		    setenv DATAPATH ${s_dir}/InstallArea/share:${DATAPATH}
		endif
		unset s_dir
		unset s_test
		break
	end
	unset s_dirs
endif
#+
# If the SLC4 compatibility libraries are in LD_LIBRARY_PATH, move them to the front, before /lib, such
# that libraries in the latter can be overridden.
#-

set s_dirs=`\echo $LD_LIBRARY_PATH | \sed 's/:/ /g'`
set s_path=
set s_compat=
foreach s_dir ( $s_dirs )
    set s_test=`echo $s_dir | \grep "/slc4compat/"`
    if ( "${s_test}" == "" ) then
		if ( "${s_path}" == "" ) then
			set s_path=${s_dir}
		else
	    	set s_path=${s_path}:${s_dir}
		endif
	else
		set s_compat=${s_dir}
	endif
end
if ( "${s_compat}" != "" ) then
	set s_path=${s_compat}:${s_path}
endif
setenv LD_LIBRARY_PATH ${s_path}
unset s_dirs
unset s_path
unset s_compat
