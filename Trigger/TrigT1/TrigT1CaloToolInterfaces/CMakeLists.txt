################################################################################
# Package: TrigT1CaloToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloEvent )

# Component(s) in the package:
atlas_add_library( TrigT1CaloToolInterfaces
                   PUBLIC_HEADERS TrigT1CaloToolInterfaces
                   LINK_LIBRARIES DataModel Identifier xAODTrigL1Calo GaudiKernel AnalysisTriggerEvent TrigT1CaloCalibConditions TrigT1CaloEventLib )

