################################################################################
# Package: MagFieldServices
################################################################################

# Declare the package name:
atlas_subdir( MagFieldServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          MagneticField/MagFieldInterfaces
                          MagneticField/MagFieldConditions
			  Control/CxxUtils
                          PRIVATE
                          MagneticField/MagFieldElements
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          Tools/PathResolver
			  Event/EventInfo
			  )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MagFieldServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
		     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} MagFieldElements MagFieldConditions CxxUtils 
		     AthenaBaseComps MagFieldInterfaces StoreGateLib SGtests AthenaPoolUtilities GaudiKernel PathResolver
		     EventInfo )

# Install files from the package:
atlas_install_headers( MagFieldServices )
atlas_install_python_modules( python/*.py )

if( NOT SIMULATIONBASE )
  atlas_add_test( MagFieldServicesConfig    SCRIPT python -m MagFieldServices.MagFieldServicesConfig
                  PROPERTIES TIMEOUT 300
                  POST_EXEC_SCRIPT nopost.sh )
endif()
