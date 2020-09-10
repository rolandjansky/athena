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
  InnerDetector/InDetConditions/PixelCoralClientUtils
  PRIVATE
  Control/AthenaKernel
  GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( PixelConditionsData
                   src/*.cxx
                   PUBLIC_HEADERS PixelConditionsData
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CxxUtils Identifier GeoPrimitives PixelCoralClientUtils
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel AthenaPoolUtilities GaudiKernel )

