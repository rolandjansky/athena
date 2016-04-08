################################################################################
# Package: OverlayAlgBase
################################################################################

# Declare the package name:
atlas_subdir( OverlayAlgBase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel )

# Component(s) in the package:
atlas_add_library( OverlayAlgBase
                   src/*.cxx
                   PUBLIC_HEADERS OverlayAlgBase
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests )

