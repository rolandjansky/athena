################################################################################
# Package: IsolationCorrections
################################################################################

# Declare the package name:
atlas_subdir( IsolationCorrections )

# Extra dependencies, based on the environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel 
   Control/AthenaBaseComps	       
   Control/AthAnalysisBaseComps)
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   Event/xAOD/xAODMetaData
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/PATCore	
   PRIVATE
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( Boost )

atlas_add_library( IsolationCorrectionsLib
   IsolationCorrections/*.h Root/*.cxx
   PUBLIC_HEADERS IsolationCorrections
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODEventInfo
   xAODPrimitives xAODTracking PATInterfaces PATCoreLib
   PRIVATE_LINK_LIBRARIES PathResolver xAODMetaData)

if( NOT XAOD_STANDALONE )
   atlas_add_component( IsolationCorrections
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps AthAnalysisBaseCompsLib IsolationCorrectionsLib )
endif()

atlas_add_dictionary( IsolationCorrectionsDict
   IsolationCorrections/IsolationCorrectionsDict.h
   IsolationCorrections/selection.xml
   LINK_LIBRARIES IsolationCorrectionsLib )

# Install files from the package:
atlas_install_data( data/*.root )

