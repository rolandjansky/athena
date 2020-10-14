# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaMVACalib )

# Extra dependencies for Athena capable builds:
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_libs xAODRootAccess)
else()
   set( extra_libs GaudiKernel egammaInterfacesLib )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Tree Core Hist)

# Component(s) in the package:
atlas_add_library( egammaMVACalibLib
   egammaMVACalib/*.h Root/*.cxx
   PUBLIC_HEADERS egammaMVACalib
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib xAODCaloEvent xAODEgamma ${extra_libs} )

if( NOT XAOD_STANDALONE )
atlas_add_component( egammaMVACalib
	src/*.cxx src/components/*.cxx
	INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MVAUtils PathResolver egammaInterfacesLib egammaMVACalibLib xAODCaloEvent xAODEgamma xAODTracking )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
