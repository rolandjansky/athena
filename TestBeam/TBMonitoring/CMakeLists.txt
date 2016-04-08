################################################################################
# Package: TBMonitoring
################################################################################

# Declare the package name:
atlas_subdir( TBMonitoring )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Control/AthenaMonitoring
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          TestBeam/TBEvent
                          TestBeam/TBUtils
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloUtils
                          Event/EventInfo
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArTools )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TBMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS TBMonitoring
                   INCLUDE_DIRS ${AIDA_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloGeoHelpers CaloIdentifier Identifier GaudiKernel TBEvent AthenaMonitoringLib StoreGateLib SGtests CaloUtilsLib LArToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloEvent EventInfo LArIdentifier )

atlas_add_component( TBMonitoring
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloGeoHelpers CaloIdentifier AthenaMonitoringLib StoreGateLib SGtests Identifier GaudiKernel TBEvent CaloEvent CaloUtilsLib EventInfo LArIdentifier LArToolsLib TBMonitoringLib )

# Install files from the package:
atlas_install_joboptions( share/*.txt share/*.py )

