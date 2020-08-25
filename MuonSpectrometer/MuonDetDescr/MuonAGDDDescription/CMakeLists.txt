################################################################################
# Package: MuonAGDDDescription
################################################################################

# Declare the package name:
atlas_subdir( MuonAGDDDescription )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/AGDD/AGDDKernel
                          DetectorDescription/AGDD/AGDDModel
                        )

# Component(s) in the package:
atlas_add_library( MuonAGDDDescription
                   src/*.cxx
                   PUBLIC_HEADERS MuonAGDDDescription
                   LINK_LIBRARIES AGDDKernel AGDDModel )

