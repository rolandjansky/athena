################################################################################
# Package: MdtDriftCircleOnTrackCreator
################################################################################

# Declare the package name:
atlas_subdir( MdtDriftCircleOnTrackCreator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkSpaceTimePoint
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibData
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          Tracking/TrkDetDescr/TrkDistortedSurfaces
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkParameters )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )

# Component(s) in the package:
atlas_add_component( MdtDriftCircleOnTrackCreator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps GaudiKernel MuonRIO_OnTrack MuonRecToolInterfaces TrkEventPrimitives TrkSpaceTimePoint TrkToolInterfaces StoreGateLib SGtests Identifier EventPrimitives MdtCalibData MdtCalibSvcLib MuonCalibEvent MuonReadoutGeometry MuonIdHelpersLib MuonPrepRawData TrkDistortedSurfaces TrkSurfaces TrkParameters )

# Install files from the package:
atlas_install_headers( MdtDriftCircleOnTrackCreator )
atlas_install_joboptions( share/*.py )

