#!/bin/sh
# This just selects the directory to run cmt commands
# But with project builds and as this package (AssociationUtils) can be moved
# from some project to another, we wildcard the AtlasXyzRelease directory
cd ${NIGHTLYAREA}/Atlas*Release/cmt
#
cmt broadcast -select=AssociationUtils make CppUnit
stat=$?
if [ "$stat" != "0"  ]; then
       echo " -------------------------------- "
       echo " FAILURE : test AssociationUtils  "
       echo " -------------------------------- "
fi
