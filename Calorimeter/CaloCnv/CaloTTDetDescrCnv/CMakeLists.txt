################################################################################
# Package: CaloTTDetDescrCnv
################################################################################

# Declare the package name:
atlas_subdir( CaloTTDetDescrCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTTDetDescr
                          Calorimeter/CaloTriggerTool
                          Control/StoreGate
                          DetectorDescription/DetDescrCnvSvc
                          LArCalorimeter/LArTools )

# Component(s) in the package:
atlas_add_component( CaloTTDetDescrCnv
                     src/*.cxx
                     LINK_LIBRARIES GaudiKernel CaloDetDescrLib CaloIdentifier CaloTTDetDescr CaloTriggerToolLib StoreGateLib SGtests DetDescrCnvSvcLib LArToolsLib )

