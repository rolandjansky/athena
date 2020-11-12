# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArDigitization )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( LArDigitizationLib
                   src/*.cxx
                   PUBLIC_HEADERS LArDigitization
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES CaloIdentifier AthenaBaseComps
                   AthenaKernel xAODEventInfo GaudiKernel
                   LArElecCalib LArRawEvent LArSimEvent
                   LArRecConditions LArRawConditions CaloDetDescrLib
                   PileUpToolsLib StoreGateLib LArCablingLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib EventInfoUtils
                   GeneratorObjects Identifier LArIdentifier )


atlas_add_component( LArDigitization
                     src/components/*.cxx
                     LINK_LIBRARIES LArDigitizationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
