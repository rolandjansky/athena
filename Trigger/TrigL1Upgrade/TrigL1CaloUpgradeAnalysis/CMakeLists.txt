################################################################################
# Package: TrigL1CaloUpgradeAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TrigL1CaloUpgradeAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArRawEvent
                          Reconstruction/RecoTools/RecoToolInterfaces
                          Trigger/TrigL1Upgrade/TrigL1CaloUpgrade )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigL1CaloUpgradeAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                                    CaloDetDescr CaloEvent CaloIdentifier AthenaBaseComps xAODEgamma xAODEventInfo
                                    xAODTracking xAODTrigger xAODTrigCalo xAODTruth LArCabling LArRawEvent RecoToolInterfaces TrigL1CaloUpgradeLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


