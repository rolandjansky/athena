################################################################################
# Package: InDetExtensionProcessor
################################################################################

# Declare the package name:
atlas_subdir( InDetExtensionProcessor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterUtils
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( InDetExtensionProcessor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkEventPrimitives TrkTrack TrkFitterUtils StoreGateLib SGtests TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkFitterInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetExtensionProcessor )

