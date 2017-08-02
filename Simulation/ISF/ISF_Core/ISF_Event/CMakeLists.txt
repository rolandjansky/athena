################################################################################
# Package: ISF_Event
################################################################################

# Declare the package name:
atlas_subdir( ISF_Event )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/GeoPrimitives
                          Generators/GeneratorObjects
                          GaudiKernel
                          Simulation/Barcode/BarcodeEvent )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( ISF_Event
                   src/*.cxx
                   PUBLIC_HEADERS ISF_Event
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} TestTools AthenaBaseComps AtlasDetDescr GeoPrimitives GeneratorObjects GaudiKernel )

atlas_add_test( ISFParticle_test
                SOURCES
                test/ISFParticle_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${EIGEN_LIBRARIES} TestTools AthenaBaseComps AtlasDetDescr GeoPrimitives GeneratorObjects GaudiKernel ISF_Event )
