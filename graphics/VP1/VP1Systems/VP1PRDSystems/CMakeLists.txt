# $Id: CMakeLists.txt 728680 2016-03-09 15:15:07Z krasznaa $
################################################################################
# Package: VP1PRDSystems
################################################################################

# Declare the package name:
atlas_subdir( VP1PRDSystems )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   DetectorDescription/GeoModel/GeoModelKernel
   DetectorDescription/GeoPrimitives
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetRecEvent/InDetPrepRawData
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
   Tracking/TrkEvent/TrkPrepRawData
   Tracking/TrkEvent/TrkSpacePoint
   graphics/VP1/VP1Base
   graphics/VP1/VP1Utils
   PRIVATE
   Control/DataModel
   DetectorDescription/GeoModel/GeoModelUtilities
   Event/EventPrimitives
   InnerDetector/InDetDetDescr/InDetIdentifier
   MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
   MuonSpectrometer/MuonIdHelpers
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkCompetingRIOsOnTrack
   Tracking/TrkEvent/TrkMeasurementBase
   Tracking/TrkEvent/TrkRIO_OnTrack
   Tracking/TrkEvent/TrkSegment
   Tracking/TrkEvent/TrkTrack
   graphics/VP1/VP1HEPVis
   graphics/VP1/VP1Systems/VP1GuideLineSystems )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Qt5 COMPONENTS Core Gui Widgets)

# Generate UI files automatically:
set( CMAKE_AUTOUIC TRUE )
# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1PRDSystems VP1PRDSystems/*.h src/*.cxx
   PUBLIC_HEADERS VP1PRDSystems
   PRIVATE_INCLUDE_DIRS ${COIN3D_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   ${CMAKE_CURRENT_BINARY_DIR}
   LINK_LIBRARIES GeoModelKernel GeoPrimitives InDetReadoutGeometry
   InDetPrepRawData MuonPrepRawData TrkPrepRawData TrkSpacePoint VP1Base
   VP1Utils MuonIdHelpersLib Qt5::Core Qt5::Gui
   PRIVATE_LINK_LIBRARIES ${COIN3D_LIBRARIES} ${CLHEP_LIBRARIES} DataModel
   GeoModelUtilities EventPrimitives InDetIdentifier MuonReadoutGeometry
   TrkSurfaces TrkCompetingRIOsOnTrack TrkMeasurementBase TrkRIO_OnTrack
   TrkSegment TrkTrack VP1HEPVis VP1GuideLineSystems  )
