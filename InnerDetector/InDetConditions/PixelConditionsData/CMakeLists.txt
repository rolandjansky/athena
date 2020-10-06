################################################################################
# Package: PixelConditionsData
################################################################################

# Declare the package name:
atlas_subdir( PixelConditionsData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( 
  PUBLIC
  Control/CxxUtils
  DetectorDescription/Identifier
  DetectorDescription/GeoPrimitives
  Database/AthenaPOOL/AthenaPoolUtilities
  InnerDetector/InDetConditions/InDetByteStreamErrors
  PRIVATE
  Control/AthenaKernel
  GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( PixelConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS PixelConditionsData
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CxxUtils Identifier GeoPrimitives
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel AthenaPoolUtilities GaudiKernel )

