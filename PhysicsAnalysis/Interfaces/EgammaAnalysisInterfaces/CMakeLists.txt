# The name of the package:
atlas_subdir( EgammaAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   PhysicsAnalysis/AnalysisCommon/PATInterfaces 
   PhysicsAnalysis/AnalysisCommon/PATCore
)

# External(s):
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( EgammaAnalysisInterfaces
   EgammaAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS EgammaAnalysisInterfaces
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma PATInterfaces PATCoreLib)

atlas_add_dictionary( EgammaAnalysisInterfacesDict
   EgammaAnalysisInterfaces/EgammaAnalysisInterfacesDict.h
   EgammaAnalysisInterfaces/selection.xml
   LINK_LIBRARIES EgammaAnalysisInterfaces )

atlas_add_dictionary( EgammaAnalysisInterfacesEnumDict
   EgammaAnalysisInterfaces/EgammaAnalysisInterfacesEnumDict.h
   EgammaAnalysisInterfaces/selectionEnum.xml
   LINK_LIBRARIES EgammaAnalysisInterfaces )

