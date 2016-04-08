################################################################################
# Package: TrkGlobalChi2AlignTools
################################################################################

# Declare the package name:
atlas_subdir( TrkGlobalChi2AlignTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkAlignment/TrkAlignInterfaces
                          PRIVATE
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          Tracking/TrkAlignment/TrkAlignEvent
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# tag rootMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( TrkGlobalChi2AlignTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel StoreGateLib SGtests xAODEventInfo TrkAlignEvent TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkTrack )

# Install files from the package:
atlas_install_headers( TrkGlobalChi2AlignTools )
atlas_install_python_modules( python/*.py )

