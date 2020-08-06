# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SGComps )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( SGComps
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthContainersInterfaces AthLinks AthenaBaseComps AthenaKernel CxxUtils GaudiKernel SGTools StoreGateLib )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Declare the test(s) of the package:
atlas_add_test( SGFolder_test
   SOURCES test/SGFolder_test.cxx src/SGFolder.cxx
   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel SGTools TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

 atlas_add_test( ProxyProviderSvc_test
   SOURCES test/ProxyProviderSvc_test.cxx
   LINK_LIBRARIES AthenaKernel GaudiKernel SGTools TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

 atlas_add_test( AddressRemappingSvc_test
   SOURCES test/AddressRemappingSvc_test.cxx
   LINK_LIBRARIES AthContainersInterfaces AthenaBaseComps AthenaKernel GaudiKernel SGTools TestTools
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )

atlas_add_test( AddressRemappingConfig_test
   SCRIPT test/AddressRemappingConfig_test.py )
