#!/bin/bash

ROOTDIR=`pwd`
skip_compile=$1

# comment out the line below to disable compilation of a private version of libdcap
compile_dcap=0

# extract the tarball prepared by the build job
if [ -n "${skip_compile}" ]; then
    if [ -s "all_compiled_code.tar" ]; then
	tar xf all_compiled_code.tar
    else
	echo "ERROR: cannot find the tarball with compiled code:"
	ls -l ${ROOTDIR}
	echo "Build job failed?"
	exit 1
    fi
fi;

echo "Memory summary:"
free -m
DQ2_LOCAL_SITE_ID=ROAMING;
echo "DQ2_LOCAL_SITE_ID = $DQ2_LOCAL_SITE_ID"
if [ ! "$DQ2_LOCAL_SITE_ID" ]; then
    echo DQ2_LOCAL_SITE_ID not defined \(use prun_jobs.sh default\)
    echo "DQ2_LOCAL_SITE_ID = $DQ2_LOCAL_SITE_ID";
fi
echo "PandaSite = $PandaSite"
echo "Proxy variable X509_USER_PROXY: $X509_USER_PROXY"
echo "voms-proxy-info:"
voms-proxy-info -all 2>&1
echo "grid-proxy-info -all:"
grid-proxy-info -all 2>&1
echo "which python:"
which python
file -L $(which python)

if [ ! -n "${skip_compile}" ]; then
    echo "which dccp:"
    which dccp
    echo "which gcc"
    which gcc
    file -L $(which gcc)
    gcc --version
    echo "gcc search dirs:"
    gcc -print-search-dirs 2>&1
    echo "which g++"
    which g++
    file -L $(which g++)
    g++ --version
    echo "g++ search dirs:"
    g++ -print-search-dirs 2>&1
    echo "which dq2-get:"
    which dq2-get
fi;

# SETTING UP DQ2 (this is non-trivial due to 32-bit vs 64-bit issues)
echo "Checking which version of python is able to import _lfc.so:"
echo "Trying default-environment python:"
PYTHON=$(which python);

# Wrapper function for dq2-get:
function DQ2_GET_CMD () {
    dq2-get -L ROAMING $@ 2>&1
    return;
    # see if -p dcap should be used:
    DQ2_FORCE_DCAP=`extractOption DQ2_FORCE_DCAP`
    local dq2_extras=""
    if [ "${DQ2_FORCE_DCAP}" == "" ]; then
	dq2_extras=""
    else
	dq2_extras="-p dcap"
    fi;
    # see if python path should be cleared of SITEROOT garbage
    OLDPYTHONPATH=$PYTHONPATH
    if [ "$PYTHON" == "/usr/bin/python" ]; then
	PYTHONPATH=`cleanedPythonpath`
    fi;
    time $PYTHON $(which dq2-get) ${dq2_extras} $@ 2>&1
    PYTHONPATH=$OLDPYTHONPATH
}
# Wrapper function for dq2-ls:
function DQ2_LS_CMD () {
    dq2-ls -L ROAMING $@
    return;
    # see if python path should be cleared of SITEROOT garbage
    OLDPYTHONPATH=$PYTHONPATH
    if [ "$PYTHON" == "/usr/bin/python" ]; then
	PYTHONPATH=`cleanedPythonpath`
    fi;
    $PYTHON $(which dq2-ls) $@
    PYTHONPATH=$OLDPYTHONPATH
}

# Make sure autotools are available
echo "Looking for aclocal:"
aclocal_exec=`which aclocal`
if [ -n ${aclocal_exec} ] && `test -s "${aclocal_exec}"`; then
    echo "Good, aclocal found"
else
    echo "ERROR: Oops, this computer node doesn't have aclocal (and probably the entire GNU autotools). Exiting..."
    exit 1;
fi;

# Make sure ROOT is present
if [ -n "${ROOTSYS}" ]; then
    echo "Root variable ROOTSYS: $ROOTSYS"
else
    echo "ROOTSYS is not set on this node. Exiting..."
    exit 1;
fi

# In case TrigFTK* packages have already been compiled (e.g, in the build job),
# allow to skip these steps:
if [ -n "${skip_compile}" ]; then
    echo "Using pre-compiled TrigFTK* packages"
    cd TrigFTKSim/standalone && source ./bootstrap.sh && cd -
else
    # Retrieving and compiling TrigFTKSim
    if [ -s TrigFTKSim.tar.bz2 ]; then
	echo "Extracting your version of TrigFTKSim executable"
	tar xfj TrigFTKSim.tar.bz2
    else
	echo "ERROR: Can't find TrigFTKSim.tar.bz2 in current directory ($PWD)"
	exit 1
    fi;
    if [ -s ${ROOTDIR}/TrigFTKSim/config/map_file/raw_11L.pmap ]; then
	echo "Good, config files and maps were extracted successfully"
    else
	echo "ERROR: Cannot find map_file/raw_11L.pmap "
	find ${ROOTDIR}/TrigFTKSim/config/map_file/
	exit 1
    fi;
    if [ -s ${ROOTDIR}/TrigFTKSim/standalone_scripts/ftk.py ]; then
	echo "Good, run scripts and templates were extracted successfully"
    else
	echo "ERROR: Cannot find ftk.py "
	find ${ROOTDIR}/TrigFTKSim/standalone_scripts/
	exit 1
    fi;
    cd TrigFTKSim/standalone
    if [ -s bootstrap.sh ]; then
	echo "Ready to compile in standalone/ directory"
    else
	echo "ERROR: cannot find bootstrap.sh script in ${PWD}"
	exit 1
    fi;
    source ./bootstrap.sh
    gmake -f Makefile.grid.mk clean 2>&1
    gmake -j6 -f Makefile.grid.mk 2>&1
    #gmake -f Makefile.grid.mk compact 2>&1
    gmake -C ../standalone_scripts clean
    gmake -C ../standalone_scripts

    ls
    cd ${ROOTDIR}
    for app in TrigFTKSim/standalone/{road_finder,road_merger,track_fitter,track_merger,patmerge,libftk_classes.so}; do
	if [ -s ${app} ]; then
	    echo "Good, executable `basename ${app}` was created in ${app}"
	    ldd ${app}
	else
	    echo "ERROR: cannot find executable `basename ${app}`"
	    echo "Directory listing:"
	    ls `dirname ${app}`
	    echo "ldconfig:"
	    /sbin/ldconfig -p
	    exit 1
	fi;
    done
    echo "DONE COMPILING TrigFTKSim EXECUTABLES"

    # Retrieving and compiling TrigFTKLib (if present)
    if [ -f "TrigFTKLib.tar.bz2" ]; then
	cd ${ROOTDIR}
	if [ -s TrigFTKLib.tar.bz2 ]; then
	    echo "Extracting your version of TrigFTKLib executable"
	    tar xfj TrigFTKLib.tar.bz2
	else
	    echo "ERROR: Can't find TrigFTKLib.tar.bz2 in current directory ($PWD)"
	    exit 1
	fi;
	if [ ! -s TrigFTKLib/src/bzip2/bzlib.h ]; then
	    echo 'ERROR: missing bzip2 in your version of TrigFTKLib'
	    echo 'Try updating TrigFTKLib via "svn update"'
	    exit 1;
	fi
	# perform some extra cleanup
	find TrigFTKLib -type d -name \.deps | xargs rm -rf
	# prepare bzip2 support (for compressed pattern output)
	cd TrigFTKLib/src/bzip2/ && gmake && cd -
	cd TrigFTKLib/standalone
	if [ -s build_standalone.csh ]; then
	    echo "Ready to compile in standalone/ directory"
	else
	    echo "ERROR: cannot find build_standalone.csh script in ${PWD}"
	    exit 1
	fi;
        # disable libdcap since the banks are always stored locally
	sed -i -e 's#dc_enabled=true#dc_enabled=false#g' configure.ac
        # enable bz2 output of pattern files
	sed -i -e 's#bz_enabled=false#bz_enabled=true#g' configure.ac
	./build_standalone.csh 2>&1
	ls
	cd ${ROOTDIR}
	for app in {TrigFTKLib/src/pattgen/pattgen,}; do
	    if [ -s ${app} ]; then
		echo "Good, executable `basename ${app}` was created in ${app}"
		ldd ${app}
	    else
		echo "ERROR: cannot find executable `basename ${app}`"
		ls `dirname ${app}`
		echo "ldconfig:"
		/sbin/ldconfig -p
		exit 1
	    fi;
	done
	echo "DONE COMPILING TrigFTKLib EXECUTABLES"
    fi;

    # Retrieving and compiling TrigFTKAna (if present)
    if [ -f "TrigFTKAna.tar.bz2" ]; then
	cd ${ROOTDIR}
	if [ -s TrigFTKAna.tar.bz2 ]; then
	    echo "Extracting your version of TrigFTKAna executable"
	    tar xfj TrigFTKAna.tar.bz2
	    cd TrigFTKAna/
	    if [ -s bootstrap.sh ]; then
		echo "Ready to compile TrigFTKAna"
	    else
		echo "ERROR: cannot find bootstrap.sh script in ${PWD}"
		exit 1
	    fi;
	    sh ./bootstrap.sh
	    gmake clean 2>&1
            # only compile what we need:
	    gmake -C src/ 2>&1
	    gmake -C dg/  2>&1
	    gmake -C ana/ ana_standard_overview 2>&1
	    gmake -C ana/ ana_ftkeffchain 2>&1
	    ls
	    cd ${ROOTDIR}
	    for app in {TrigFTKAna/ana/ana_standard_overview,}; do
		if [ -s ${app} ]; then
		    echo "Good, executable `basename ${app}` was created in ${app}"
		    ldd ${app}
		else
		    echo "WARNING: cannot find executable `basename ${app}`"
		    ls `dirname ${app}`
		    echo "Analysis jobs will surely fail! But continuing anyway..."
		fi;
	    done
	    echo "DONE COMPILING TrigFTKAna EXECUTABLES"
	else
	    echo "WARNING: can't find TrigFTKAna.tar.bz2. Analysis jobs will surely fail."
	fi;
    fi;
fi;

# Compiling a private version of libdcap
# (sometimes needed for 64-bit libpdcap.so)
if [  "${compile_dcap}" == "1" ]; then
    echo "Extracting your version of libdcap"
    cd ${ROOTDIR}/TrigFTKSim/external
    source compile.sh ${skip_compile}
    echo "DONE COMPILING libdcap"
fi;
cd ${ROOTDIR}

# tar up compiled code in the end of the build job (to reduce log verbosity)
# alternatively, untar it in the beginning of analysis jobs
if [ ! -n "${skip_compile}" ]; then
    tar cf all_compiled_code.tar TrigFTK*
    rm -rf TrigFTK*
    echo "Final tarball with compiled code:"
    ls -l all_compiled_code.tar
fi;
