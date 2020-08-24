################################################################################
# Package: AnalysisTriggerAlgs
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTriggerAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Event/EventInfo
   Event/xAOD/xAODTrigL1Calo
   Event/xAOD/xAODTrigger
   Event/xAOD/xAODTriggerCnv
   GaudiKernel
   PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
   Trigger/TrigConfiguration/TrigConfInterfaces
   Trigger/TrigConfiguration/TrigConfL1Data
   Trigger/TrigT1/TrigT1CaloEvent
   Trigger/TrigT1/TrigT1CaloToolInterfaces
   Trigger/TrigT1/TrigT1Interfaces
   Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( AnalysisTriggerAlgs
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib
   EventInfo xAODTrigL1Calo xAODTrigger GaudiKernel AnalysisTriggerEvent
   TrigConfL1Data TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Interfaces
   TrigT1Result TrigConfInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
