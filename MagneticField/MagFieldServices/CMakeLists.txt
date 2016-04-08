################################################################################
# Package: MagFieldServices
################################################################################

# Declare the package name:
atlas_subdir( MagFieldServices )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          MagneticField/MagFieldInterfaces
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MagFieldServices
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps MagFieldInterfaces StoreGateLib SGtests AthenaPoolUtilities GaudiKernel PathResolver )

# Install files from the package:
atlas_install_headers( MagFieldServices )
atlas_install_python_modules( python/*.py )

