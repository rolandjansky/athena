################################################################################
# Package: MuonAGDDBase
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDDBase )

find_package( GeoModelCore )

# Component(s) in the package:
atlas_add_library( MuonAGDDBase
                   src/*.cxx
                   PUBLIC_HEADERS MuonAGDDBase
                   LINK_LIBRARIES AGDDControl AGDDKernel GaudiKernel MuonAGDDDescription StoreGateLib SGtests MuonGeoModelLib
                   PRIVATE_LINK_LIBRARIES ${GEOMODELCORE_LIBRARIES} AGDDModel )

