################################################################################
# Package: LArClusterRec
################################################################################

# Declare the package name:
atlas_subdir( LArClusterRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloRec
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArTools
                          PRIVATE
                          Calorimeter/CaloUtils
                          Control/AthContainers
                          DetectorDescription/Identifier
                          LArCalorimeter/LArRawEvent )

# Component(s) in the package:
atlas_add_component( LArClusterRec
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CaloRecLib AthenaBaseComps StoreGateLib SGtests xAODCaloEvent GaudiKernel LArIdentifier LArRecEvent LArToolsLib CaloUtilsLib AthContainers Identifier LArRawEvent )

# Install files from the package:
atlas_install_headers( LArClusterRec )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

