################################################################################
# Package: TileTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( TileTrackingGeometry )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloTrackingGeometry
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/StoreGate
                          TileCalorimeter/TileDetDescr
                          Tracking/TrkDetDescr/TrkDetDescrGeoModelCnv
                          Tracking/TrkDetDescr/TrkDetDescrUtils
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkDetDescr/TrkVolumes )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( TileTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GEOMODELCORE_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrInterfaces CaloDetDescrLib StoreGateLib SGtests GeoPrimitives TileDetDescr TrkDetDescrGeoModelCnv TrkDetDescrUtils TrkGeometry TrkSurfaces TrkVolumes )

# Install files from the package:
atlas_install_headers( TileTrackingGeometry )
atlas_install_python_modules( python/*.py )

