# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArClusterRec )

# Component(s) in the package:
atlas_add_component( LArClusterRec
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CaloRecLib AthenaBaseComps StoreGateLib xAODCaloEvent GaudiKernel LArIdentifier LArRecEvent LArCablingLib CaloUtilsLib AthContainers Identifier LArRawEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

