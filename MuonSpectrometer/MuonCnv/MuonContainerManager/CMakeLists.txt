################################################################################
# Package: MuonContainerManager
################################################################################

# Declare the package name:
atlas_subdir( MuonContainerManager )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/MuonRDO )

# Component(s) in the package:
atlas_add_library( MuonContainerManager
                   src/*.cxx
                   PUBLIC_HEADERS MuonContainerManager
                   LINK_LIBRARIES GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES MuonRDO )

