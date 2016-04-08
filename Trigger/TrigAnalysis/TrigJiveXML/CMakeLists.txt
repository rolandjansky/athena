################################################################################
# Package: TrigJiveXML
################################################################################

# Declare the package name:
atlas_subdir( TrigJiveXML )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkEvent/TrkPrepRawData
                          Trigger/TrigAnalysis/TrigDecisionTool
                          graphics/JiveXML
                          PRIVATE
                          Event/EventInfo
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODTrigger
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          InnerDetector/InDetRecEvent/InDetPrepRawData
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigJiveXML
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps TrkPrepRawData TrigDecisionToolLib JiveXMLLib EventInfo xAODTrigL1Calo xAODTrigger GaudiKernel InDetIdentifier InDetReadoutGeometry InDetPrepRawData AnalysisTriggerEvent TrigInDetEvent TrigSteeringEvent TrigT1CaloEventLib TrigT1Interfaces )

# Install files from the package:
atlas_install_headers( TrigJiveXML )
atlas_install_joboptions( share/*.py )

