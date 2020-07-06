################################################################################
# Package: MuonTGMeasAssocAlg
################################################################################

# Declare the package name:
atlas_subdir( MuonTGMeasAssocAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonTGRecTools
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          PRIVATE
                          Event/EventPrimitives
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuonTGMeasAssocAlg
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkGeometry TrkSurfaces TrkPrepRawData TrkSegment TrkTrack TrkExInterfaces EventPrimitives MuonSegment TrkEventPrimitives TrkMeasurementBase TrkParameters MuonTGRecToolsLib )

# Install files from the package:
atlas_install_headers( MuonTGMeasAssocAlg )
atlas_install_joboptions( share/*.py )

