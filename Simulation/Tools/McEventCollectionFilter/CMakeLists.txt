################################################################################
# Package: McEventCollectionFilter
################################################################################

# Declare the package name:
atlas_subdir( McEventCollectionFilter )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( McEventCollectionFilterLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps GeneratorObjects InDetSimEvent MuonSimEvent StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${EIGEN_LIBRARIES} GeoPrimitives GaudiKernel )

atlas_add_component( McEventCollectionFilter
                     src/components/*.cxx
                     LINK_LIBRARIES McEventCollectionFilterLib )

atlas_install_python_modules( python/*.py )
