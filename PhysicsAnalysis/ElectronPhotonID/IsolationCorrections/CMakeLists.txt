################################################################################
# Package: IsolationCorrections
################################################################################

# Declare the package name:
atlas_subdir( IsolationCorrections )

# Extra dependencies, based on the environment:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( Boost )

atlas_add_library( IsolationCorrectionsLib
   IsolationCorrections/*.h Root/*.cxx
   PUBLIC_HEADERS IsolationCorrections
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo
   xAODPrimitives xAODTracking PATInterfaces PATCoreLib
   PRIVATE_LINK_LIBRARIES PathResolver xAODMetaData ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( IsolationCorrections
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps AthAnalysisBaseCompsLib IsolationCorrectionsLib )
endif()

atlas_add_dictionary( IsolationCorrectionsDict
   IsolationCorrections/IsolationCorrectionsDict.h
   IsolationCorrections/selection.xml
   LINK_LIBRARIES IsolationCorrectionsLib )
