# The name of the package:
atlas_subdir( FTagAnalysisInterfaces )

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODBTagging
   Event/xAOD/xAODJet
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/CalibrationDataInterface )

# Component(s) in the package:
atlas_add_library( MuonAnalysisInterfacesLib
   FTagAnalysisInterfaces/*.h
   INTERFACE
   PUBLIC_HEADERS FTagAnalysisInterfaces
   LINK_LIBRARIES AsgTools xAODBTagging xAODJet PATCoreLib CalibrationDataInterfaceLib )

atlas_add_dictionary( FTagAnalysisInterfacesDict
   FTagAnalysisInterfaces/FTagAnalysisInterfacesDict.h
   FTagAnalysisInterfaces/selection.xml
   LINK_LIBRARIES FTagAnalysisInterfacesLib )
