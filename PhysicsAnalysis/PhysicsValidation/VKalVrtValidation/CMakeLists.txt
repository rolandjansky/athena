# The name of the package:
atlas_subdir( VKalVrtValidation )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaBaseComps Control/AthAnalysisBaseComps GaudiKernel )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC 
   PRIVATE
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/JpsiUpsilonTools
   Tracking/TrkVertexFitter/TrkVKalVrtFitter
   ${extra_deps} )

# Externals used by the package:
find_package( ROOT COMPONENTS Core  )

find_package( HepPDT )

# Libraries in the package:
atlas_add_library( VKalVrtValidationLib
   VKalVrtValidation/*.h src/*.cxx
   NO_PUBLIC_HEADERS 
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODBase AthenaBaseComps AthAnalysisBaseCompsLib xAODTracking JpsiUpsilonToolsLib
   )

if( NOT XAOD_STANDALONE )
   atlas_add_component( VKalVrtValidation
   src/components/*.cxx
   LINK_LIBRARIES VKalVrtValidationLib )
endif()


# Install files from the package:
atlas_install_joboptions( share/*.py )


