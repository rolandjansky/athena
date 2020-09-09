################################################################################
# Package: MuonAGDDDescription
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDDDescription )

# Component(s) in the package:
atlas_add_library( MuonAGDDDescription
                   src/*.cxx
                   PUBLIC_HEADERS MuonAGDDDescription
                   LINK_LIBRARIES AGDDKernel AGDDModel )

