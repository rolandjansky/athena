################################################################################
# Package: JetSelectorTools
################################################################################

# Declare the package name:
atlas_subdir( JetSelectorTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODJet
                          PhysicsAnalysis/AnalysisCommon/PATCore
                          Reconstruction/Jet/JetInterface
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread PyROOT )

# Component(s) in the package:
atlas_add_library( JetSelectorToolsLib
                   Root/*.cxx
                   PUBLIC_HEADERS JetSelectorTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODJet JetInterface PATCoreLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_component( JetSelectorTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet PATCoreLib JetInterface GaudiKernel JetSelectorToolsLib )

atlas_add_dictionary( JetSelectorToolsDict
                      JetSelectorTools/JetSelectorToolsDict.h
                      JetSelectorTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet PATCoreLib JetInterface GaudiKernel JetSelectorToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

