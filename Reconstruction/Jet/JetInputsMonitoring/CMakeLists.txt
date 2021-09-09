################################################################################
# Package: JetInputsMonitoring
################################################################################

# Declare the package name:
atlas_subdir( JetInputsMonitoring )

# External dependencies:
#find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( JetInputsMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODCaloEvent xAODPFlow GaudiKernel JetInterface AthContainers AthenaMonitoringLib JetUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )


