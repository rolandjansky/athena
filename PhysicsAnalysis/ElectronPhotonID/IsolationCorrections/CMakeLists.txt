# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IsolationCorrections )

# Extra dependencies, based on the environment:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO Matrix)
find_package( Boost )

atlas_add_library( IsolationCorrectionsLib
   IsolationCorrections/*.h Root/*.cxx
   PUBLIC_HEADERS IsolationCorrections
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools xAODEgamma xAODEventInfo xAODEventShape
   xAODPrimitives xAODTracking PATInterfaces PATCoreAcceptLib
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} PathResolver ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( IsolationCorrections
      src/components/*.cxx
      LINK_LIBRARIES IsolationCorrectionsLib )
endif()

atlas_add_dictionary( IsolationCorrectionsDict
   IsolationCorrections/IsolationCorrectionsDict.h
   IsolationCorrections/selection.xml
   LINK_LIBRARIES IsolationCorrectionsLib )
