# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloCellCorrection )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( CaloCellCorrection
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloCondBlobObjs CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloInterfaceLib CaloUtilsLib GaudiKernel Identifier StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

