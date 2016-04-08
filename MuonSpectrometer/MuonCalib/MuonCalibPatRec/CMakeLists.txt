################################################################################
# Package: MuonCalibPatRec
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibPatRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCalib/MuonCalibEvent
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkSegment
                          PRIVATE
                          DetectorDescription/Identifier
                          MuonSpectrometer/MuonCalib/MdtCalib/MdtCalibSvc
                          MuonSpectrometer/MuonCalib/MuonCalibEventBase
                          MuonSpectrometer/MuonCalib/MuonCalibITools
                          MuonSpectrometer/MuonCalib/MuonCalibNtuple
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters )

# Component(s) in the package:
atlas_add_library( MuonCalibPatRecLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibPatRec
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonCalibEvent MuonSegment TrkSegment MdtCalibSvcLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier MuonCalibEventBase MuonCalibITools MuonCalibNtuple MuonReadoutGeometry MuonPattern MuonRIO_OnTrack MuonRecToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters )

atlas_add_component( MuonCalibPatRec
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonCalibEvent MuonSegment TrkSegment Identifier MdtCalibSvcLib MuonCalibEventBase MuonCalibITools MuonCalibNtuple MuonReadoutGeometry MuonIdHelpersLib MuonPattern MuonRIO_OnTrack MuonRecToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters MuonCalibPatRecLib )

