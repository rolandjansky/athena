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
                          Control/CxxUtils
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/AthenaKernel
                          Control/StoreGate
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArIdentifier
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_component( TestLArHardwareID
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CxxUtils AthenaBaseComps GaudiKernel CaloIdentifier CaloTriggerToolLib AthenaKernel StoreGateLib SGtests LArCablingLib LArIdentifier PathResolver )

# Install files from the package:
atlas_install_headers( TestLArHardwareID )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/FCal-online-map.txt )

