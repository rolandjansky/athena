# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FlavourTaggingTests )

# External dependencies:
find_package( Eigen )
#find_package( CLHEP )
#find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

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
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AsgTools AthenaBaseComps xAODEventInfo xAODBTagging xAODJet xAODTracking GaudiKernel AthContainers TrigDecisionToolLib FlavorTagDiscriminants GeoPrimitives )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.sh test/*.py )
