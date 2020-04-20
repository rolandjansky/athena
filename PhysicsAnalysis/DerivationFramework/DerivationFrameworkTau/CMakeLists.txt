################################################################################
# Package: DerivationFrameworkTau
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkTau )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
                          PRIVATE
                          Event/xAOD/xAODTau
                          PhysicsAnalysis/JpsiUpsilonTools
                          PhysicsAnalysis/TauID/TauAnalysisTools )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkTauLib
                   src/*.cxx
                   PUBLIC_HEADERS DerivationFrameworkTau
                   LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel JpsiUpsilonToolsLib
                   PRIVATE_LINK_LIBRARIES xAODTau )

atlas_add_component( DerivationFrameworkTau
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps xAODTracking GaudiKernel xAODTau JpsiUpsilonToolsLib DerivationFrameworkTauLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

