# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( egammaD3PDMaker )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( egammaD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib CaloIdentifier AthenaKernel Navigation StoreGateLib EventKernel FourMomUtils xAODBase xAODCaloEvent xAODEgamma xAODJet xAODTracking xAODTruth GaudiKernel AnalysisTriggerEvent D3PDMakerInterfaces D3PDMakerUtils TriggerD3PDMakerLib MCTruthClassifierLib egammaEvent TrkParameters TrkExInterfaces TrkVertexFitterInterfaces CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
