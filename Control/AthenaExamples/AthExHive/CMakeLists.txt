# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExHive )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( AthExHiveLib
                   AthExHive/*.h
                   INTERFACE
                   PUBLIC_HEADERS AthExHive
                   LINK_LIBRARIES AthenaKernel GaudiKernel)

atlas_add_component( AthExHive
                     src/*.cxx
                     src/condEx/*.cxx
                     src/loopTest/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel AthenaBaseComps GaudiKernel StoreGateLib xAODEventInfo AthExHiveLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/condDb.txt )

# Tests in the package:
atlas_add_test ( AthExHive_test
   SCRIPT test/AthExHive_test.sh
   PROPERTIES TIMEOUT 300
   )

atlas_add_test ( AthExHiveCond_test
   SCRIPT test/AthExHiveCond_test.sh
   PROPERTIES TIMEOUT 300
   )

