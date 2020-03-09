################################################################################
# Package: AFP_GeoModel
################################################################################

# Declare the package name:
atlas_subdir( AFP_GeoModel )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoModel/GeoModelUtilities
                          ForwardDetectors/AFP/AFP_Geometry
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          DetectorDescription/GeoModel/GeoModelInterfaces
			  DetectorDescription/GeoPrimitives
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( AFP_GeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS AFP_GeoModel
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel ${GEOMODELCORE_LIBRARIES} GeoModelUtilities AFP_Geometry StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaPoolUtilities GaudiKernel )

atlas_add_component( AFP_GeoModel
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities AFP_Geometry StoreGateLib SGtests AthenaPoolUtilities GaudiKernel AFP_GeoModelLib )

