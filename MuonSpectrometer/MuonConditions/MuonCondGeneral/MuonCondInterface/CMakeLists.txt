################################################################################
# Package: MuonCondInterface
################################################################################

# Declare the package name:
atlas_subdir( MuonCondInterface )

# Component(s) in the package:
atlas_add_library( MuonCondInterface
                   PUBLIC_HEADERS MuonCondInterface
                   LINK_LIBRARIES AthenaKernel GeoPrimitives GaudiKernel MuonAlignmentData StoreGateLib SGtests CxxUtils )

