################################################################################
# Package: LArCalibDataQuality
################################################################################

# Declare the package name:
atlas_subdir( LArCalibDataQuality )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          LArCalorimeter/LArRawConditions
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArTools
                          PRIVATE
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent )

# Component(s) in the package:
atlas_add_component( LArCalibDataQuality
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloIdentifier AthenaBaseComps StoreGateLib SGtests GaudiKernel LArRawConditions LArRecConditions LArToolsLib xAODEventInfo LArIdentifier LArRawEvent )

# Install files from the package:
atlas_install_headers( LArCalibDataQuality )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

