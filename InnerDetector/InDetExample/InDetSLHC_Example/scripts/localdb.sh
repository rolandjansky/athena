#!/bin/bash 
# script to set up access to geometry and conditions database access via
# softlinks in run directory to current latest replicas at CERN
# only works at CERN, or with access to CERN AFS tree!
#
  echo Setting up geomDB, triggerDB and sqlite200 links in current directory to use 
  echo CERN afs SQLite replicas of geometry, conditions and trigger databases
  if [ ! -e geomDB ]
  then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB geomDB
  fi
  if [ ! -e triggerDB ]
  then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/triggerDB triggerDB
  fi
  if [ ! -e sqlite200 ]
  then
    if [ -n "$1" -a "$1" = "COMP200" ]
    then
      echo Special setup for COMP200 replica
      mkdir sqlite200
      ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db sqlite200/ALLP200.db
    else
      ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200 sqlite200
    fi
  fi
