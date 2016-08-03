################################################################################
# Package: AthAnalysisBaseComps
################################################################################

# Declare the package name:
atlas_subdir( AthAnalysisBaseComps )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Control/AthContainers
                          Database/IOVDbDataModel
                          GaudiKernel )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthAnalysisBaseCompsLib
                   src/*.cxx
                   PUBLIC_HEADERS AthAnalysisBaseComps
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps IOVDbDataModel GaudiKernel StoreGateLib SGtests AthContainers
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( AthAnalysisBaseComps
                     src/components/*.cxx
                     PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES AthenaBaseComps IOVDbDataModel GaudiKernel StoreGateLib SGtests AthContainers AthAnalysisBaseCompsLib
                     PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES})

atlas_add_dictionary( AthAnalysisBaseCompsDict
                      AthAnalysisBaseComps/AthAnalysisBaseCompsDict.h
                      AthAnalysisBaseComps/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib SGtests IOVDbDataModel GaudiKernel AthContainers AthAnalysisBaseCompsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

