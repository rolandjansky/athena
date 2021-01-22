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
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_ReadoutGeometry
                          HighGranularityTimingDetector/HGTD_DetDescr/HGTD_Identifier
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/PixelReadoutGeometry
                          InnerDetector/InDetDetDescr/SCT_ReadoutGeometry
                          InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
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
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier CaloSimEvent Identifier HGTD_ReadoutGeometry HGTD_Identifier InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry SCT_ReadoutGeometry PixelReadoutGeometry InDetSimEvent LArSimEvent MuonReadoutGeometry MuonPrepRawData MuonSimEvent CaloDetDescrLib StoreGateLib SGtests MuonIdHelpersLib )

