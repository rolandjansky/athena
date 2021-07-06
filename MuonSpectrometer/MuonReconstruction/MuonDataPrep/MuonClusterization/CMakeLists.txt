################################################################################
# Package: MuonClusterization
################################################################################

# Declare the package name:
atlas_subdir( MuonClusterization )

# Component(s) in the package:
atlas_add_library( MuonClusterizationLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonClusterization
                   LINK_LIBRARIES GeoPrimitives Identifier GaudiKernel MuonReadoutGeometry MuonPrepRawData MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps EventPrimitives )

atlas_add_component( MuonClusterization
                     src/components/*.cxx
                     LINK_LIBRARIES GeoPrimitives Identifier GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData AthenaBaseComps EventPrimitives MuonClusterizationLib )

