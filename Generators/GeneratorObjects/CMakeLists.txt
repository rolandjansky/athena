# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GeneratorObjects )

# External dependencies:
find_package( CLHEP )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( GeneratorObjects
                   src/HepMcParticleLink.cxx
                   src/McEventCollection.cxx
                   src/HijingEventParams.cxx
                   PUBLIC_HEADERS GeneratorObjects
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaKernel AtlasHepMCLib AthContainers AthLinks SGTools xAODTruth GaudiKernel RootUtils StoreGateLib )

atlas_add_dictionary( GeneratorObjectsDict
                      GeneratorObjects/McEventDict.h
                      GeneratorObjects/selection.xml
                      LINK_LIBRARIES GeneratorObjects RootUtils )

# Install a dummy rootmap file to suppress loading the rootmap from hepmc3,
# which defines dictionary entries for HepMC types that clash with
# what we generate here (and result in ROOT giving duplicate rootmap
# entry warnings).  See !31633.
atlas_install_generic ( share/libHepMC3rootIO.rootmap
                        DESTINATION ${CMAKE_INSTALL_LIBDIR}
                        BUILD_DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} )

atlas_add_test( HepMcParticleLink_test
                SOURCES test/HepMcParticleLink_test.cxx
                INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GTEST_LIBRARIES} CxxUtils GeneratorObjects TestTools )

atlas_add_test( CachedParticlePtr_test
                SOURCES test/CachedParticlePtr_test.cxx
                LINK_LIBRARIES GeneratorObjects )
