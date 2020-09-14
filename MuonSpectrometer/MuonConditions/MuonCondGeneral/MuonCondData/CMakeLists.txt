################################################################################
# Package: MuonCondData
################################################################################

# Declare the package name:
atlas_subdir( MuonCondData )

# Component(s) in the package:
atlas_add_library( MuonCondData
                   src/*.cxx
                   PUBLIC_HEADERS MuonCondData
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthContainers AthenaPoolUtilities Identifier GaudiKernel StoreGateLib SGtests MuonIdHelpersLib 
                   PRIVATE_LINK_LIBRARIES EventInfo )

