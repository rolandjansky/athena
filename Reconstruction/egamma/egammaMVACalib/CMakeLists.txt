################################################################################
# Package: egammaMVACalib
################################################################################

# Declare the package name:
atlas_subdir( egammaMVACalib )

# Extra dependencies for Athena capable builds:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess)
else()
   set( extra_dep 
	GaudiKernel 
	Reconstruction/egamma/egammaInterfaces)
endif()


# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODTracking
   PRIVATE
   Reconstruction/MVAUtils
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Tree Core Hist)

# Component(s) in the package:
atlas_add_library( egammaMVACalibLib
   egammaMVACalib/*.h Root/*.cxx
   PUBLIC_HEADERS egammaMVACalib
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODEgamma xAODTracking)

if( NOT XAOD_STANDALONE )
atlas_add_component( egammaMVACalib
	src/*.cxx src/components/*.cxx
	INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
	LINK_LIBRARIES ${ROOT_LIBRARIES} egammaMVACalibLib PathResolver MVAUtils GaudiKernel)
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )

