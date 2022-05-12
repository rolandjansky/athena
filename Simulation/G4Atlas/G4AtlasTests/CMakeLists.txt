# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

################################################################################
# Package: G4AtlasTests
################################################################################

# Declare the package name:
atlas_subdir( G4AtlasTests )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_library( G4AtlasTests
                   src/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${GEANT4_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${GEANT4_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel CaloDetDescrLib CaloIdentifier CaloSimEvent AthenaKernel StoreGateLib GeoAdaptors GeoPrimitives Identifier xAODEventInfo ALFA_SimEv LUCID_SimEvent ZDC_SimEvent GeneratorObjects InDetSimEvent LArSimEvent MuonReadoutGeometry MuonIdHelpersLib MuonSimEvent McParticleEvent G4AtlasToolsLib MCTruth HitManagement TileDetDescr TileIdentifier TileSimEvent TrackRecordLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/sim_*.py )

