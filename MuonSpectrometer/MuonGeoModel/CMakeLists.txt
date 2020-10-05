################################################################################
# Package: MuonGeoModel
################################################################################

# Declare the package name:
atlas_subdir( MuonGeoModel )

# External dependencies:
find_package( Eigen )
find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( MuonGeoModelLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonGeoModel
                   LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects StoreGateLib SGtests MuonIdHelpersLib MuonAGDDDescription AGDDKernel AmdcDbLib
                   PRIVATE_LINK_LIBRARIES AthenaPoolUtilities IdDictDetDescr MuonDetDescrUtils )

atlas_add_component( MuonGeoModel
                     src/components/*.cxx
                     LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AthenaKernel StoreGateLib SGtests GeoModelUtilities GaudiKernel MuonCondInterface MuonReadoutGeometry MuonGMdbObjects AthenaPoolUtilities AGDDKernel IdDictDetDescr MuonAGDDDescription MuonDetDescrUtils MuonIdHelpersLib MuonGeoModelLib AmdcDbLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

