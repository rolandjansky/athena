################################################################################
# Package: egammaD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( egammaD3PDMaker )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( egammaD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib CaloIdentifier AthenaKernel Navigation StoreGateLib SGtests EventKernel FourMomUtils xAODBase xAODCaloEvent xAODEgamma xAODJet xAODTracking xAODTruth GaudiKernel AnalysisTriggerEvent D3PDMakerUtils TriggerD3PDMakerLib MCTruthClassifierLib egammaEvent TrkParameters TrkExInterfaces TrkVertexFitterInterfaces TrigObjectMatchingLib TrigCaloEvent TrigInDetEvent TrigParticle CaloTrackingGeometryLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

