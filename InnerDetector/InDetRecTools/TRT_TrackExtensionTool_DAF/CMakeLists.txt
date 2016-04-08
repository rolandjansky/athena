################################################################################
# Package: TRT_TrackExtensionTool_DAF
################################################################################

# Declare the package name:
atlas_subdir( TRT_TrackExtensionTool_DAF )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/StoreGate
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecTools/InDetCompetingRIOsOnTrackTool
                          MagneticField/MagFieldInterfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TRT_TrackExtensionTool_DAF
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetPrepRawData InDetRecToolInterfaces TrkGeometry TrkParameters StoreGateLib SGtests InDetIdentifier InDetReadoutGeometry MagFieldInterfaces TrkMeasurementBase TrkTrack TrkExInterfaces TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( TRT_TrackExtensionTool_DAF )

