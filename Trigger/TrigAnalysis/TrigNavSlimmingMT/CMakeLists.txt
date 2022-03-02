 # Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigNavSlimmingMT )

atlas_add_component( TrigNavSlimmingMT
    src/*.cxx src/components/*.cxx
    LINK_LIBRARIES GaudiKernel AthAnalysisBaseCompsLib TrigCompositeUtilsLib TrigDecisionToolLib TrigTimeAlgsLib
    xAODParticleEvent xAODTrigMissingET )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )