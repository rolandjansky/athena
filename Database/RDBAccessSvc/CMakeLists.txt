# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RDBAccessSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( SQLite3 )
find_package( Boost )

# Component(s) in the package:
atlas_add_library( RDBAccessSvcLib
                   RDBAccessSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS RDBAccessSvc
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel DBLock )

atlas_add_component( RDBAccessSvc
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${SQLITE3_INCLUDE_DIRS}
                     LINK_LIBRARIES ${SQLITE3_LIBRARIES} AthenaBaseComps CxxUtils RDBAccessSvcLib )

atlas_add_executable( geomdb2sqlite
                      replicator/geomdb2sqlite.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} RDBAccessSvcLib )

atlas_add_executable( geomdb2oracle
                      replicator/geomdb2oracle.cpp
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} RDBAccessSvcLib CxxUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )
