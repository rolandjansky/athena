################################################################################
# Package: TrigT1CaloMappingTools
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloMappingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/Identifier
                          GaudiKernel
                          Trigger/TrigT1/TrigT1CaloMappingToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrigT1CaloMappingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloIdentifier CaloTriggerToolLib AthenaBaseComps StoreGateLib SGtests Identifier GaudiKernel )

