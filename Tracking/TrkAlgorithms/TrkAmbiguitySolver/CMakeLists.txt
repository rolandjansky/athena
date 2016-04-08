################################################################################
# Package: TrkAmbiguitySolver
################################################################################

# Declare the package name:
atlas_subdir( TrkAmbiguitySolver )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkAmbiguitySolver
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps DataModel GaudiKernel TrkTrack StoreGateLib SGtests TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TrkAmbiguitySolver )

