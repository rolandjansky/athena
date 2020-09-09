################################################################################
# Package: MuidCaloEnergyTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCaloEnergyTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuidCaloEnergyTools
                     src/MuidCaloEnergyTool.cxx
                     src/MuidCaloEnergyMeas.cxx
                     src/MuidCaloEnergyParam.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel MuidInterfaces CaloEvent CaloIdentifier CaloUtilsLib MuidEvent muonEvent TrkMaterialOnTrack TrkTrack )

# Install files from the package:
atlas_install_headers( MuidCaloEnergyTools )

