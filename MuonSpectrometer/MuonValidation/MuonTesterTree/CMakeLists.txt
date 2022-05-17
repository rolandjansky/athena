################################################################################
# Package: MuonTesterTree
################################################################################

# Declare the package name:
atlas_subdir( MuonTesterTree )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( MuonTesterTreeLib
                   MuonTesterTree/*.h Root/*.cxx
                   PUBLIC_HEADERS MuonTesterTree
                  LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEventInfo xAODMuon xAODTruth GeoPrimitives TrkEventPrimitives MuonIdHelpersLib AthenaKernel MuonSimEvent)
 


