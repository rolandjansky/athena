################################################################################
# Package: LArCalibTest
################################################################################

# Declare the package name:
atlas_subdir( LArCalibTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArCabling
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/StoreGate
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArRawConditions
                          Tools/PathResolver )

# Component(s) in the package:
atlas_add_component( LArCalibTest
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloConditions CaloIdentifier AthenaBaseComps GaudiKernel LArIdentifier LArRawEvent LArRecConditions LArCablingLib CaloDetDescrLib StoreGateLib SGtests xAODEventInfo LArRawConditions PathResolver )

# Install files from the package:
atlas_install_headers( LArCalibTest )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

