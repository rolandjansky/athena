# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AsgServices )

# Dependencies are taken based on what environment we are in:
if( XAOD_STANDALONE )
   set( libs )
else()
   set( libs )
endif()

# Decide which sources to use:
if( XAOD_STANDALONE )
   set( sources Root/*.cxx )
else()
   set( sources Root/AsgService.cxx Root/AsgServiceConfig.cxx )
endif()

# Component(s) in the package:
atlas_add_library( AsgServicesLib
   AsgServices/*.h AsgServices/*.icc ${sources}
   PUBLIC_HEADERS AsgServices
   PRIVATE_INCLUDE_DIRS
   LINK_LIBRARIES AsgTools ${libs}
   PRIVATE_LINK_LIBRARIES )

# Select which xml files to use for dictionary generation

atlas_add_dictionary( AsgServicesDict
   AsgServices/AsgServicesDict.h AsgServices/selection.xml
   LINK_LIBRARIES AsgServicesLib )

if (XAOD_STANDALONE)
  atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
   atlas_add_test( AsgServiceConfigUnitTests
      SCRIPT python -m unittest AsgServices.AsgServiceConfig
      POST_EXEC_SCRIPT nopost.sh )
  atlas_add_dictionary( AsgServicesStandaloneDict
     AsgServices/AsgServicesStandaloneDict.h AsgServices/selection_standalone.xml
     LINK_LIBRARIES AsgServicesLib )
endif ()
