# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArOnlDbPrep )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( LArOnlDbPrep
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CaloIdentifier CaloTriggerToolLib AthenaBaseComps GaudiKernel LArRecConditions LArCablingLib CaloDetDescrLib StoreGateLib AthenaPoolUtilities LArCOOLConditions LArIdentifier CaloInterfaceLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.sh )

