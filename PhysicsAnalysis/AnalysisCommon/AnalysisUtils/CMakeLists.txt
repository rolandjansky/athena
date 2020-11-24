# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisUtils )

if (NOT XAOD_STANDALONE)

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_library( AnalysisUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel CxxUtils AthContainers EventKernel NavFourMom GaudiKernel ParticleEvent AraTool McParticleEvent
                   PRIVATE_LINK_LIBRARIES Particle egammaEvent )

atlas_add_component( AnalysisUtils
                     src/components/*.cxx
                     LINK_LIBRARIES AnalysisUtilsLib )

atlas_add_dictionary( AnalysisUtilsDict
                      AnalysisUtils/AnalysisUtilsDict.h
                      AnalysisUtils/selection.xml
                      LINK_LIBRARIES AnalysisUtilsLib )

else()

find_package( Boost )

# Component(s) in the package:
atlas_add_library( AnalysisUtilsLib
                   INTERFACE
                   PUBLIC_HEADERS AnalysisUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} CxxUtils AthContainers )

endif()
