################################################################################
# Package: GoodRunsListsUser
################################################################################

# Declare the package name:
atlas_subdir( GoodRunsListsUser )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DataQuality/GoodRunsLists
                          Event/EventInfo
                          GaudiKernel
                          Trigger/TrigAnalysis/TrigDecisionTool
                          Trigger/TrigEvent/TrigDecisionEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_component( GoodRunsListsUser
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GoodRunsListsLib EventInfo GaudiKernel TrigDecisionToolLib TrigDecisionEvent )

# Install files from the package:
atlas_install_headers( GoodRunsListsUser )
atlas_install_joboptions( share/*.py )

