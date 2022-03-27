# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloUtilsLib FourMomUtils GaudiKernel MuidEvent MuidInterfaces StoreGateLib TrkMaterialOnTrack TrkTrack muonEvent )
