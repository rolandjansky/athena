################################################################################
# Package: MuonCalibPatRec
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibPatRec )

# Component(s) in the package:
atlas_add_library( MuonCalibPatRecLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibPatRec
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonCalibEvent MuonSegment TrkSegment MdtCalibSvcLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier MuonCalibEventBase MuonCalibITools MuonCalibNtuple MuonReadoutGeometry MuonPattern MuonRIO_OnTrack MuonRecToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters )

atlas_add_component( MuonCalibPatRec
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonCalibEvent MuonSegment TrkSegment Identifier MdtCalibSvcLib MuonCalibEventBase MuonCalibITools MuonCalibNtuple MuonReadoutGeometry MuonIdHelpersLib MuonPattern MuonRIO_OnTrack MuonRecToolInterfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters MuonCalibPatRecLib )

