# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigL1CaloUpgrade )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree Hist RIO )

# Component(s) in the package:
atlas_add_component( TrigL1CaloUpgrade
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps CaloDetDescrLib CaloEvent CaloIdentifier GaudiKernel LArCablingLib LArRawEvent StoreGateLib xAODEgamma xAODTracking xAODTrigCalo xAODTrigL1Calo xAODTrigger xAODTruth )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )



