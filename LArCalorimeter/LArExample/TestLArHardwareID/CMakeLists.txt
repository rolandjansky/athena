################################################################################
# Package: TestLArHardwareID
################################################################################

# Declare the package name:
atlas_subdir( TestLArHardwareID )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/RegistrationServices
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaKernel
                          Control/StoreGate
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecConditions
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_component( TestLArHardwareID
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel CaloIdentifier CaloTriggerToolLib AthenaKernel StoreGateLib SGtests LArCablingLib LArIdentifier LArRecConditions PathResolver RegistrationServicesLib )

# Install files from the package:
atlas_install_headers( TestLArHardwareID )
atlas_install_joboptions( share/*.py )

