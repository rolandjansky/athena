################################################################################
# Package: EventTagUtils
################################################################################

# Declare the package name:
atlas_subdir( EventTagUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Control/AthenaKernel
                          Database/AthenaPOOL/DBDataModel
                          Event/EventInfo
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTracking
                          LumiBlock/LumiBlockComps
                          Reconstruction/RecEvent
                          Trigger/TrigConfiguration/TrigConfInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( EventTagUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS EventTagUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaPoolUtilities xAODEventInfo GaudiKernel TagEvent StoreGateLib SGtests TrigDecisionToolLib LumiBlockCompsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel DBDataModel EventInfo xAODCaloEvent xAODTracking RecEvent )

atlas_add_component( EventTagUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities xAODEventInfo GaudiKernel TagEvent TrigDecisionToolLib AthenaKernel DBDataModel EventInfo xAODCaloEvent xAODTracking LumiBlockCompsLib RecEvent EventTagUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

