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
                          Generators/GeneratorObjects
                          InnerDetector/InDetSimEvent
                          MuonSpectrometer/MuonSimEvent )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( McEventCollectionFilterLib
                   src/*.cxx
                   PUBLIC_HEADERS McEventCollectionFilter
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel GeneratorObjects InDetSimEvent MuonSimEvent )

atlas_add_component( McEventCollectionFilter
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives GaudiKernel GeneratorObjects InDetSimEvent MuonSimEvent McEventCollectionFilterLib )

