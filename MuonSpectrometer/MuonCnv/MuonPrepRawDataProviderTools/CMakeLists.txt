################################################################################
# Package: MuonPrepRawDataProviderTools
################################################################################

# Declare the package name:
atlas_subdir( MuonPrepRawDataProviderTools )

# Component(s) in the package:
atlas_add_library( MuonPrepRawDataProviderToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonPrepRawDataProviderTools
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel MuonLayerEvent MuonPrepRawData MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES EventPrimitives MuonDetDescrUtils MuonReadoutGeometry MuonRIO_OnTrack MuonRecToolInterfaces )

atlas_add_component( MuonPrepRawDataProviderTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel MuonIdHelpersLib MuonLayerEvent MuonPrepRawData EventPrimitives MuonDetDescrUtils MuonReadoutGeometry MuonRIO_OnTrack MuonRecToolInterfaces MuonPrepRawDataProviderToolsLib )

