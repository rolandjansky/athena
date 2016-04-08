################################################################################
# Package: FourMom
################################################################################

# Declare the package name:
atlas_subdir( FourMom )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/EventKernel
                          Event/EventPrimitives
                          PRIVATE
                          AtlasTest/TestTools
                          TestPolicy )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( FourMom
                   src/*.cxx
                   PUBLIC_HEADERS FourMom
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} EventKernel EventPrimitives
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} TestTools )

atlas_add_dictionary( FourMomDict
                      FourMom/FourMomDict.h
                      FourMom/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_test( EEtaPhiM_test
                SOURCES
                test/EEtaPhiM_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_test( IPtCotThPhiM_test
                SOURCES
                test/IPtCotThPhiM_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_test( PtEtaPhiM_test
                SOURCES
                test/PtEtaPhiM_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_test( PxPyPzE_test
                SOURCES
                test/PxPyPzE_test.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_executable( benchEEtaPhiM
                      test/bench_EEtaPhiM.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_executable( benchIPtCotThPhiM
                      test/bench_IPtCotThPhiM.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_executable( benchPtEtaPhiM
                      test/bench_PtEtaPhiM.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

atlas_add_executable( benchPxPyPzE
                      test/bench_PxPyPzE.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} EventKernel EventPrimitives TestTools FourMom )

