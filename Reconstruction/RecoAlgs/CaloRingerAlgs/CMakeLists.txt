################################################################################
# Package: CaloRingerAlgs
################################################################################

# Declare the package name:
atlas_subdir( CaloRingerAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Control/AthContainers
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloRings
                          Event/xAOD/xAODCore
                          Reconstruction/RecoTools/CaloRingerTools )

# Component(s) in the package:
atlas_add_component( CaloRingerAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel CaloEvent AthContainers AthenaBaseComps AthenaKernel StoreGateLib SGtests xAODBase xAODCaloRings xAODCore CaloRingerToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/CaloRingerAlgs_TestConfiguration.xml share/NeuralNetTests_Ref.txt )

