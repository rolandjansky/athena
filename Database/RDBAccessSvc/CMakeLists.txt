# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RDBAccessSvc )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( RDBAccessSvcLib
                   RDBAccessSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS RDBAccessSvc
                   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CORAL_LIBRARIES} GaudiKernel )

atlas_add_component( RDBAccessSvc
                     src/RDBAccessSvc.cxx
                     src/RDBRecordset.cxx
                     src/RDBRecord.cxx
                     src/RDBVersionAccessor.cxx
                     src/RDBQuery.cxx
                     src/SourceCompAlg.cxx
                     src/components/RDBAccessSvc_entries.cxx
                     LINK_LIBRARIES AthenaBaseComps CxxUtils RDBAccessSvcLib )

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
