################################################################################
# Package: TrigL1CaloUpgrade
################################################################################

# Declare the package name:
atlas_subdir( TrigL1CaloUpgrade )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasPolicy
                          GaudiKernel
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/AthenaBaseComps
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigL1Calo
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArTools
			  LArCalorimeter/LArCabling )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigL1CaloUpgrade
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps xAODEgamma xAODTracking xAODTrigCalo xAODTrigL1Calo xAODTrigger xAODTruth LArRawEvent LArToolsLib LArCablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )



