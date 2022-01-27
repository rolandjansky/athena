# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BTagging )

# Component(s) in the package:
atlas_add_library( BTaggingLib
                   src/BTagTool.cxx
                   src/BTagSecVertexing.cxx
                   src/BTagLightSecVertexing.cxx
                   src/JetBTaggingAlg.cxx
                   src/BTagJetPtScaling.cxx
                   src/JetSecVertexingAlg.cxx
                   src/JetSecVtxFindingAlg.cxx
                   src/BTagTrackAugmenterAlg.cxx
                   PUBLIC_HEADERS BTagging
                   LINK_LIBRARIES AsgTools AthContainers AthenaBaseComps CxxUtils FlavorTagDiscriminants GaudiKernel GeoPrimitives JetInterface JetRecLib MagFieldConditions ParticleJetToolsLib StoreGateLib TrkExInterfaces TrkVertexFitterInterfaces VxSecVertex VxVertex xAODBTagging xAODEventInfo xAODJet xAODMuon xAODTracking
                   PRIVATE_LINK_LIBRARIES InDetRecToolInterfaces JetTagEvent JetTagToolsLib Particle TrkLinks TrkSurfaces VxJetVertex xAODBase xAODCore )

atlas_add_component( BTagging
                     src/components/*.cxx
                     LINK_LIBRARIES BTaggingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} ) 
atlas_install_joboptions( share/*.py )

