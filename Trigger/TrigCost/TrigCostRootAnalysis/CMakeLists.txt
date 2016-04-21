################################################################################
# Package: TrigCostRootAnalysis
################################################################################

# Declare the package name:
atlas_subdir( TrigCostRootAnalysis )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigCost/TrigCostD3PD
                          PRIVATE
                          Tools/PathResolver
                          Trigger/TrigAnalysis/TrigRootAnalysis )

# External dependencies:
find_package( ROOT COMPONENTS Graf Gpad MathCore XMLIO XMLParser Core Tree Hist RIO pthread )
find_package( xml )

# Component(s) in the package:
atlas_add_library( TrigCostRootAnalysisLib
                   Root/*.cxx
                   PUBLIC_HEADERS TrigCostRootAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} TrigCostD3PD ${XML_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES PathResolver TrigRootAnalysis )

atlas_add_executable( RunTrigCostD3PD
                      util/RunTrigCostD3PD.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XML_LIBRARIES} TrigCostD3PD PathResolver TrigRootAnalysis TrigCostRootAnalysisLib )

atlas_add_executable( TrigCostD3PD_UserSkeleton
                      util/TrigCostD3PD_UserSkeleton.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XML_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${XML_LIBRARIES} TrigCostD3PD PathResolver TrigRootAnalysis TrigCostRootAnalysisLib )

# Install files from the package:
atlas_install_runtime( data/*.xml )

