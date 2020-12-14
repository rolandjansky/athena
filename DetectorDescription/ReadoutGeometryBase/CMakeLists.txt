################################################################################
# Package: ReadoutGeometryBase
################################################################################

# Declare the package name:
atlas_subdir( ReadoutGeometryBase )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( ReadoutGeometryBase
                   src/*.c*
                   PUBLIC_HEADERS ReadoutGeometryBase
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel SGTools AtlasDetDescr GeoPrimitives Identifier GaudiKernel InDetCondTools InDetIdentifier TrkDetElementBase TrkSurfaces TrkEventPrimitives StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities DetDescrConditions IdDictDetDescr TRT_ConditionsData GeoModelUtilities)
