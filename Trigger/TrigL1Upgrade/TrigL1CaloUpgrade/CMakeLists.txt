################################################################################
# Package: TrigL1CaloUpgrade
################################################################################

# Declare the package name:
atlas_subdir( TrigL1CaloUpgrade )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODTruth
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArRawEvent
                          Event/xAOD/xAODTrigger )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigL1CaloUpgradeLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigL1CaloUpgrade
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                                          CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps xAODEventInfo
                                          xAODTracking xAODTrigCalo xAODTrigL1Calo xAODTruth LArCablingLib LArRawEvent xAODTrigger )

atlas_add_component( TrigL1CaloUpgrade
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} TrigL1CaloUpgradeLib
                                    CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps xAODEventInfo
                                    xAODTracking xAODTrigCalo xAODTrigL1Calo xAODTruth LArCablingLib LArRawEvent xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

