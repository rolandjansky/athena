################################################################################
# Package: EventBookkeeperTools
################################################################################

# Declare the package name:
atlas_subdir( EventBookkeeperTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCutFlow
                          GaudiKernel
                          PRIVATE
                          Control/SGTools
                          Control/StoreGate
                          Event/EventBookkeeperMetaData
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( EventBookkeeperTools
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaKernel xAODCutFlow GaudiKernel SGTools StoreGateLib SGtests EventBookkeeperMetaData EventInfo xAODEventInfo )

# Install files from the package:
atlas_install_headers( EventBookkeeperTools )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/PrintCutFlow.py )

