#!/bin/sh
#+
#	Helper script to setup the test area entries in the various paths
#	(PATH, LD_LIBRARY_PATH, PYTHONPATH, DATAPATH, JOBOPTIONSEARCHPATH)
#	if the test area doesn't exist, or the InstallArea within the test
#	area doesn't yet exist. 
#
#	The test area is taken as the first entry of CMTPATH if it's non-empty
#
#	Author:-
#	David Quarrie
#-
[ -n "${ATLAS_NO_SETUP_RUNTIME_HELPER:-}" ] && return

# Determine the current Python 2-digit version for later use by PYTHONPATH
s_vers=`python -c "import sys; print '%s.%s' % sys.version_info[:2]"`
	
if [ ! -z "${CMTPATH}" ]; then
    s_dirs=`\echo $CMTPATH | \sed 's/:/ /g'`
    for s_dir in `\echo $s_dirs`; do
	    s_test=`echo $LD_LIBRARY_PATH | \grep -E ${s_dir}`
	    if [ -z "${s_test}" ]; then
		    s_lib=`echo $LD_LIBRARY_PATH | \grep -E "^/lib:"`
		    if [ -z ${s_lib} ]; then
		        export LD_LIBRARY_PATH=${s_dir}/InstallArea/${CMTCONFIG}/lib:${LD_LIBRARY_PATH}
			else
			    s_lib=`echo $LD_LIBRARY_PATH | \sed 's#^/lib:##'`
		        export LD_LIBRARY_PATH=/lib:${s_dir}/InstallArea/${CMTCONFIG}/lib:${s_lib}
			fi
			s_lib=
		fi
	    s_test=`\echo $PATH | \grep ${s_dir}`
	    if [ -z "${s_test}" ]; then
		    export PATH=${s_dir}/InstallArea/share/bin:${s_dir}/InstallArea/${CMTCONFIG}/bin:${PATH}
		fi
	    s_test=`\echo $PYTHONPATH | \grep ${s_dir}`
	    if [ -z "${s_test}" ]; then
		    export PYTHONPATH=${s_dir}/InstallArea/python:${s_dir}/InstallArea/${CMTCONFIG}/lib/python${s_vers}:${PYTHONPATH}
		fi
	    s_test=`\echo $JOBOPTSEARCHPATH | \grep ${s_dir}`
	    if [ -z "${s_test}" ]; then
		    export JOBOPTSEARCHPATH=${s_dir}/InstallArea/jobOptions:${JOBOPTSEARCHPATH}
		fi
	    s_test=`\echo $DATAPATH | \grep ${s_dir}`
	    if [ -z "${s_test}" ]; then
		    export DATAPATH=${s_dir}/InstallArea/share:${DATAPATH}
		fi
		unset s_dir
		unset s_test
		break
	done
	unset s_dirs
fi

#+
# If the SLC4 compatibility libraries are in LD_LIBRARY_PATH, move them to the front, before /lib, such
# that libraries in the latter can be overridden.
#-

s_dirs=`\echo $LD_LIBRARY_PATH | \sed 's/:/ /g'`
s_path=
s_compat=
for s_dir in `\echo $s_dirs`; do
    s_test=`echo $s_dir | \grep "/slc4compat/"`
    if [ -z "${s_test}" ]; then
		if [ -z "${s_path}" ]; then
			s_path=${s_dir}
		else
	    	s_path=${s_path}:${s_dir}
		fi
	else
		s_compat=${s_dir}
	fi
done
if [ ! -z "${s_compat}" ]; then
	s_path=${s_compat}:${s_path}
fi
export LD_LIBRARY_PATH=${s_path}
unset s_dirs
unset s_path
unset s_compat
unset s_vers
