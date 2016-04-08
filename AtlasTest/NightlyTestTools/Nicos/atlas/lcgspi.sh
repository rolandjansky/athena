# CERN, LCG-SPI Project, 2002
#
# set environment for LCG-SPI tools
#
# usage: ./lcgspi.sh
# options: 
#   -q   : be silent

export PATH=/afs/cern.ch/sw/lcg/external/QMtest/2.0.3/rh73_gcc32/bin:/usr/local/gcc-alt-3.2/bin:/afs/cern.ch/sw/lcg/app/spi/scram:$PATH
export LD_LIBRARY_PATH=/usr/local/gcc-alt-3.2/lib:$LD_LIBRARY_PATH
export SCRAM_ARCH=rh73_gcc32

# the string 0.3.1 is modified automatically by the release script
export LCGSPI_VERSION=0.3.1

export LCGSPI_TOP=/afs/cern.ch/sw/lcg/app/spi
export LCGSPI_RELDIR=${LCGSPI_TOP}/tools/${LCGSPI_VERSION}

# path to scram is not versioned -- scram takes care of swicthing to
# the right version automatically
export PATH=${LCGSPI_RELDIR}/scripts:${LCGSPI_TOP}/scram:${PATH}

export PYTHONPATH=${LCGSPI_RELDIR}/python:${PYTHONPATH}

# guess the architecture
# FIXME: for the moment only one architecture supported
export SCRAM_ARCH=rh73_gcc32

if [ ! ${1} = "-q" ]; then
echo 'Environment for LCG Applications is set up'
echo 'SPI Tools release' ${LCGSPI_VERSION}
fi
