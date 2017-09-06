################################################################################
# Package: VP1Utils
################################################################################
# Author: Thomas Kittelmann
# Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
################################################################################

# Declare the package name:
atlas_subdir( VP1Utils )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/CLIDSvc
   Control/SGTools
   Control/StoreGate
   DetectorDescription/GeoModel/GeoModelKernel
   DetectorDescription/GeoPrimitives
   Event/EventPrimitives
   GaudiKernel
   graphics/VP1/VP1Base
   PRIVATE
   Calorimeter/CaloIdentifier
   Calorimeter/CaloTTDetDescr
   Control/CxxUtils
   DetectorDescription/AtlasDetDescr
   DetectorDescription/GeoModel/GeoModelUtilities
   DetectorDescription/GeoModel/GeoSpecialShapes
   DetectorDescription/Identifier
   InnerDetector/InDetDetDescr/InDetIdentifier
   InnerDetector/InDetDetDescr/InDetReadoutGeometry
   InnerDetector/InDetRecEvent/InDetRIO_OnTrack
   LArCalorimeter/LArGeoModel/LArReadoutGeometry
   MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
   MuonSpectrometer/MuonIdHelpers
   MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
   TileCalorimeter/TileDetDescr
   Tracking/TrkDetDescr/TrkSurfaces
   Tracking/TrkEvent/TrkRIO_OnTrack
   graphics/VP1/VP1HEPVis )

# External dependencies:
find_package( CLHEP )
find_package( Coin3D )
find_package( Eigen )
find_package( HepPDT )
find_package( Qt5 COMPONENTS Core )

# CLHEP definitions:
add_definitions( -DCLHEP_MAX_MIN_DEFINED
                 -DCLHEP_ABS_DEFINED
                 -DCLHEP_SQR_DEFINED )

# Generate MOC files automatically:
set( CMAKE_AUTOMOC TRUE )

# Component(s) in the package:
atlas_add_library( VP1Utils VP1Utils/*.h src/*.cxx src/*.cpp
   PUBLIC_HEADERS VP1Utils
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${COIN3D_INCLUDE_DIRS}
   ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools GeoModelKernel EventPrimitives
   GaudiKernel VP1Base StoreGateLib SGtests AthDSoCallBacks MuonIdHelpersLib
   GeoPrimitives Qt5::Core
   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${COIN3D_LIBRARIES}
   ${EIGEN_LIBRARIES} CaloIdentifier CaloTTDetDescr CxxUtils AtlasDetDescr
   GeoModelUtilities GeoSpecialShapes Identifier InDetIdentifier
   InDetReadoutGeometry InDetRIO_OnTrack LArReadoutGeometry
   MuonReadoutGeometry MuonRIO_OnTrack TileDetDescr TrkSurfaces
   TrkRIO_OnTrack VP1HEPVis )
