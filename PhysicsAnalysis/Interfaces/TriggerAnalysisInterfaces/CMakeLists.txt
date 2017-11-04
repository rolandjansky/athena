# copied from AsgAnalysisInterfaces
	
# The name of the package:
atlas_subdir( TriggerAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODMuon
   PhysicsAnalysis/AnalysisCommon/PATInterfaces )
	
# External(s):
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( TriggerAnalysisInterfaces
   TriggerAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS TriggerAnalysisInterfaces
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODMuon PATInterfaces )

atlas_add_dictionary( TriggerAnalysisInterfacesDict
   TriggerAnalysisInterfaces/TriggerAnalysisInterfacesDict.h
   TriggerAnalysisInterfaces/selection.xml
   LINK_LIBRARIES TriggerAnalysisInterfaces )
