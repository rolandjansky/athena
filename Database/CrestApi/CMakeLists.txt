# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set the name of the package.
atlas_subdir( CrestApi )

# External dependencies.
find_package( CURL )
find_package( nlohmann_json )
find_package( Boost )

# Component(s) in the package.
atlas_add_library( CrestApi
   CrestApi/CrestApi.h src/CrestApi.cxx
   PUBLIC_HEADERS CrestApi
   LINK_LIBRARIES nlohmann_json::nlohmann_json
   PRIVATE_INCLUDE_DIRS ${CURL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${CURL_LIBRARIES} ${Boost_LIBRARIES} -lstdc++fs )

# Test(s) in the package.
atlas_add_test( test
   SOURCES test/test.cxx
   LINK_LIBRARIES CrestApi nlohmann_json::nlohmann_json
   INCLUDE_DIRS ${CURL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${CURL_LIBRARIES} ${Boost_LIBRARIES} -lstdc++fs
   POST_EXEC_SCRIPT nopost.sh )
