#!/bin/bash
#
# $Id: cmakeNightlyInstall.sh 756309 2016-06-20 15:39:40Z krasznaa $
#
# Installtion script of ATLAS CMake nightly RPMs
# Author: Johannes Elmsheuser, Attila Krasznahorkay 
# Date: April 2016

# Function showing the usage help
show_help() {
    echo "Usage: cmakeRPMInstall.sh -r nightlyVer -d installDir -t dateString pkg1 pkg2..."
    echo "Example: ./cmakeRPMInstall.sh -r 21.X.Y-VAL/rel_2 -d $HOME/opt -t 2016-06-20T1650 AtlasOffline*"
}

# Stop on errors:
set -e

# Parse the command line arguments:
OPTIND=1
while getopts ":r:d:t:" opt; do
    case "$opt" in
	     h|\?)
	         show_help
	         exit 0
	         ;;
	     r)
	         NIGHTLYVER=$OPTARG
	         ;;
	     d)
	         INSTALLDIR=`readlink -f $OPTARG`
	         ;;
        t)
            DATEDIR=$OPTARG
            ;;
    esac
done
shift $((OPTIND-1))
PROJECTS=$@

if [ ! -d "$TMPDIR" ]; then 
   if [ -d "/tmp/$USER" ]; then 
      export TMPDIR=/tmp/$USER
   else 
      export TMPDIR=$HOME
   fi
fi

# ayum directory
AYUMDIR=$TMPDIR
# Directory name with the date
if [ -z "$DATEDIR" ]; then
    DATEDIR=`date "+%FT%H%M"`
fi

echo "#############################################"
echo "Installing project(s) $PROJECTS"
echo "  from nightly  : $NIGHTLYVER"
echo "  into directory: $INSTALLDIR/$DATEDIR"
echo "  AYUM directory: $AYUMDIR"
echo "#############################################"
echo

# Check that everything was specified:
if [ -z "$NIGHTLYVER" ] || [ -z "$INSTALLDIR" ] || \
    [ -z "$PROJECTS" ]; then
    show_help
    exit 1
fi

# Create RPM directory:
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
./configure.ayum -i $INSTALLDIR -D > yum.conf

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

cat - >./etc/yum.repos.d/atlas-offline-nightly.repo <<EOF
[atlas-offline-nightly]
name=ATLAS offline nightly releases
baseurl=http://cern.ch/atlas-software-dist/RPMs/nightlies/${NIGHTLYVER}
prefix=${INSTALLDIR}/${DATEDIR}
enabled=1
EOF

# Tell the user what happened:
echo "Configured AYUM"

# Setup environment to run the ayum command:
shopt -s expand_aliases
source ./setup.sh
cd $CURDIR

# First try to reinstall the project. Assuming that a previous version
# of it is already installed. If it's not, then simply install it.
ayum -y reinstall $PROJECTS || ayum -y install $PROJECTS
