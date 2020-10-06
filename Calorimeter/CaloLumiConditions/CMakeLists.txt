# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloLumiConditions )

# External dependencies:
find_package( Boost COMPONENTS timer )

# Component(s) in the package:
atlas_add_library( CaloLumiConditions
                   src/*.cxx
                   PUBLIC_HEADERS CaloLumiConditions
                   LINK_LIBRARIES CxxUtils Identifier AthenaKernel LArElecCalib
                   PRIVATE_LINK_LIBRARIES LArIdentifier LumiBlockData )

atlas_add_test( CaloBCIDCoeffs_test
                SOURCES
                test/CaloBCIDCoeffs_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} CaloLumiConditions IdDictParser LArIdentifier TestTools )

atlas_add_test( CaloBCIDLumi_test
                SOURCES
                test/CaloBCIDLumi_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES CaloLumiConditions IdDictParser LArIdentifier LumiBlockData TestTools
                PROPERTIES TIMEOUT 300 )
