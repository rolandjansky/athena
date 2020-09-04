################################################################################
# Package: MuonCombinePatternTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinePatternTools )

# Component(s) in the package:
atlas_add_library( MuonCombinePatternToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCombinePatternTools
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonHoughPatternEvent MuonRecToolInterfaces MuonIdHelpersLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES CxxUtils MuonReadoutGeometry MuonPattern MuonPrepRawData TrkSurfaces TrkParameters )

atlas_add_component( MuonCombinePatternTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib MuonHoughPatternEvent MuonRecToolInterfaces CxxUtils StoreGateLib SGtests MuonReadoutGeometry MuonPattern MuonPrepRawData TrkSurfaces TrkParameters MuonCombinePatternToolsLib )

