# This script sets BOOST_LDFLAGS,BOOST_CXXFLAGS,DCAP_LDFLAGS,DCAL_CXXFLAGS from athena environment
# Note that you must have sourced athena setup.sh -tag=whatever, so that SITEROOT is defined!
# VERIFIED TO WORK WITH ALL ATHENA VERSIONS BETWEEN 13.0.40 and 15.6.1

# Make sure athena environment has been bootstrapped
#uct3_64=0 # new uct3 installation not require any special treatment for uct3_64

arch=`echo $CMTCONFIG | cut -d '-' -f 1`
# Stefan, 2013/12/17, replace == by = otherwise script does not work with zsh
### if [ "$arch" == "x86_64" ] ; then
if [ "$arch" = "x86_64" ] ; then
    export uct3_64=1
    echo "Detected 64 bit environment"
else
    export uct3_64=0
    echo "Detected 32 bit environment"
fi

# Make sure dcache is present and export compiler flags
echo "Trying to find libdcap shipped with athena:"
export DCAPDIR=`cd $SITEROOT/sw/lcg/external/dcache_client/*/$CMTCONFIG/dcap/ && pwd`

DCAP_INCDIR="${DCAPDIR}/include"
export DCAP_CXXFLAGS="-I${DCAP_INCDIR}"
if [ ${uct3_64} -ne 1 ]; then
    DCAP_LDDIR="${DCAPDIR}/lib"
else
    DCAP_LDDIR="${DCAPDIR}/lib64"
fi
export DCAP_LDPRELOAD_ATH="${DCAP_LDDIR}/libpdcap.so"  # usually won't work
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${DCAP_LDDIR}
export DCAP_LDFLAGS="-L${DCAP_LDDIR} -ldcap"
if [ -s "${DCAP_INCDIR}/dcap.h" ]; then
    echo "Good, libdcap is found:"
    echo "DCAP_CXXFLAGS = ${DCAP_CXXFLAGS}"
    echo "DCAP_LDFLAGS = ${DCAP_LDFLAGS}"
    echo "DCAP_LDPRELOAD_ATH = ${DCAP_LDPRELOAD_ATH}"
else
    echo "OOPS, cannot get libdcap. Something is very wrong with this node's setup"
    if [ "$#" -gt "0" ]; then exit 2; fi;
fi;

# Make sure boost c++ is present and export compiler flags
echo "Trying to find boost libraries shipped with athena:"
export BOOSTDIR=`cd $SITEROOT/sw/lcg/external/Boost/*/$CMTCONFIG/ && pwd`

BOOST_INCDIR=`cd ${BOOSTDIR}/include/boost-*/ && pwd`
export BOOST_CXXFLAGS="-I${BOOST_INCDIR}"
export BOOST_VER=`echo ${BOOST_CXXFLAGS} | awk 'BEGIN{FS="-"}; { print $NF; }'`
BOOST_LDDIR="${BOOSTDIR}/lib"
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${BOOST_LDDIR}
BOOST_IOLIB=`cd ${BOOST_LDDIR}; /bin/ls libboost_iostreams-gcc*-mt-1*.so | sed -e 's#.so##' -e 's#lib##'`
BOOST_PRLIB=`cd ${BOOST_LDDIR}; /bin/ls libboost_program_options-gcc*-mt-1*.so | sed -e 's#.so##' -e 's#lib##'`
BOOST_FSLIB=`cd ${BOOST_LDDIR}; /bin/ls libboost_filesystem-gcc*-mt-1*.so | sed -e 's#.so##' -e 's#lib##'`
export BOOST_LDFLAGS="-L${BOOST_LDDIR} -l${BOOST_IOLIB} -l${BOOST_PRLIB} -l${BOOST_FSLIB}"
if [ -s "${BOOST_INCDIR}/boost/iostreams/operations.hpp" -a -n "${BOOST_IOLIB}" -a -n "${BOOST_PRLIB}" -a -n "${BOOST_FSLIB}" ]; then
    echo "Good, boost version ${BOOST_VER} is found:"
    echo "BOOST_CXXFLAGS = ${BOOST_CXXFLAGS}"
    echo "BOOST_LDFLAGS = ${BOOST_LDFLAGS}"
else
    echo "OOPS, cannot find boost. Something is very wrong with this node's setup"
    if [ "$#" -gt "0" ]; then exit 2; fi;
fi;

# Alias to variables used in TrigFTKAna
export TRIGFTKANA_BOOST_DIR=${BOOSTDIR}
export TRIGFTKANA_DCAP_DIR=${DCAPDIR}

