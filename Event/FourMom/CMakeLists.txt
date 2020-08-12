# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FourMom )

# External dependencies:
find_package( Boost )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( FourMom
                   src/*.cxx
                   PUBLIC_HEADERS FourMom
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
		           LINK_LIBRARIES ${CLHEP_LIBRARIES} EventKernel EventPrimitives CxxUtils
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} )

atlas_add_dictionary( FourMomDict
                      FourMom/FourMomDict.h
                      FourMom/selection.xml
                      LINK_LIBRARIES FourMom )

atlas_add_test( EEtaPhiM_test
                SOURCES
                test/EEtaPhiM_test.cxx
                LINK_LIBRARIES FourMom )

atlas_add_test( IPtCotThPhiM_test
                SOURCES
                test/IPtCotThPhiM_test.cxx
                LINK_LIBRARIES FourMom )

atlas_add_test( PtEtaPhiM_test
                SOURCES
                test/PtEtaPhiM_test.cxx
                LINK_LIBRARIES FourMom )

atlas_add_test( PxPyPzE_test
                SOURCES
                test/PxPyPzE_test.cxx
                LINK_LIBRARIES FourMom )

atlas_add_executable( benchEEtaPhiM
                      test/bench_EEtaPhiM.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} FourMom )

atlas_add_executable( benchIPtCotThPhiM
                      test/bench_IPtCotThPhiM.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} FourMom )

atlas_add_executable( benchPtEtaPhiM
                      test/bench_PtEtaPhiM.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} FourMom )

atlas_add_executable( benchPxPyPzE
                      test/bench_PxPyPzE.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} FourMom )
