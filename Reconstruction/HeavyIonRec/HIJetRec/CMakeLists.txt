# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HIJetRec )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist Physics )

# Component(s) in the package:
atlas_add_library( HIJetRecLib
                   HIJetRec/*.h
                   INTERFACE
                   PUBLIC_HEADERS HIJetRec
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools FourMomUtils HIEventUtilsLib xAODBase xAODCaloEvent xAODHIEvent xAODJet )

atlas_add_component( HIJetRec
                     Root/*.cxx src/*.h src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps CaloEvent CaloInterfaceLib CaloRecLib CaloUtilsLib CxxUtils HIJetRecLib JetInterface JetRecLib NavFourMom Navigation PathResolver StoreGateLib xAODCore xAODEventInfo xAODTracking )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
