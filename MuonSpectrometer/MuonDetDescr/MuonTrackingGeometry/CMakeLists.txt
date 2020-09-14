################################################################################
# Package: MuonTrackingGeometry
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackingGeometry )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_component( MuonTrackingGeometry
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${GEOMODELCORE_LIBRARIES} AthenaBaseComps GaudiKernel TrkDetDescrGeoModelCnv TrkDetDescrInterfaces TrkDetDescrUtils TrkGeometry TrkVolumes StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonIdHelpersLib TrkGeometrySurfaces TrkSurfaces GeoModelUtilities SubDetectorEnvelopesLib )

# Install files from the package:
atlas_install_headers( MuonTrackingGeometry )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

