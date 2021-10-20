################################################################################
# Package: HGTD_TrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( HGTD_TrackingGeometry )

# External dependencies:
find_package( Boost )
find_package( GeoModel COMPONENTS GeoModelKernel )

# Component(s) in the package:
atlas_add_component( HGTD_TrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     PUBLIC_HEADERS HGTD_TrackingGeometry
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${GEOMODEL_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry StoreGateLib HGTD_Identifier HGTD_ReadoutGeometry TrkSurfaces SubDetectorEnvelopesLib)

# Install files from the package:
#atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
