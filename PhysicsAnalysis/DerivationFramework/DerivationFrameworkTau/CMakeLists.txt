################################################################################
# Package: DerivationFrameworkTau
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTau )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkTauLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkTau
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel JpsiUpsilonToolsLib TauAnalysisToolsLib DerivationFrameworkInterfaces
                   PRIVATE_LINK_LIBRARIES xAODTau)

atlas_add_component( DerivationFrameworkTau
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel xAODTau JpsiUpsilonToolsLib DerivationFrameworkTauLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
