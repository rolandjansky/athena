#!/bin/bash
#
# $Id: cmakeReleaseInstall.sh 756309 2016-06-20 15:39:40Z krasznaa $
#
# Installtion script of ATLAS CMake release RPMs
# Author: Johannes Elmsheuser, Attila Krasznahorkay 
# Date: May 2016

# Function showing the usage help
show_help() {
    echo "Usage: cmakeRPMInstall.sh -d installDir -c yumCacheDir pkg1 pkg2..."
    echo "Example: ./cmakeRPMInstall.sh -d /opt/releases -c /tmp/rpms AtlasOffline_21.0.0_x86_64-slc6-gcc49-opt"
}

# Stop on errors:
set -e

# Parse the command line arguments:
OPTIND=1
while getopts ":d:c:" opt; do
    case "$opt" in
	     h|\?)
	         show_help
	         exit 0
	         ;;
	     d)
	         INSTALLDIR=`readlink -f $OPTARG`
	         ;;
        c)
            CACHEDIR=`readlink -f $OPTARG`
            ;;
    esac
done
shift $((OPTIND-1))
PROJECTS=$@

# ayum directory
AYUMDIR=$TMPDIR
# Directory name with the date
DATEDIR=`date "+%FT%H%M"`

echo "#############################################"
echo "Installing project(s) $PROJECTS"
echo "  into directory : $INSTALLDIR"
echo "  using cache dir: $CACHEDIR"
echo "  AYUM directory : $AYUMDIR"
echo "#############################################"
echo

# Check that everything was specified:
if [ -z "$INSTALLDIR" ] || [ -z "$PROJECTS" ]; then
    show_help
    exit 1
fi

# Create the installation directory:
if [ ! -d "$INSTALLDIR" ]; then
    echo "Creating directory $INSTALLDIR"
    mkdir -p $INSTALLDIR
fi

# Set up ayum from scratch in the current directory:
CURDIR=$PWD
cd $AYUMDIR
rm -rf ayum/
git clone https://gitlab.cern.ch/rhauser/ayum.git
cd ayum
if [ -z "$CACHEDIR" ]; then
    ./configure.ayum -i $INSTALLDIR -D > yum.conf
else
    ./configure.ayum -i $INSTALLDIR -c $CACHEDIR -D > yum.conf
fi

# Remove the unnecessary line from the generated file:
sed 's/AYUM package location.*//' yum.conf > yum.conf.fixed
mv yum.conf.fixed yum.conf

# Configure the ayum repositories:
cat - >./etc/yum.repos.d/lcg.repo <<EOF
[lcg-repo]
name=LCG Repository
baseurl=http://cern.ch/service-spi/external/rpms/lcg
prefix=${INSTALLDIR}/sw/lcg/releases
enabled=1
EOF

cat - >./etc/yum.repos.d/tdaq-nightly.repo <<EOF
[tdaq-nightly]
name=nightly snapshots of TDAQ releases
baseurl=http://cern.ch/atlas-tdaq-sw/yum/tdaq/nightly
enabled=1
EOF

cat - >./etc/yum.repos.d/atlas-offline-data.repo <<EOF
[atlas-offline-data]
name=ATLAS offline data packages
baseurl=http://cern.ch/atlas-software-dist/RPMs/data
enabled=1
EOF

cat - >./etc/yum.repos.d/atlas-offline-releases.repo <<EOF
[atlas-offline-releases-slc6]
name=ATLAS offline releases SLC6
baseurl=http://cern.ch/atlas-software-dist/RPMs/releases/slc6
enabled=1

[atlas-offline-releases-cc7]
name=ATLAS offline releases CC7
baseurl=http://cern.ch/atlas-software-dist/RPMs/releases/cc7
enabled=0
EOF

# Tell the user what happened:
echo "Configured AYUM"

# Setup environment to run the ayum command:
shopt -s expand_aliases
source ./setup.sh
cd $CURDIR

# Install the requested project(s):
ayum -y install $PROJECTS
