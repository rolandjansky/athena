################################################################################
# Package: egammaD3PDAnalysis
################################################################################

# Declare the package name:
atlas_subdir( egammaD3PDAnalysis )

# Component(s) in the package:
atlas_add_component( egammaD3PDAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloClusterCorrectionLib CaloEvent CaloGeoHelpers CaloRecLib AthContainers AthenaBaseComps AthenaKernel EventKernel xAODCaloEvent xAODEgamma xAODTruth GaudiKernel LArCablingLib D3PDMakerUtils MCTruthClassifierLib RecoToolInterfaces egammaEvent TrkCaloExtension VxVertex egammaInterfacesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

