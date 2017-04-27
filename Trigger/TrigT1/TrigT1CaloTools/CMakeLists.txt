################################################################################
# Package: TrigT1CaloTools
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1CaloUtils
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloTriggerTool
                          Control/CxxUtils
                          Event/EventInfo
                          Tools/PathResolver
                          Trigger/TrigAnalysis/TrigAnalysisInterfaces
                          Trigger/TrigAnalysis/TrigBunchCrossingTool
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCalibToolInterfaces
                          Trigger/TrigT1/TrigT1CaloCondSvc
                          Trigger/TrigT1/TrigT1CaloMappingToolInterfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigT1CaloTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaBaseComps DataModel xAODTrigL1Calo GaudiKernel TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1CaloUtilsLib TrigT1Interfaces CaloIdentifier CaloTriggerToolLib CxxUtils EventInfo PathResolver TrigConfL1Data TrigT1CaloCalibConditions TrigT1CaloCalibToolInterfaces TrigT1CaloCondSvcLib )

# Install files from the package:
atlas_install_headers( TrigT1CaloTools )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root share/*.txt )

