#!/bin/sh
# This just selects the directory to run cmt commands
# But with project builds and as this package (McParticleUtils) can be moved
# from some project to another, we wildcard the AtlasXyzRelease directory
cd ${NIGHTLYAREA}/Atlas*Release/cmt
#
cmt broadcast -select=McParticleUtils make CppUnit
stat=$?
if [ "$stat" != "0"  ]; then
       echo " -------------------------------- "
       echo " FAILURE : test McParticleUtils   "
       echo " -------------------------------- "
fi
