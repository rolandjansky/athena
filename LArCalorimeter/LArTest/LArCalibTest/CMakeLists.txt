# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCalibTest )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )

# Component(s) in the package:
atlas_add_component( LArCalibTest
                     src/LAr*.cxx
                     src/DumpCaloBadChannels.cxx
                     src/FixLArElecCalib.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloConditions CaloDetDescrLib CaloIdentifier GaudiKernel Identifier LArCOOLConditions LArCablingLib LArElecCalib LArIdentifier LArRawConditions LArRawEvent LArRecConditions PathResolver StoreGateLib xAODEventInfo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

