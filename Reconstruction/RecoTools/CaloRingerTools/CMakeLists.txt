################################################################################
# Package: CaloRingerTools
################################################################################

# Declare the package name:
atlas_subdir( CaloRingerTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODCaloRings
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloUtils
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/xAOD/xAODEgamma
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          PhysicsAnalysis/RingerSelectorTools )

atlas_add_library( CaloRingerToolsLib
                   CaloRingerTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS CaloRingerTools
                   LINK_LIBRARIES GaudiKernel AthLinks StoreGateLib xAODBase xAODCaloRings xAODCaloEvent )

# Component(s) in the package:
atlas_add_component( CaloRingerTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloRingerToolsLib xAODCaloRings CaloEvent CaloGeoHelpers CaloUtilsLib AthenaBaseComps AthenaKernel CxxUtils xAODEgamma PATCoreLib RingerSelectorToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

