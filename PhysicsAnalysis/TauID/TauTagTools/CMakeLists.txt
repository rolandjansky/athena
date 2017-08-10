################################################################################
# Package: TauTagTools
################################################################################

# Declare the package name:
atlas_subdir( TauTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent
                          PRIVATE
                          Control/StoreGate
                          Event/xAOD/xAODTau
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PhysicsAnalysis/TauID/TauAnalysisTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TauTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TauTagTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent StoreGateLib SGtests AnalysisUtilsLib TauAnalysisToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} xAODTau )

atlas_add_component( TauTagTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent StoreGateLib SGtests xAODTau AnalysisUtilsLib TauTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

