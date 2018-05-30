# The name of the package:
atlas_subdir( JetAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
    PUBLIC
    Control/AthToolSupport/AsgTools
    Event/xAOD/xAODJet
    PhysicsAnalysis/AnalysisCommon/PATCore )

# Component(s) in the package:
atlas_add_library( JetAnalysisInterfacesLib
   JetAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS JetAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODJet PATCoreLib PATInterfaces )

atlas_add_dictionary( JetAnalysisInterfacesDict
   JetAnalysisInterfaces/JetAnalysisInterfacesDict.h
   JetAnalysisInterfaces/selection.xml
   LINK_LIBRARIES JetAnalysisInterfacesLib )
