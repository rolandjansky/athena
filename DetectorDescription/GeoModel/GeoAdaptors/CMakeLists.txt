################################################################################
# Package: GeoAdaptors
################################################################################

# Declare the package name:
atlas_subdir( GeoAdaptors )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloSimEvent
                          Control/StoreGate
                          DetectorDescription/Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetSimEvent
                          LArCalorimeter/LArSimEvent
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonSimEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( GeoAdaptors
                   src/*.cxx
                   PUBLIC_HEADERS GeoAdaptors
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier CaloSimEvent Identifier InDetIdentifier InDetReadoutGeometry InDetSimEvent LArSimEvent MuonReadoutGeometry MuonPrepRawData MuonSimEvent CaloDetDescrLib StoreGateLib SGtests MuonIdHelpersLib )

