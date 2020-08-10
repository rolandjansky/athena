################################################################################
# Package: TrkAlignGenAlgs
################################################################################

# Declare the package name:
atlas_subdir( TrkAlignGenAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkAlignment/TrkAlignInterfaces
                          PRIVATE
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_component( TrkAlignGenAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests xAODEventInfo TrkAlignEvent TrkTrack TrkAlignInterfaces )

# Install files from the package:
atlas_install_headers( TrkAlignGenAlgs )
atlas_install_python_modules( python/*.py )

