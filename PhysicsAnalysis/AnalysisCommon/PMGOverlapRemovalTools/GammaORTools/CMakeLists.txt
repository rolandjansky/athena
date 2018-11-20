################################################################################
# Package: GammaORTools
################################################################################

# Declare the package name:
atlas_subdir( GammaORTools )

# Extra dependencies, based on the environment:
if( XAOD_STANDALONE )
    set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel PhysicsAnalysis/POOLRootAccess)
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTruth
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PRIVATE
   ${extra_deps} 
   )


# Libraries in the package:
atlas_add_library( GammaORToolsLib
   GammaORTools/*.h Root/*.cxx
   PUBLIC_HEADERS GammaORTools
   INCLUDE_DIRS 
   LINK_LIBRARIES AsgTools xAODTruth xAODEventInfo MCTruthClassifierLib )

if( NOT XAOD_STANDALONE )
  atlas_add_component( GammaORTools
    src/components/*.cxx
    LINK_LIBRARIES AthenaBaseComps GaudiKernel GammaORToolsLib)
endif()

atlas_add_dictionary( GammaORToolsDict
  GammaORTools/GammaORToolsDict.h
  GammaORTools/selection.xml
  LINK_LIBRARIES GammaORToolsLib )
