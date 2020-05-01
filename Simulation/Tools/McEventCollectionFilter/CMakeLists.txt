################################################################################
# Package: McEventCollectionFilter
################################################################################

# Declare the package name:
atlas_subdir( McEventCollectionFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          Generators/AtlasHepMC
                          Generators/GeneratorObjects
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( McEventCollectionFilterLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel GeneratorObjects InDetSimEvent MuonSimEvent )

atlas_add_component( McEventCollectionFilter
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives GaudiKernel GeneratorObjects InDetSimEvent MuonSimEvent McEventCollectionFilterLib )

atlas_install_python_modules( python/*.py )
