################################################################################
# Package: TrigL1CaloUpgradeAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TrigL1CaloUpgradeAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigL1CaloUpgradeAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                                    CaloDetDescrLib CaloEvent CaloIdentifier AthenaBaseComps xAODEgamma xAODEventInfo
                                    xAODTracking xAODTrigger xAODTrigCalo xAODTruth LArCablingLib LArRawEvent RecoToolInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


