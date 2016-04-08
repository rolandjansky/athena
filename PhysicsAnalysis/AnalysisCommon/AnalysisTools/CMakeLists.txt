################################################################################
# Package: AnalysisTools
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/DataModel
                          Control/StoreGate
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          PRIVATE
                          Control/SGTools
                          Database/APR/RootCollection
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/PersistentDataModel
                          Event/EventInfo )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( PythonLibs )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthAnalysisToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisTools
                   INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${PYTHON_LIBRARIES} AthenaBaseComps DataModel NavFourMom GaudiKernel McParticleEvent StoreGateLib SGtests AnalysisUtilsLib RootCollectionComponents
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} SGTools RootCollection AthenaPoolUtilities PersistentDataModel EventInfo )

atlas_add_component( AthAnalysisTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps DataModel StoreGateLib SGtests NavFourMom GaudiKernel AnalysisUtilsLib McParticleEvent SGTools RootCollection RootCollectionComponents AthenaPoolUtilities PersistentDataModel EventInfo AthAnalysisToolsLib )

atlas_add_dictionary( AthAnalysisToolsDict
                      AnalysisTools/AnalysisToolsDict.h
                      AnalysisTools/selection.xml
                      INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${CORAL_LIBRARIES} ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps DataModel StoreGateLib SGtests NavFourMom GaudiKernel AnalysisUtilsLib McParticleEvent SGTools RootCollection RootCollectionComponents AthenaPoolUtilities PersistentDataModel EventInfo AthAnalysisToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

