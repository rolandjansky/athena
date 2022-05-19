# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FlavourTaggingTests )

if (NOT XAOD_ANALYSIS)
  # Component(s) in the package:
  atlas_add_library( FlavourTaggingTestsLib
    src/*.cxx
    PUBLIC_HEADERS FlavourTaggingTests
    PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES AsgTools AthenaBaseComps xAODEventInfo xAODBTagging xAODJet xAODTracking GaudiKernel AthContainers TrigDecisionToolLib FlavorTagDiscriminants
    PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives )

  atlas_add_component( FlavourTaggingTests
    src/*.cxx
    src/components/*.cxx
    LINK_LIBRARIES AsgTools AthenaBaseComps xAODEventInfo xAODBTagging xAODJet xAODTracking GaudiKernel AthContainers TrigDecisionToolLib FlavorTagDiscriminants GeoPrimitives )

  # Install files from the package:
  atlas_install_python_modules( python/*.py )
  atlas_install_joboptions( share/*.py )
  atlas_install_scripts( test/*.sh test/*.py )
endif()

# add the test utility
atlas_add_executable(
  test-btagging
  util/test-btagging.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODTracking xAODBTagging xAODRootAccess
  )

atlas_add_executable(
  test-jet-links
  util/test-jet-links.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODTracking xAODBTagging xAODRootAccess
  )

atlas_add_executable(
  test-associated-iparticles
  util/test-associated-iparticles.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODTracking xAODRootAccess
  )
