# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DiTauRec )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_component( DiTauRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${FASTJET_LIBRARIES} AthenaBaseComps xAODEgamma xAODJet xAODMuon xAODTau GaudiKernel JetEDM TrkToolInterfaces CaloEvent xAODTracking RecoToolInterfaces StoreGateLib tauRecToolsLib TrkParametersIdentificationHelpers )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
