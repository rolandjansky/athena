################################################################################
# Package: MuonClusterOnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( MuonClusterOnTrackCreator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkTools/TrkToolInterfaces 
			  MuonSpectrometer/MuonReconstruction/MuonDataPrep/MMClusterization
                          MuonSpectrometer/MuonCalib/NSWCalib/NSWCalibTools )

# Component(s) in the package:
atlas_add_component( MuonClusterOnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib CscClusterizationLib MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces TrkSurfaces TrkEventPrimitives TrkParameters TrkPrepRawData TrkToolInterfaces )

# Install files from the package:
atlas_install_joboptions( share/*.py )

