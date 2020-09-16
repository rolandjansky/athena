# The name of the package:
atlas_subdir( MuonAnalysisInterfaces )

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
