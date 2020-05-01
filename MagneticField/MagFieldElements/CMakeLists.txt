################################################################################
# Package: MagFieldElements
################################################################################

# Declare the package name:
atlas_subdir( MagFieldElements )

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
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MagFieldElements
                   src/*.cxx
                   PUBLIC_HEADERS MagFieldElements
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
		   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}  ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel CxxUtils)

# Install files from the package:
atlas_install_headers( MagFieldElements )


