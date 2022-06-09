# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CoralUtilities )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( nlohmann_json )
find_package( ROOT COMPONENTS Core RIO Tree )
find_package( ZLIB )

# Component(s) in the package:
atlas_add_library( CoralUtilitiesLib
   CoralUtilities/*.h src/*.cxx
   PUBLIC_HEADERS CoralUtilities
   LINK_LIBRARIES nlohmann_json::nlohmann_json
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                        ${ZLIB_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${ZLIB_LIBRARIES}
                          CxxUtils )
