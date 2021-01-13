# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( D2PDMaker )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist )

# Component(s) in the package:
atlas_add_component( D2PDMaker
   D2PDMaker/*.h D2PDMaker/*.icc src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES}
   AtlasHepMCLib CaloEvent AthenaBaseComps AthenaKernel AthContainers AthLinks
   Navigation EventKernel NavFourMom GaudiKernel
   GeneratorObjects AnalysisUtilsLib PATCoreLib ParticleEvent AssociationKernel
   McParticleEvent muonEvent Particle egammaEvent tauEvent VxVertex
   GeoPrimitives TruthHelper TrkTrackSummary )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
