################################################################################
# Package: AnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthContainers
                          Control/StoreGate
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PRIVATE
                          Control/SGTools
                          Control/RootUtils
                          Database/APR/RootCollection
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/PersistentDataModel
                          Event/xAOD/xAODEventInfo
                          Event/EventInfo )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Python COMPONENTS Development )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthAnalysisToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisTools
                   INCLUDE_DIRS ${Python_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Python_LIBRARIES} AthenaBaseComps AthContainers NavFourMom GaudiKernel McParticleEvent StoreGateLib SGtests AnalysisUtilsLib RootCollection
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} SGTools RootCollection AthenaPoolUtilities PersistentDataModel xAODEventInfo EventInfo )

atlas_add_component( AthAnalysisTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${Python_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthContainers StoreGateLib SGtests NavFourMom GaudiKernel AnalysisUtilsLib McParticleEvent SGTools RootCollection AthenaPoolUtilities PersistentDataModel xAODEventInfo EventInfo AthAnalysisToolsLib )

atlas_add_dictionary( AthAnalysisToolsDict
                      AnalysisTools/AnalysisToolsDict.h
                      AnalysisTools/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${Python_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} ${Python_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthContainers StoreGateLib SGtests NavFourMom GaudiKernel AnalysisUtilsLib McParticleEvent SGTools RootCollection RootCollectionComponents AthenaPoolUtilities PersistentDataModel xAODEventInfo EventInfo AthAnalysisToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

