################################################################################
# Package: RPC_LinearSegmentMakerTool
################################################################################

# Declare the package name:
atlas_subdir( RPC_LinearSegmentMakerTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLinearSegmentMakerUtilities
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkRoad )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( RPC_LinearSegmentMakerTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel MuonRecToolInterfaces StoreGateLib SGtests GeoPrimitives MuonReadoutGeometry MuonRIO_OnTrack MuonSegment MuonLinearSegmentMakerUtilities TrkSurfaces TrkEventPrimitives TrkRoad )

# Install files from the package:
atlas_install_headers( RPC_LinearSegmentMakerTool )

