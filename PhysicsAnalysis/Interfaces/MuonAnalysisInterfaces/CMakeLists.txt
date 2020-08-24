# The name of the package:
atlas_subdir( MuonAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools   
   Event/xAOD/xAODMuon
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/PATCore )

# Component(s) in the package:
atlas_add_library( MuonAnalysisInterfacesLib
   MuonAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS MuonAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODEventInfo xAODMuon PATCoreLib PATInterfaces )

atlas_add_dictionary( MuonAnalysisInterfacesDict
   MuonAnalysisInterfaces/MuonAnalysisInterfacesDict.h
   MuonAnalysisInterfaces/selection.xml
   LINK_LIBRARIES MuonAnalysisInterfacesLib )
