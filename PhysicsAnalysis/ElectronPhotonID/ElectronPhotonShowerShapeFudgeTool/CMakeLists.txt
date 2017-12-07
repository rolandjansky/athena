# $Id: CMakeLists.txt 776838 2016-10-06 12:21:19Z krasznaa $
################################################################################
# Package: ElectronPhotonShowerShapeFudgeTool
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonShowerShapeFudgeTool )

# Extra dependencies based on the environment:
set( extra_dep )
if( NOT XAOD_STANDALONE )
   set( extra_dep GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces	
   PRIVATE
   Tools/PathResolver
   ${extra_dep} )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist RIO Gui Gpad )

# Component(s) in the package:
atlas_add_library( ElectronPhotonShowerShapeFudgeToolLib
   ElectronPhotonShowerShapeFudgeTool/*.h
   ElectronPhotonShowerShapeFudgeTool/*.icc Root/*.cxx
   PUBLIC_HEADERS ElectronPhotonShowerShapeFudgeTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma PATInterfaces EgammaAnalysisInterfacesLib
   PRIVATE_LINK_LIBRARIES PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronPhotonShowerShapeFudgeTool
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel ElectronPhotonShowerShapeFudgeToolLib )
endif()

atlas_add_dictionary( ElectronPhotonShowerShapeFudgeToolDict
   ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeToolDict.h
   ElectronPhotonShowerShapeFudgeTool/selection.xml
   LINK_LIBRARIES ElectronPhotonShowerShapeFudgeToolLib )

# Install files from the package:
atlas_install_data( data/*.conf data/*.root )
