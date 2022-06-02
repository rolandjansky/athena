# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( RecAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core )
find_package( Boost COMPONENTS unit_test_framework )

# Component(s) in the package:
atlas_add_component( RecAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CxxUtils xAODEventInfo GaudiKernel RecEvent )
                     
atlas_add_test(ParseCpuInfo_test
  SOURCES test/ParseCpuInfo_test.cxx src/ParseCpuInfo.cxx
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} PathResolver
  POST_EXEC_SCRIPT "nopost.sh" )
  
 atlas_install_data( test/cpuinfo )
